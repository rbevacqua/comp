import StringIO
import os
import sys

class PERIOD():

	def __init__(self, start, end, color):

		self.start = start
		self.end = end
		self.color = color

class GRAPH():

	def __init__(self):

		self.end = 0
		self.intervals = []

	def addInterval(self, start, end, color):

		self.intervals.append(PERIOD(start, end, color))

		self.end = max(end, self.end)

	def scriptGNUPlot(self, title, filename, buff, size_x, size_y):
		buff.write('''
			set title "%s"
			set xlabel "Time (ms)"
			set nokey
			set noytics
			set term postscript eps 8
			set size %s, %s
			set output "%s"
			''' % (title, size_x, size_y, filename))

		num = 1

		for i in self.intervals:
			buff.write('set object %d rect from %f, %f to %f, %f, 2 fc rgb "%s" fs solid\n' % (num, i.start, 0.8, i.end, 3.8, i.color))

			num += 1

		buff.write('plot [0:%d] [0:%d] 0\n' % (self.end, 3.8))

	def makeGraph(self, title, filename):
		buff = StringIO.StringIO()

		buff.write('gnuplot << ---EOF---\n')

                self.scriptGNUPlot(title, filename, buff, '0.85', '0.35')

		buff.write('\n')

		if os.system(buff.getvalue()) != 0:
			print("an error ocurred running the script")
