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

class perfctr_request(object):
    pass

def likwid_perfctr_cmd(perf_req):
    def hardware_counter_to_string():
        tmp_str = ""
        for counter_id, what_to_count in zip(range(0, len(perf_req.what_to_counts)),perf_req.what_to_counts):
            tmp_str += what_to_count[0] + ":" + what_to_count[1] + str(counter_id) + ","
        tmp_str[-1] = " "
        return tmp_str
    likwid_program = "likwid-perfctr -f -O " 
    perf_req.cmd = []
    for where_to_count in where_to_counts:
        perf_req.cmd.append(likwid_prgram + "-c" + where_to_count + " -g " + hardware_counter_to_string() + " ")
    return perf_req

def load_raw_data(cmd, perf_req):
    perf_req = likwperf_ctr_cmd(perf_req)
    raw_data = []
    for perf_cmd in perf_req.cmd:
        print("cmd: " + perf_cmd + cmd)
        raw_data.extend(execute_cmd(perf_cmd + cmd)) 
    return raw_data

# parse the raw data
def get_accesses_per_core(raw_data, perf_req):
    def get_search_lines():
        result = dict()  #{LOCAL ACCESSES: UNC_CPU_REQUEST_TO_MEMORY_LOCAL_LOCAL_CPU_MEM,UPMC0, ..}
        for i in range(0, len(perf_req.names_of_what_to_counts)):
            result[perf_req.names_of_what_to_counts[i]] = perf_req.what_to_counts[i][0] + "," + perf_req.what_to_counts[i][1] + str(i)
        return result
    def fill_counter(data, counters):
        if len(data) <= 2:
            return
        for line_idx in range(0, len(data), 2):
            for column_idx in range(2, len(data[line_idx])):
                header_line = data[line_idx]
                value_line = data[line_idx+1]
                dict_value = int(header_line[column_idx][5:])
                counters[dict_value] = int(value_line[column_idx])
    counters = dict()
    data = dict()
    for name in perf_req.names_of_what_to_counts:
        counters[name] = dict()
        data[name] = []
    # local_counters = dict()
    # remote_counters = dict()
    # suc_cache_counters = dict()
    # mis_cache_counters = dict()
    # local_data = []
    # remote_data = []
    # suc_cache_data = []
    # mis_cache_data = []
    for line in raw_data:
        if "Event,Counter,Core" in line:
            for vals in data.values():
                vals.append(line.split(","))
            # local_data.appendline.split(",")()
            # remote_data.append(line.split(","))
            # suc_cache_data.append(line.split(","))
            # mis_cache_data.append(line.split(","))
        for name_of_what_to_count, search_for in get_search_lines().items():
            if search_for in line:
                data[name_of_what_to_count].append(line.split(","))
        # if "UNC_CPU_REQUEST_TO_MEMORY_LOCAL_LOCAL_CPU_MEM,UPMC0" in line:
            # local_data.append(line.split(","))
        # if "UNC_CPU_REQUEST_TO_MEMORY_LOCAL_REMOTE_CPU_MEM,UPMC1" in line:
            # remote_data.append(line.split(","))
        # if "DATA_CACHE_ACCESSES,PMC0" in line:
            # suc_cache_data.append(line.split(","))
        # if "DATA_CACHE_MISSES_ALL,PMC1" in line:
            # mis_cache_data.append(line.split(","))
    for name in perf_req.names_of_what_to_counts:
        fill_counter(data[name], counters[name])
    # fill_counters(local_data, local_counters)
    # fill_counters(remote_data, remote_counters)
    # fill_counters(suc_cache_data, suc_cache_counters)
    # fill_counters(mis_cache_data, mis_cache_counters)
    # return {"local":local_counters, "remote":remote_counters, "suc_cache":suc_cache_counters, "mis_cache":mis_cache_counters}
    return counters

def plot_data(data, perf_req):
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
    for name_of_what_to_count in data.keys():
        print(name_of_what_to_count + "\n" + data_to_string(data[name_of_what_to_count]))
        print("")
    # print("LOCAL ACCESSES\n" +data_to_string(data["local"]))
    # print("")
    # print("REMOTE ACCESSES\n" +data_to_string(data["remote"]))
    # print("")
    # print("SUCCESS CACHE A.\n" +data_to_string(data["suc_cache"]))
    # print("")
    # print("MISS CACHE A.\n" +data_to_string(data["mis_cache"]))

def load_dummy_raw_data():
    f = open("test_data/plot_a.txt", "r")
    result = [line.strip() for line in f]
    f = open("test_data/plot_b.txt", "r")
    result.extend([line.strip() for line in f])
    return result 

def run(args):
    cmd = " ".join(args)
    # raw_data = load_dummy_raw_data()
    # a = "likwid-perfctr -f -O -c 0,16,32,48 -g UNC_CPU_REQUEST_TO_MEMORY_LOCAL_LOCAL_CPU_MEM:UPMC0,UNC_CPU_REQUEST_TO_MEMORY_LOCAL_REMOTE_CPU_MEM:UPMC1,DATA_CACHE_ACCESSES:PMC0,DATA_CACHE_MISSES_ALL:PMC1 "
    # b = "likwid-perfctr -f -O -c 8,24,40,56 -g UNC_CPU_REQUEST_TO_MEMORY_LOCAL_LOCAL_CPU_MEM:UPMC0,UNC_CPU_REQUEST_TO_MEMORY_LOCAL_REMOTE_CPU_MEM:UPMC1,DATA_CACHE_ACCESSES:PMC0,DATA_CACHE_MISSES_ALL:PMC1 "

    ## count local and remote mem accesses
    # "likwid-perfctr -f -O -c 8,24,40,56 -g UNC_CPU_REQUEST_TO_MEMORY_LOCAL_LOCAL_CPU_MEM:UPMC0,UNC_CPU_REQUEST_TO_MEMORY_LOCAL_REMOTE_CPU_MEM:UPMC1 "
    perf_req = perfctr_request()
    perf_req.where_to_counts = ["0,16,32,48","8,24,40,56"]
    perf_req.names_of_what_to_counts = ["LOCAL ACCESSES", "REMOTE ACCESSES"]
    perf_req.what_to_counts = [("UNC_CPU_REQUEST_TO_MEMORY_LOCAL_LOCAL_CPU_MEM","UPMC"), ("UNC_CPU_REQUEST_TO_MEMORY_LOCAL_REMOTE_CPU_MEM", "UPMC")]

    # raw_data = load_dummy_raw_data()
    raw_data = load_raw_data(cmd, perf_req)
    for line in raw_data:
        print(line)
    data = get_accesses_per_core(raw_data, perf_req)
    print(data)
    plot_data(data, perf_req)

if __name__ == "__main__":
    run(sys.argv[1:])

