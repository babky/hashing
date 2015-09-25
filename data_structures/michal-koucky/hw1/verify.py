if __name__ == "__main__":
	f = file('data.out', 'r')
	
	last_key = -1;
	while True:
		line = f.readline()
		if line == '':
			print 'OK'
			break
		key = int(line.split(' ')[0])
		
		if last_key >= key:
			print "FAIL";
			quit();
		else:
			last_key = key
	f.close()
