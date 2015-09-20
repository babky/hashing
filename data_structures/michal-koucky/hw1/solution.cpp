/*
 * solution.cpp
 *
 *  Created on: Sep 5, 2015
 *      Author: Martin Babka
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

// Perform the inner sort of large blocks.
// Perform the merge until the output is sorted.

template<typename Key, typename Value, class Push>
class MinReducer {
	bool first;
	Key prevKey;
	Value prevValue;
	Push push;

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
		} else if (prevValue > value) {
			// In the case of the same key, just remember the minimum.
			prevValue = value;
		}

		prevKey = key;
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
		std::cerr << "Writing length of the run " << size << ".\n";
	}

	void operator()(Key key, Value value) {
		++length;
		out.write((char *) &key, sizeof(key));
		out.write((char *) &value, sizeof(value));

		std::cerr << "Pushing " << key << " " << value << ", having length " << length << ".\n";
	}

	void finish_run() {
		mark_size(length);
	}

	void start_run() {
		block_size_pos = out.tellp();
		length = 0;
		out.write((char *) &length, sizeof(length));
		std::cerr << "Starting a new run.\n";
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

public:
	TextOutputPusher(std::ostream & out_stream):
		out(out_stream) {
	}

	void operator()(Key key, Value value) {
		std::cerr << "Outputting " << key << " " << value << ".\n";
		out << key << " " << value << "\n";
	}
};

template<typename Key, typename Value>
class Run {
	std::pair<Key, Value> current_pair;
	std::fstream * in;
	std::size_t length;

public:
	Run(std::fstream & input_file):
		in(&input_file) {
		in->read((char *) &length, sizeof(length));
		std::cerr << "Having run of length " << length << ".\n";
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

template<typename Key, typename Value, template<class, class, class> class Reducer, class Pusher>
class Merger {
private:
	std::vector<RunSource<Key, Value>> sources;
	Pusher push;

public:
	Merger(std::vector<RunSource<Key, Value>> run_sources, Pusher pusher):
		sources(run_sources),
		push(pusher) {
	}

	std::size_t merge() {
		std::size_t run = 0;
		std::vector<std::pair<Run<Key, Value>, bool>> runs;
		for (std::size_t i = 0; i < sources.size(); ++i) {
			runs.push_back(std::pair<Run<Key, Value>, bool>(sources[i].next(), true));
			runs[i].first.next();
		}

		std::cerr << "Runs created.\n";

		for (bool found_new_run = true; found_new_run;) {
			++run;
			create_new_run(runs);

			found_new_run = false;
			for (std::size_t i = 0; i < sources.size(); ++i) {
				if (sources[i].has_next()) {
					std::cerr << "Run in the file " << i << ".\n";
					runs[i] = std::pair<Run<Key, Value>, bool>(sources[i].next(), true);
					std::cerr << "Reading the first key-value-pair in the run of file " << i << ".\n";
					runs[i].first.next();
					found_new_run = true;
					std::cerr << "Found a new run in the input file " << i << ". We will start a next round.\n";
				}
			}

			std::cerr << "Finished creating a run.\n";
		}

		std::cerr << "Created " << run << " new runs.\n";

		return run;
	}

private:
	void create_new_run(std::vector<std::pair<Run<Key, Value>, bool>> & runs) {
		std::cerr << "Starting a new merge run.\n";
		Reducer<Key, Value, Pusher> reducer(push);
		reducer.start();
		reducer.get_pusher().start_run();

		for (;;) {
			std::pair<Key, Value> * minKey = 0;
			std::size_t minIndex = 0;

			for (std::size_t i = 0; i < runs.size(); ++i) {
				if (runs[i].second && (minKey == 0 || minKey->first > runs[i].first.current().first)) {
					minKey = &runs[i].first.current();
					minIndex = i;
				}
			}

			if (minKey == 0) {
				break;
			}

			std::cerr << "Adding key " << minKey->first << " " << minKey->second << ".\n";
			reducer.add(minKey->first, minKey->second);

			if (runs[minIndex].first.has_next()) {
				runs[minIndex].first.next();
				std::cerr << "Advancing in " << minIndex << ".\n";
			} else {
				std::cerr << "Disabling " << minIndex << ".\n";
				runs[minIndex].second = false;
			}
		}

		reducer.stop();
		reducer.get_pusher().finish_run();
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

template<typename Key, typename Value, template<class, class, class> class Reducer>
class Sorter {
	std::size_t max_elements_in_memory;
	std::size_t max_files;
	std::string temp_directory;
	std::vector<std::fstream *> streams;

public:
	Sorter(std::size_t temp_file_limit, std::string temporary_directory,
			std::size_t max_elements):
		max_elements_in_memory(max_elements),
		max_files(temp_file_limit),
		temp_directory(temporary_directory),
		streams(max_files) {
	}

	void sort(std::ifstream & in, std::ostream & out) {
		clear_temp_files();

		// parse the data and sort them into the files as runs
		std::size_t runs = parse(in);
		if (runs == 1) {
			// write the output
			write_to_out(0, out);
		} else {
			// merge the runs
			std::cerr << "Merging " << runs << " runs.\n";
			for (; runs != 1;) {
				runs = merge_runs();
				if (runs > 1) {
					distribute_runs(runs);
				}
			}

			// write the output
			write_to_out(max_files - 1, out);
		}

		clear_temp_files();
	}

private:
	void distribute_runs(std::size_t runs) {
		RunSource<Key, Value> run_sources(get_temp_file(max_files - 1, true, std::ios::binary | std::ios::in));

		std::size_t i = 0;
		std::size_t runs_per_file = runs / (max_files - 1);
		std::size_t last_file = 0;
		std::size_t current_file = 0;
		BinaryOutputPusher<Key, Value> * pusher = new BinaryOutputPusher<Key, Value>(get_temp_file(current_file, true, std::ios::binary | std::ios::out | std::ios::trunc));

		while (run_sources.has_next()) {
			current_file = i / runs_per_file;
			if (last_file != current_file && current_file < max_files - 1) {
				delete pusher;
				pusher = new BinaryOutputPusher<Key, Value>(get_temp_file(current_file, true, std::ios::binary | std::ios::out | std::ios::trunc));
				last_file = current_file;
			}

			Run<Key, Value> run = run_sources.next();
			pusher->start_run();
			while (run.has_next()) {
				run.next();
				pusher->operator()(run.current().first, run.current().second);
			}
			pusher->finish_run();
			++i;
		}

		delete pusher;
	}

	std::size_t parse(std::ifstream & in) {
		Key key;
		Value value;

		std::vector<std::pair<Key, Value>> data;
		VectorPusher<Key, Value> pusher(data);
		Reducer<Key, Value, VectorPusher<Key, Value>> reducer(pusher);

		reducer.start();
		std::size_t run = 0;

		for (;;) {
			in >> key >> value;
			if (in.eof()) {
				break;
			}

			reducer.add(key, value);

			if (data.size() == max_elements_in_memory) {
				sort_and_output_vector(data, run % (max_files - 1));
				run += 1;
				data.clear();
			}
		}
		reducer.stop();
		sort_and_output_vector(data, run % (max_files - 1));
		run += 1;

		return run;
	}

	std::size_t merge_runs() {
		std::vector<RunSource<Key, Value>> run_sources;
		for (int i = 0; i < max_files - 1; ++i) {
			std::fstream & fs = get_temp_file(i, true, std::ios::binary | std::ios::in);
			run_sources.push_back(RunSource<Key, Value>(fs));
		}

		Merger<Key, Value, Reducer, BinaryOutputPusher<Key, Value>> merger(run_sources, BinaryOutputPusher<Key, Value>(get_temp_file(max_files - 1, true, std::ios::binary | std::ios::out | std::ios::trunc)));
		return merger.merge();
	}

	void write_to_out(std::size_t idx, std::ostream & out) {
		Key key;
		Value value;

		std::fstream & in = get_temp_file(idx, true, std::ios::binary | std::ios::in);
		in.seekg(std::ios_base::beg);
		TextOutputPusher<Key, Value> text_pusher(out);
		MinReducer<Key, Value, TextOutputPusher<Key, Value>> reducer(text_pusher);

		std::size_t size;
		in.read((char*) &size, sizeof(size));

		reducer.start();
		for (;;--size) {
			read_binary(in, key, value);
			reducer.add(key, value);
			if (in.eof()) {
				assert(size == 0);
				break;
			}
		}
		reducer.stop();
	}

	void sort_and_output_vector(std::vector<std::pair<Key, Value>> & data, std::size_t temp_file_idx) {
		// Sort internally.
		std::sort(data.begin(), data.end(), PairKeyComparer<Key, Value>());
		std::ostream & out = get_temp_file(temp_file_idx, false, std::ios::binary | std::ios::out | std::ios::trunc);

		// Write the block.
		BinaryOutputPusher<Key, Value> pusher(out);

		Reducer<Key, Value, BinaryOutputPusher<Key, Value>> reducer(pusher);
		reducer.get_pusher().start_run();
		reducer.start();
		for (int i = 0; i < data.size(); ++i) {
			reducer.add(data[i].first, data[i].second);
		}
		reducer.stop();
		reducer.get_pusher().finish_run();
		std::cerr << "Internally sorted into file " << temp_file_idx << ".\n";
	}


	std::string get_temp_filename(std::size_t i) {
		return temp_directory + "\\" + std::to_string(i) + ".tmp";
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
			std::cerr << "Opening " << i << "-th file " << filename << ".\n";
			streams[i] = new std::fstream(filename.c_str(), std::ios::binary | mode);
		}

		return *streams[i];
	}
};

int main(int argc, char ** argv) {
	std::ifstream fin("data.txt");
	std::ofstream fout("output.txt");

	Sorter<std::size_t, std::size_t, MinReducer> sorter(3, ".", 1 << 2);
	sorter.sort(fin, fout);
	return 0;
}
