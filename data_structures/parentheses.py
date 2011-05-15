#!/bin/env python
import sys

class Processor:
	def __init__(self, lines):
		self.lines = lines
		self.state = {}
		self.replacements = {
			'(': ('\\left(', 1), 
			')': ('\\right)', -1), 
			'[': ('\\left[', 1),
			']': ('\\right]', -1),
			'\\{': ('\\left\\{', 1),
			'\\}': ('\\right\\}', -1),
			'\\lceil': ('\\left\\lceil', 1),
			'\\rceil': ('\\right\\rceil', -1)
		}
		self.forbidden = ['\\vbox', '\\vtop']

	def log(self, message):
		return
		print self.state['line_no'], message

	def findNearestChange(self, line, pos):
		oldPos = pos
	
		if self.state['forbidden_mode'] != False:
			self.log('Forbidden mode')
			pos = line.find(self.state['forbidden_mode'])
			if pos == -1:
				return (-1, False, line)
			else:
				self.state['forbidden_mode'] = False
				pos += 2

		assert(self.state['forbidden_mode'] == False)

		while True:
			pos = line.find('$', pos)
			if pos == -1:
				return (-1, False, line[oldPos:len(line)])

			if line[pos - 1] == '\\':
				pos += 1
				continue

			if line[pos + 1] == '$':
				pos += 2
				mode = '$$'
			else:
				mode = '$'
				pos += 1

			flagForbidden = False
			for f in self.forbidden:
				if line[pos:pos + len(f)] == f:
					pos = line.find(mode, pos)
					if pos == -1:
						self.state['forbidden_mode'] = mode
						return (-1, False, line[oldPos:len(line)])
					else:
						pos += len(mode) 

					flagForbidden = True

			if flagForbidden:
				continue

			return (pos, mode, line[oldPos:pos])

	def findPar(self, line, pos, par, stop):
		oldPos = pos
		pos = line.find(par, pos, stop)
		if pos == -1:
			return (False, -1, line[oldPos:len(line)])
		else:
			return (True, pos + 1, line[oldPos:pos])

	def processLine(self, line):
		self.log('Entering line in math mode {0}.'.format(self.state['math_mode']))

		# Hladame math mode
		pos = 0
		resLine = []
		atDollar = False
		for r in self.replacements:
			line = line.replace('\\big' + r, r)
		line = line.replace('\\big\n', '\n')
		stop = len(line)

		while pos < stop:
			if atDollar or self.state['math_mode'] == False:
				self.log('Finding change')
				# Najblizsia zmena
				(pos, mode, part) = self.findNearestChange(line, pos)
	
				# Skopiruj, co sa da
				resLine.append(part)
				if pos == -1:
					break
	
				# print line
				if self.state['math_mode'] == False:
					# print 'new'
					self.state['math_mode'] = [mode]
					self.state['opened'] = 0
				elif self.state['math_mode'][-1] != mode:
					# print 'push'
					self.state['math_mode'].append(mode)
				else:
					# print 'pull'
					self.state['math_mode'] = self.state['math_mode'][:-1]
					if len(self.state['math_mode']) == 0:
						self.state['math_mode'] = False
						continue

			# Hladame zatvorky a opravujeme
			atDollar = False
			opened = []
			while pos < stop:
				foundReplacement = False
				for r in self.replacements:
					if line[pos:pos + len(r)] == r:
						foundReplacement = r
						break

				if foundReplacement != False:
					(repl, d) = self.replacements[foundReplacement]
					if d == -1:
						if self.state['opened'] == 0:
							resLine.append(foundReplacement)
						else:
							opened = opened[:-1]
							resLine.append(repl)
							self.state['opened'] += d
					else:
						self.state['opened'] += d
						opened.append((len(resLine), foundReplacement))
						resLine.append(repl)

					pos += len(foundReplacement)
					continue

				if line[pos] == '$':
					atDollar = True
					break
				elif line[pos] == '&':
					self.state['opened'] -= len(opened)
					for o in opened:
						resLine[o[0]] = o[1]

					if self.state['opened'] > 0:
						print 'Alignment char inside left-right opened.', self.state['line_no']
						exit()

					resLine.append(line[pos])
				else:
					resLine.append(line[pos])
				pos += 1

		return "".join(resLine)
				

	def processLines(self):
		self.state['math_mode'] = False
		self.state['forbidden_mode'] = False
		self.state['line_no'] = 0
		resLines = []

		for line in self.lines:
			self.state['line_no'] += 1
			resLine = self.processLine(line)
			resLines.append(resLine)

		return resLines

if __name__ == "__main__":
	f = open(sys.argv[1], 'r')
	lines = f.readlines()

	p = Processor(lines)
	resLines = p.processLines()

	f.close()

	f = open(sys.argv[2], 'w')
	f.write("".join(resLines))
	f.close()

