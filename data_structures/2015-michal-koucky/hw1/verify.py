if __name__ == "__main__":
	f = file('/tmp/data.out', 'r')
	
	last_key = -1;
	verified = 0;
	while True:
		line = f.readline()
		if line == '':
			print verified
			print 'OK'
			break
		verified += 1
		key = int(line.split(' ')[0])
		
		if last_key >= key:
			print "FAIL";
			quit();
		else:
			last_key = key
	f.close()
