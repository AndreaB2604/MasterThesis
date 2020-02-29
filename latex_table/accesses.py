import sys
import csv


def printLatexTableCSV(the_file):
	with open(the_file, mode='r') as csv_file:
		csv_reader = csv.reader(csv_file)
		csv_list = list(csv_reader)

	rng = 14
	for line in csv_list:
		if(len(line[0].strip()) != 0):
			chunks = line[0].split(" ")
			for j in range(len(chunks)):
				chunks[j] = chunks[j].capitalize()
			line[0] = " ".join(chunks)
		
		for i in range(len(line)):
			if(len(line[i]) == 0):
				line[i] = "-"

		string = ""
		#print data for the months
		for i in range(rng):
			if(i < rng-1):
				string += line[i] + " & "
			else:
				string += line[i] + " \\\\"
		print(string)

	print("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%")
	print("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%")
	print("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%")

	for line in csv_list:
		#print(line)
		# print the other data
		string = line[0] + " & "
		for i in range(rng, len(line)):
			if(i < len(line)-1):
				string += line[i] + " & "
			else:
				string += line[i] + " \\\\"

		print(string)
		



if __name__ == '__main__':
	if len(sys.argv) != 2:
		print("Only the csv file in input accepted!")
		sys.exit(1)
	printLatexTableCSV(sys.argv[1])
