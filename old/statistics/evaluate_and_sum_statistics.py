#!/usr/bin/python3

import sys
import pathlib
import os.path

def load_file(file):
    f = open(file, 'r') 
    raw_data = [line.strip().split(sep=",") for line in f] 
    data = []
    data.append(raw_data[0]) # add header
    for line in raw_data[1:]: # convert lines of strings to line of integers
        data.append([int(new_line) for new_line in line])
    return data
        
def get_column(data):
    for column_idx in range(0, len(data[0])):
        column = [row[column_idx] for row in data]    
        yield column

def beautify_file(file):
    return pathlib.PurePosixPath(file).name

def print_evaluation(db):
    for header, data_sets in db.items():
        print("%57s %s" %("", header))
        for data_set in data_sets:
            print("%55s:" % (data_set[0]), end='')
            data = data_set[1]
            for column in get_column(data):
                print("%10d " % (sum(column)/len(column)), end='')
            print("")

def run(files):
    db = dict() #key: header, value: list<file name, data>
    for file in files:
        data = load_file(file)
        header = ",".join(data[0])
        data = data[1:]
        if header in db:
            db[header].append((beautify_file(file), data))
        else:
            db[header] = [(beautify_file(file), data)]
    print_evaluation(db)

def print_help():
    print("evaluate_and_sum_statistics <files>")

if __name__ == '__main__':
    if len(sys.argv) < 2:
        print_help()
    else:
        run(sys.argv[1:])
