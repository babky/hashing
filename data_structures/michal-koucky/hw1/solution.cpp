/*
 * solution.cpp
 *
 *  Created on: Sep 5, 2015
 *	  Author: Martin Babka
 */

#include <iostream>
#include <cassert>
#include <string>
#include <vector>
#include <algorithm>
#include <string>
#include <fstream>
#include <sstream>
#include <cstdio>
#include <cstdlib>

// #define LOG_SUPPORT

class Log {
#ifdef LOG_SUPPORT
	bool output;
#endif

public:
	bool is_outputting() {
#ifdef LOG_SUPPORT
		return output;
#else
		return false;
#endif

	}

public:
	Log(bool show) {
#ifdef LOG_SUPPORT
		output = show;
#endif
	}
};

#ifdef LOG_SUPPORT

template<typename T>
Log & operator <<(Log & log, const T & x) {
	if (log.is_outputting()) {
		std::cerr << x;
	}

	return log;
}

#else

template<typename T>
Log & operator <<(Log & log, const T & x) {
}

#endif

Log logger(false);
Log logger_trace(false);

struct Stats {

	std::size_t output_size = 0;
	std::size_t input_size = 0;
	std::size_t inner_reductions = 0;

};

// Perform the inner sort of large blocks.
// Perform the merge until the output is sorted.

template<typename Key, typename Value, class Push>
class MinReducer {
	bool first;
	Key prevKey;
	Value prevValue;
	Push push;
	std::size_t reduction_count = 0;

public:
	MinReducer(Push pusher):
		first(false),
		push(pusher) {
	}

	void start() {
		first = true;
	}

	Push & get_pusher() {
		return push;
	}

	void add(Key key, Value value) {
		if (first) {
			// In the first key, value pair, just store them and remember that we are no longer in the first mode.
			first = false;
			prevValue = value;
		} else if (key != prevKey) {
			// Push the previous key/value pair to the output.
			push(prevKey, prevValue);
			prevValue = value;
		} else {
			if (prevValue > value) {
				// In the case of the same key, just remember the minimum.
				prevValue = value;
			}

			++reduction_count;
			logger_trace << "Reduction\n";
		}

		prevKey = key;
	}

	std::size_t get_reduction_count() {
		return reduction_count;
	}

	void stop() {
		if (!first) {
			push(prevKey, prevValue);
		}
	}
};

template<typename Key, typename Value, class Push>
class NoReducer {
	Push push;

public:
	NoReducer(Push pusher):
		push(pusher) {
	}

	void start() {
	}

	void add(Key key, Value value) {
		push(key, value);
	}

	void stop() {
	}

	Push & get_pusher() {
		return push;
	}

	std::size_t get_reduction_count() {
		return 0;
	}

};

template<typename Key, typename Value>
class VectorPusher {
public:
	typedef std::vector<std::pair<Key, Value>> VectorType;

private:
	VectorType & out;

public:
	VectorPusher(VectorType& out_vector):
		out(out_vector) {
	}

	void operator()(Key key, Value value) {
		out.push_back(std::pair<Key, Value>(key, value));
	}
};

template<typename Key, typename Value>
class BinaryOutputPusher {
	std::ostream& out;
	std::ostream::pos_type block_size_pos;
	std::size_t length = 0;
	std::size_t push_count = 0;

public:
	BinaryOutputPusher(std::ostream & out_stream):
		out(out_stream),
		block_size_pos(out_stream.tellp()) {
	}

	void mark_size(std::size_t size) {
		std::ostream::pos_type current_pos = out.tellp();
		out.seekp(block_size_pos);
		out.write((char *) &size, sizeof(size));
		out.seekp(current_pos);
		logger_trace << "Writing length of the run " << size << ".\n";
	}

	void operator()(Key key, Value value) {
		++length;
		++push_count;
		out.write((char *) &key, sizeof(key));
		out.write((char *) &value, sizeof(value));

		logger_trace << "Pushing " << key << " " << value << ", having length " << length << ".\n";
	}

	void finish_run() {
		mark_size(length);
	}

	void start_run() {
		block_size_pos = out.tellp();
		length = 0;
		out.write((char *) &length, sizeof(length));
		logger_trace << "Starting a new run.\n";
	}

	std::size_t get_push_count() const {
		return push_count;
	}
};

template<typename Key, typename Value>
void read_text(std::istream & in, Key & key, Value & value) {
	in >> key >> value;
}

template<typename Key, typename Value>
void read_binary(std::istream & in, Key & key, Value & value) {
	in.read((char *) &key, sizeof(key));
	in.read((char *) &value, sizeof(value));
}

template<typename Key, typename Value>
class TextOutputPusher {
	std::ostream& out;
	std::size_t push_count = 0;

public:
	TextOutputPusher(std::ostream & out_stream):
		out(out_stream) {
	}

	void finish_run() {
		out << "-\n";
	}

	void start_run() {
	}

	void operator()(Key key, Value value) {
		++push_count;
		logger_trace << "Outputting " << key << " " << value << ".\n";
		out << key << " " << value << "\n";
	}

	std::size_t get_push_count() const {
		return push_count;
	}

};

template<typename Key, typename Value>
class BufferedBinaryRun {
public:
	static const size_t BUFFER_SIZE = 128;

	std::size_t current_pair;
	std::size_t buffer_size;
	std::pair<Key, Value> buffer[BUFFER_SIZE];
	std::fstream * in;
	std::size_t length;

public:
	typedef BinaryOutputPusher<Key, Value> Pusher;

	const static std::ios::openmode READ_MODE = std::ios::binary | std::ios::in;
	const static std::ios::openmode WRITE_MODE = std::ios::binary | std::ios::out | std::ios::trunc;

public:
	BufferedBinaryRun(std::fstream & input_file):
		in(&input_file), 
		current_pair(0), 
		buffer_size(0) {
		in->read((char *) &length, sizeof(length));
		logger_trace << "Having run of length " << length << ".\n";
	}

	BufferedBinaryRun():
		current_pair(0),
		buffer_size(0),
		length(0),
		in(0) {
	}

	std::pair<Key, Value> & current() {
		logger_trace << "Current pair.\n";
		return buffer[current_pair];
	}

	void next() {
		logger_trace << "Next pair.\n";
		++current_pair;	
		if (current_pair < buffer_size) {
			return;
		}

		buffer_size = length < BUFFER_SIZE ? length : BUFFER_SIZE;
		current_pair = 0;
		assert(!in->eof());
		in->read((char*) buffer, sizeof(std::pair<Key, Value>) * buffer_size);
		length -= buffer_size;
	}

	bool has_next() {
		logger_trace << "Has next: " << (buffer_size != current_pair || length != 0) << ".\n";
		return current_pair + 1 < buffer_size || length != 0;
	}
};

template<typename Key, typename Value>
class BinaryRun {
	std::pair<Key, Value> current_pair;
	std::fstream * in;
	std::size_t length;

public:
	typedef BinaryOutputPusher<Key, Value> Pusher;

	const static std::ios::openmode READ_MODE = std::ios::binary | std::ios::in;
	const static std::ios::openmode WRITE_MODE = std::ios::binary | std::ios::out | std::ios::trunc;

public:
	BinaryRun(std::fstream & input_file):
		in(&input_file) {
		in->read((char *) &length, sizeof(length));
		logger_trace << "Having run of length " << length << ".\n";
	}

	BinaryRun():
		in(0) {
		length = 0;
	}

	std::pair<Key, Value> & current() {
		return current_pair;
	}

	void next() {
		assert(!in->eof());
		read_binary(*in, current_pair.first, current_pair.second);
		--length;
	}

	bool has_next() {
		return length != 0;
	}
};

template<typename Key, typename Value>
class TextRun {
	std::pair<Key, Value> current_pair;
	std::fstream * in;
	bool next_flag_current = false;
	bool next_flag = true;

public:
	typedef TextOutputPusher<Key, Value> Pusher;

	const static std::ios::openmode READ_MODE = std::ios::in;
	const static std::ios::openmode WRITE_MODE = std::ios::out | std::ios::trunc;

public:
	TextRun(std::fstream & input_file):
		in(&input_file) {
	}

	TextRun():
		in(0) {
		next_flag = false;
		next_flag_current = true;
	}

	std::pair<Key, Value> & current() {
		return current_pair;
	}

	void next() {
		assert(next_flag == true);
		has_next();
		next_flag_current = false;
	}

	bool has_next() {
		if (!next_flag_current) {
			next_flag_current = true;

			std::string str;
			(*in) >> str;

			if (str == "-") {
				next_flag = false;

				while (std::isspace(in->peek())) {
					in->get();
				}
			} else {
				std::stringstream ss;
				ss << str;
				ss >> current_pair.first;
				(*in) >> current_pair.second;
				next_flag = true;
			}

			logger_trace << "Having key/value: " << current_pair.first << " " << current_pair.second << "\n";
			logger_trace << (next_flag ? "We have a next element." : "No next element.") << "\n";
		}

		return next_flag;
	}
};

template<typename Key, typename Value, template <class, class> class Run>
class RunSource {
	std::fstream & in;

public:
	RunSource(std::fstream & input_file):
		in(input_file) {
	}

	Run<Key, Value> next() {
		assert(!in.eof());
		return Run<Key, Value>(in);
	}

	bool has_next() {
		in.peek();
		return !in.eof();
	}
};

template<typename Key, typename Value, template<class, class, class> class Reducer, class Pusher, template <class, class> class RunType>
class Merger {
private:
	std::vector<RunSource<Key, Value, RunType>> sources;
	Pusher push;
	Stats & stats;

public:
	Merger(std::vector<RunSource<Key, Value, RunType>> run_sources, Pusher pusher, Stats & statsa):
		sources(run_sources),
		push(pusher),
		stats(statsa) {
	}

	Pusher & get_pusher() {
		return push;
	}

	std::size_t merge() {
		std::size_t run = 0;
		std::vector<std::pair<RunType<Key, Value>, bool>> runs;
		for (std::size_t i = 0; i < sources.size(); ++i) {
			if (sources[i].has_next()) {
				runs.push_back(std::pair<RunType<Key, Value>, bool>(sources[i].next(), true));
				runs[i].first.next();
			} else {
				runs.push_back(std::pair<RunType<Key, Value>, bool>(RunType<Key, Value>(), false));
			}
		}

		logger << "Runs created.\n";

		for (bool found_new_run = true; found_new_run;) {
			++run;
			create_new_run(runs);

			found_new_run = false;
			for (std::size_t i = 0; i < sources.size(); ++i) {
				if (sources[i].has_next()) {
					logger << "Run in the file " << i << ".\n";
					runs[i] = std::pair<RunType<Key, Value>, bool>(sources[i].next(), true);
					logger << "Reading the first key-value-pair in the run of file " << i << ".\n";
					if (runs[i].first.has_next()) {
						runs[i].first.next();
						found_new_run = true;
						logger << "Found a new run in the input file " << i << ". We will start a next round.\n";
					}
				}
			}

			logger << "Finished creating a run.\n";
		}

		logger << "Created " << run << " new runs.\n";

		return run;
	}

private:
	std::size_t next_power(std::size_t v) {
			v--;
			v |= v >> 1;
			v |= v >> 2;
			v |= v >> 4;
			v |= v >> 8;
			v |= v >> 16;
			v++;
			return v;
	}

	std::size_t parent(std::size_t c) {
		return (c - 1) / 2;
	}

	std::size_t lchild(std::size_t p) {
		return p * 2 + 1;
	}

	std::size_t rchild(std::size_t p) {
		return lchild(p) + 1;
	}

	struct MinKeyInfo {
		std::size_t run;
		Key key;
		bool enabled;

		MinKeyInfo():
			run(0),
			enabled(false) {
		}

		MinKeyInfo(std::size_t run_value, Key key_value, bool enabled_value):
			run(run_value),
			key(key_value),
			enabled(enabled_value) {
		}
	};

	void fix_min(MinKeyInfo * mins, std::size_t l, std::size_t r, std::size_t p) {
		if (mins[l].enabled) {
			if (mins[r].enabled) {
				mins[p] = mins[r].key < mins[l].key ? mins[r] : mins[l];
			} else {
				mins[p] = mins[l];
			}
		} else {
			mins[p] = mins[r];
		}
	}

	void create_new_run(std::vector<std::pair<RunType<Key, Value>, bool>> & runs) {
		logger << "Starting a new merge run.\n";
		Reducer<Key, Value, Pusher> reducer(push);
		reducer.start();
		reducer.get_pusher().start_run();

		// Create the minimums selector.
		std::size_t s = runs.size();
		std::size_t pow = next_power(s);
		MinKeyInfo * mins = new MinKeyInfo[2 * pow - 1];
		for (std::size_t i = 0; i < s; ++i) {
			if (runs[i].second) {
				mins[i + s - 1] = MinKeyInfo(i, runs[i].first.current().first, true);
			}
		}

		for (std::size_t i = s - 1; i != 0;) {
			--i;
			std::size_t l = lchild(i);
			std::size_t r = rchild(i);
			fix_min(mins, l, r, i);
		}

		std::size_t i, min_idx, p, o;
		for (; mins[0].enabled;) {
			min_idx = mins[0].run;
			reducer.add(runs[min_idx].first.current().first, runs[min_idx].first.current().second);

			i = s - 1 + min_idx;
			if (runs[min_idx].first.has_next()) {
				runs[min_idx].first.next();
				mins[i].key = runs[min_idx].first.current().first;
			} else {
				mins[i].enabled = false;
				runs[min_idx].second = false;
			}

			// Fix the path.
			for (; i != 0; i = p) {
				p = parent(i);

				if (i % 2 == 0) {
					o = i -1;
				} else {
					o = i + 1;
				}

				fix_min(mins, o, i, p);
			}
		}

		delete [] mins;

		/*
		for (;;) {
			std::pair<Key, Value> * min_key = 0;
			std::size_t min_index = 0;

			for (std::size_t i = 0; i < runs.size(); ++i) {
				if (runs[i].second && (min_key == 0 || min_key->first > runs[i].first.current().first)) {
					min_key = &runs[i].first.current();
					min_index = i;
				}
			}

			if (min_key == 0) {
				break;
			}

			logger_trace << "Adding key/value " << min_key->first << " " << min_key->second << ".\n";
			reducer.add(min_key->first, min_key->second);

			if (runs[min_index].first.has_next()) {
				runs[min_index].first.next();
				logger_trace << "Advancing in " << min_index << ".\n";
			} else {
				logger_trace << "Disabling " << min_index << ".\n";
				runs[min_index].second = false;
			}
		}
		*/

		reducer.stop();
		reducer.get_pusher().finish_run();

		stats.inner_reductions += reducer.get_reduction_count();
	}
};

template<typename Key, typename Value>
class PairKeyComparer {
public:
	bool operator()(const std::pair<Key, Value> & a, const std::pair<Key, Value> & b) {
		return a.first < b.first;
	}
};

namespace std {
	template <typename T>
	std::string to_string(T value) {
		std::ostringstream os;
		os << value;
		return os.str();
	}
}

template<typename Key, typename Value, template<class, class, class> class Reducer, template <class, class> class RunType>
class Sorter {
	typedef typename RunType<Key, Value>::Pusher Pusher;

	std::size_t max_elements_in_memory;
	std::size_t max_files;
	std::string temp_directory;
	std::vector<std::fstream *> streams;
	Stats stats;

public:
	Sorter(std::size_t temp_file_limit, std::string temporary_directory,
			std::size_t max_elements):
		max_elements_in_memory(max_elements),
		max_files(temp_file_limit),
		temp_directory(temporary_directory),
		streams(max_files) {
	}

	const Stats & get_stats() const {
		return stats;
	}

	void sort(std::ifstream & in, std::ostream & out) {
		clear_temp_files();

		// parse the data and sort them into the files as runs
		std::size_t runs = parse(in);
		if (runs == 1) {
			// write the output directly
			write_to_out(0, out);
		} else {
			// merge the runs
			logger << "Merging " << runs << " runs.\n";
			for (;;) {
				if (runs < this->max_files) {
					runs = merge_runs(out);
					assert(runs == 0);
				} else {
					runs = merge_runs();
				}

				if (runs > 1) {
					distribute_runs(runs);
				} else {
					break;
				}
			}

			// write the output, if necessary
			if (runs == 1) {
				assert(false); // however this should not happen.
				write_to_out(max_files - 1, out);
			}
		}

		clear_temp_files();
	}

private:
	void distribute_runs(std::size_t runs) {
		RunSource<Key, Value, RunType> run_sources(get_temp_file(max_files - 1, true, RunType<Key, Value>::READ_MODE));

		std::size_t i = 0;
		std::size_t runs_per_file = runs / (max_files - 1);
		if (runs_per_file == 0) {
			runs_per_file = 1;
		}
		std::size_t last_file = 0;
		std::size_t current_file = 0;
		Pusher * pusher = new Pusher(get_temp_file(current_file, true, RunType<Key, Value>::WRITE_MODE));

		while (run_sources.has_next()) {
			current_file = i / runs_per_file;
			if (last_file != current_file && current_file < max_files - 1) {
				delete pusher;
				pusher = new Pusher(get_temp_file(current_file, true, RunType<Key, Value>::WRITE_MODE));
				last_file = current_file;
			}

			RunType<Key, Value> run = run_sources.next();
			pusher->start_run();
			while (run.has_next()) {
				run.next();
				pusher->operator()(run.current().first, run.current().second);
			}
			pusher->finish_run();
			++i;
		}

		create_empty_files(current_file);

		delete pusher;
	}

	void create_empty_files(std::size_t current_file) {
		for (++current_file; current_file < max_files - 1; ++current_file) {
			get_temp_file(current_file, true, RunType<Key, Value>::WRITE_MODE);
		}
	}

	std::size_t parse(std::ifstream & in) {
		Key key;
		Value value;

		std::vector<std::pair<Key, Value>> data;
		data.reserve(max_elements_in_memory);
		VectorPusher<Key, Value> pusher(data);
		Reducer<Key, Value, VectorPusher<Key, Value>> reducer(pusher);

		reducer.start();
		std::size_t run = 0;

		for (;;) {
			read_text(in, key, value);
			if (in.eof()) {
				break;
			}

			++stats.input_size;
			reducer.add(key, value);

			if (data.size() == max_elements_in_memory) {
				sort_and_output_vector(data, run % (max_files - 1));
				run += 1;
				data.clear();
			}
		}
		reducer.stop();
		sort_and_output_vector(data, run % (max_files - 1));
		create_empty_files(run);
		run += 1;

		stats.inner_reductions += reducer.get_reduction_count();

		return run;
	}

	/**
	 * Runs the merger.
	 */
	std::size_t merge_runs() {
		// Create the run sources.
		std::vector<RunSource<Key, Value, RunType>> run_sources;
		for (int i = 0; i < max_files - 1; ++i) {
			std::fstream & fs = get_temp_file(i, true, RunType<Key, Value>::READ_MODE);
			run_sources.push_back(RunSource<Key, Value, RunType>(fs));
		}

		// Initialize the merger.
		Merger<Key, Value, Reducer, Pusher, RunType> merger(
			run_sources,
			Pusher(get_temp_file(max_files - 1, true, RunType<Key, Value>::WRITE_MODE)),
			stats
		);

		// Merge it.
		return merger.merge();
	}

	/**
	 * Runs the merger.
	 */
	std::size_t merge_runs(std::ostream & out) {
		logger << "Outputting directly after merge.\n";

		// Create the run sources.
		std::vector<RunSource<Key, Value, RunType>> run_sources;
		for (int i = 0; i < max_files - 1; ++i) {
			std::fstream & fs = get_temp_file(i, true, RunType<Key, Value>::READ_MODE);
			run_sources.push_back(RunSource<Key, Value, RunType>(fs));
		}

		// Initialize the merger.
		Merger<Key, Value, Reducer, TextOutputPusher<Key, Value>, RunType> merger(
			run_sources,
			TextOutputPusher<Key, Value>(out),
			stats
		);

		std::size_t runs = merger.merge();
		assert(runs == 1);
		stats.output_size = merger.get_pusher().get_push_count();

		return 0;
	}

	void write_to_out(std::size_t idx, std::ostream & out) {
		std::pair<Key, Value> * element;
		std::fstream & in = get_temp_file(idx, true, RunType<Key, Value>::READ_MODE);
		in.seekg(std::ios_base::beg);
		TextOutputPusher<Key, Value> text_pusher(out);
		MinReducer<Key, Value, TextOutputPusher<Key, Value>> reducer(text_pusher);

		RunType<Key, Value> run(in);
		reducer.start();
		while (run.has_next()) {
			run.next();
			element = &run.current();
			reducer.add(element->first, element->second);
		}
		reducer.stop();
		stats.output_size = reducer.get_pusher().get_push_count();
	}

	void sort_and_output_vector(std::vector<std::pair<Key, Value>> & data, std::size_t temp_file_idx) {
		// Sort internally.
		std::sort(data.begin(), data.end(), PairKeyComparer<Key, Value>());
		std::ostream & out = get_temp_file(temp_file_idx, false, RunType<Key, Value>::WRITE_MODE);

		// Write the block.
		Pusher pusher(out);

		Reducer<Key, Value, Pusher> reducer(pusher);
		reducer.get_pusher().start_run();
		reducer.start();
		for (int i = 0; i < data.size(); ++i) {
			reducer.add(data[i].first, data[i].second);
		}
		reducer.stop();
		reducer.get_pusher().finish_run();
		logger << "Internally sorted into file " << temp_file_idx << ".\n";
	}


	std::string get_temp_filename(std::size_t i) {
		return temp_directory + "/" + std::to_string(i) + ".tmp";
	}

	void clear_temp_files() {
		close_temp_files();

		std::string filename;
		for (std::size_t i = 0; i < max_files; ++i) {
			filename = get_temp_filename(i);
			std::remove(filename.c_str());
		}
	}

	void close_temp_files() {
		for (int i = 0; i < streams.size(); ++i) {
			if (streams[i] && streams[i]->is_open()) {
				delete streams[i];
				streams[i] = 0;
			}
		}
	}

	std::fstream& get_temp_file(std::size_t i, bool reopen, const std::ios::openmode mode) {
		if (streams[i] && reopen) {
			streams[i]->close();
			delete streams[i];
			streams[i] = 0;
		}

		if (streams[i] == 0) {
			std::string filename = get_temp_filename(i);
			logger << "Opening " << i << "-th file " << filename << ".\n";
			streams[i] = new std::fstream(filename.c_str(), mode);
		}

		return *streams[i];
	}
};

template <template<class, class, class> class Reducer, template <class, class> class RunType>
Stats sort(std::ifstream & fin, std::ofstream & fout, std::string temp_directory, std::size_t max_files, std::size_t max_elements) {
	Sorter<std::size_t, std::size_t, Reducer, RunType> sorter(max_files, temp_directory, max_elements);
	sorter.sort(fin, fout);
	return sorter.get_stats();
}

int main(int argc, char ** argv) {
	const std::string DEFAULT_INPUT_FILENAME = "data.txt";
	const std::string DEFAULT_OUTPUT_FILENAME = "data.out";
	const std::string DEFAULT_TEMPORARY_DIRECTORY = "/tmp";

	std::string in_filename = DEFAULT_INPUT_FILENAME;
	std::string out_filename = DEFAULT_OUTPUT_FILENAME;
	std::string temp_directory = DEFAULT_TEMPORARY_DIRECTORY;

	bool binary = true;
	bool inner_reductions = true;

	std::size_t max_files = 256;
	std::size_t max_elements = 1 << 28;

	for (int i = 1; i < argc; ++i) {
		if (std::string(argv[i]) == "-m") {
			max_elements = 1 << atoi(argv[i + 1]);
			++i;
		}

		if (std::string(argv[i]) == "-f") {
			max_files = atoi(argv[i + 1]);
			++i;
		}

		if (std::string(argv[i]) == "-i") {
			in_filename = std::string(argv[i + 1]);
			++i;
		}

		if (std::string(argv[i]) == "-o") {
			out_filename = std::string(argv[i + 1]);
			++i;
		}

		if (std::string(argv[i]) == "--tmp" || std::string(argv[i]) == "-t") {
			temp_directory = std::string(argv[i + 1]);
			++i;
		}

		if (std::string(argv[i]) == "--text") {
			binary = false;
		}

		if (std::string(argv[i]) == "-v") {
			logger = Log(true);
		}

		if (std::string(argv[i]) == "-vv") {
			logger = Log(true);
			logger_trace = Log(true);
		}

		if (std::string(argv[i]) == "--binary") {
			binary = true;
		}

		if (std::string(argv[i]) == "--no-reductions") {
			inner_reductions = false;
		}
	}


	std::ifstream fin(in_filename);
	std::ofstream fout(out_filename);

	std::cout << "Binary mode:    " << binary << "\n";
	std::cout << "Reduction mode: " << inner_reductions << "\n";

	Stats stats;
	if (binary) {
		if (inner_reductions) {
			stats = sort<MinReducer, BufferedBinaryRun>(fin, fout, temp_directory, max_files, max_elements);
		} else {
			stats = sort<NoReducer, BufferedBinaryRun>(fin, fout, temp_directory, max_files, max_elements);
		}
	} else {
		if (inner_reductions) {
			stats = sort<MinReducer, TextRun>(fin, fout, temp_directory, max_files, max_elements);
		} else {
			stats = sort<NoReducer, TextRun>(fin, fout, temp_directory, max_files, max_elements);
		}
	}

	std::cout << "Input size:       " << stats.input_size << "\n";
	std::cout << "Output size:      " << stats.output_size << "\n";
	std::cout << "Inner reductions: " << stats.inner_reductions << "\n";

	return 0;
}
