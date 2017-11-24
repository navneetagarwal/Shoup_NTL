import csv
import sys
for i in range(10):
	filename = "file_" + str(i+1) + ".txt"
	filename = open(filename, 'w')
	filename.write(str(i+1))
	filename.write("\n")
	filename.write(str(1))
	filename.write("\n")
	filename.write(str(i))
	filename.write("\n")
	for j in range(i):
		filename.write("2 OR " + str(j) + " " + str(j))
		filename.write("\n")

	filename.write("1")
	filename.write("\n")
	filename.write("2 OR "+ str(i) + " " + str(i))
	filename.write("\n")
	filename.close()