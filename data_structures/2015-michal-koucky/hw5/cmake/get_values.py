import sys

if __name__ == "__main__":
	f = open(sys.argv[1])
	
	last_data_size = 0
	ins_cnt = 0
	find_cnt = 0
	ins_ops = 0
	find_ops = 0
	time = 0
	cnt = 0
	
	for line in f.readlines():
		t = line.split(',')
		t = map(int, t)
		(size, data_size, inserts, finds, insert_swaps, insert_find_operations, rehashes, find_operations, running_time) = t
		if last_data_size == data_size:
			if finds == 0:
				continue

			cnt += 1
			ins_cnt += inserts
			find_cnt += finds
			ins_ops += insert_swaps + insert_find_operations
			find_ops += find_operations
			time += running_time
		else:
			if last_data_size > 0:
				if ins_cnt + find_cnt == 0:
					ins_cnt = 1
				print ",".join(map(str, (float(last_data_size) / size, float(ins_ops + find_ops) / (ins_cnt + find_cnt), float(1000 * time) / (ins_cnt + find_cnt))))
			
			last_data_size = data_size
			
			if finds == 0:
				ins_cnt = 0
				find_cnt = 0
				ins_ops = 0
				find_ops = 0
				time = 0
				cnt = 0
				continue

			cnt = 1
			ins_cnt = inserts
			find_cnt = finds
			ins_ops = insert_swaps + insert_find_operations
			find_ops = find_operations
			time = running_time
			last_data_size = data_size

	if ins_cnt + find_cnt == 0:
		ins_cnt = 1
	print float(last_data_size) / size, float(ins_ops + find_ops) / (ins_cnt + find_cnt), float(1000 * time) / (ins_cnt + find_cnt)
	f.close()
