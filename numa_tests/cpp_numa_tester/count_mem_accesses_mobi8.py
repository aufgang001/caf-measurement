#!/usr//bin/python3

import io
import sys
import subprocess

def print_to_string(*args, **kwargs):
    output = io.StringIO()
    print(*args, file=output, **kwargs)
    contents = output.getvalue()
    output.close()
    return contents

def execute_cmd(cmd):
    out = subprocess.getoutput(cmd)
    return [line for line in out.split(sep="\n")]

def load_raw_data(cmd):
    a = "likwid-perfctr -O -c 0,16,32,48 -g UNC_CPU_REQUEST_TO_MEMORY_LOCAL_LOCAL_CPU_MEM:UPMC0,UNC_CPU_REQUEST_TO_MEMORY_LOCAL_REMOTE_CPU_MEM:UPMC1 "
    b = "likwid-perfctr -O -c 8,24,40,56 -g UNC_CPU_REQUEST_TO_MEMORY_LOCAL_LOCAL_CPU_MEM:UPMC0,UNC_CPU_REQUEST_TO_MEMORY_LOCAL_REMOTE_CPU_MEM:UPMC1 "
    raw_data = execute_cmd(a + cmd)
    raw_data += execute_cmd(a + cmd)
    return raw_data

def get_accesses_per_core(raw_data):
    def fill_counters(data, counters):
        for line_idx in range(0, len(data), 2):
            for column_idx in range(2, len(local_data[line_idx])):
                header_line = local_data[line_idx]
                value_line = local_data[line_idx+1]
                dict_value = int(header_line[column_idx][5:])
                counters[dict_value] = int(value_line[column_idx])
    local_counters = dict()
    remote_counters = dict()
    local_data = []
    remote_data = []
    for line in raw_data:
        if "Event,Counter,Core" in line:
            local_data.append(line.split(","))
            remote_data.append(line.split(","))
        if "UNC_CPU_REQUEST_TO_MEMORY_LOCAL_LOCAL_CPU_MEM,UPMC0" in line:
            local_data.append(line.split(","))
        if "UNC_CPU_REQUEST_TO_MEMORY_LOCAL_REMOTE_CPU_MEM,UPMC1" in line:
            remote_data.append(line.split(","))
    fill_counters(local_data, local_counters)
    fill_counters(remote_data, remote_counters)
    return {"local":local_counters, "remote":remote_counters}

def plot_data(data):
    def to_string_in_million(v):
        return str(int((v/1e6)+0.5))
    def sum_events(d):
        return sum([v for v in d.values()])
    def data_to_string(d):
        key_line = print_to_string("%13s" %("Core: "), end='')
        value_line = print_to_string("%13s" %("Mega events: "), end='')
        key_line += print_to_string("%7s" %("SUM"), end='')
        value_line += print_to_string("%7s" %(to_string_in_million(sum_events(d))), end='')

        for k, v in sorted(d.items()):
            key_line += print_to_string("%6s" %(str(k)), end='')
            value_line += print_to_string("%6s" %(to_string_in_million(v)), end='')
        return key_line + "\n" + value_line
    print("LOCAL ACCESSES\n" +data_to_string(data["local"]))
    print("")
    print("REMOTE ACCESSES\n" +data_to_string(data["remote"]))

def load_dummy_raw_data():
    f = open("test_data/plot_a.txt", "r")
    result = [line.strip() for line in f]
    f = open("test_data/plot_b.txt", "r")
    result.extend([line.strip() for line in f])
    return result 

def run(args):
    cmd = " ".join(args)
    # raw_data = load_dummy_raw_data()
    raw_data = load_raw_data(cmd)
    data = get_accesses_per_core(raw_data)
    plot_data(data)


if __name__ == "__main__":
    run(sys.argv[1:])

