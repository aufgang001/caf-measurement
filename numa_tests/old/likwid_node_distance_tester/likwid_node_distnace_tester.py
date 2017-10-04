#!/usr/bin/python3

import subprocess
import itertools


def execute_cmd(cmd):
    out = subprocess.getoutput(cmd)
    return [line for line in out.split(sep="\n")]

def load_dummy_topo_prints():
    f = open("test_topo.txt", "r")
    return [line.strip() for line in f]

def load_topo_prints():
    return execute_cmd("likwid-bench -p")

def load_dummy_copy_mem_prints(g_a, g_b):
    f = open("test_copy_mem.txt", "r")
    return [line.strip() for line in f]

def load_copy_mem_prints(g_a, g_b):
    # return execute_cmd("likwid-bench -t copy_mem -w " + g_a + ":1GB-0:" + g_b + ",1:" + g_b) #short
    return execute_cmd("likwid-bench -t copy_mem -i 1024 -w " + g_a + ":8GB-0:" + g_b + ",1:" + g_b) # long

def load_dummy_matrix():
    f = open("test_matrix.txt", "r")
    node_list = f.readline().strip().split()
    raw_data = []
    for line in f:
        raw_data.append(line.strip().split()[1:])
    data = dict()
    for zeile_idx, line in zip(range(len(node_list)), raw_data):
        for spalte_idx, dist in zip(range(len(node_list)), line):
            data[(node_list[zeile_idx], node_list[spalte_idx])] = int(dist)
    return (node_list,data)

def get_numa_domains_from_topo(topo_txt):
    result = []
    for line in topo_txt:
        signal_str = "Tag M"
        start = line.find(signal_str)
        end = line.find(":")
        if start != -1 and end != -1:
            result.append(line[start + len(signal_str)-1:end])
    return result

def get_throughput_from_copy_mem(copy_mem_txt):
    signal_str = "MByte/s:"
    for line in copy_mem_txt:
        if signal_str in line:
            return int(float(line[len(signal_str):].strip()) + 0.5)
    return 0

def plot_throughput_matrix(node_list, data):
    # header
    print("%4s" % (""), end="")
    for e in node_list:
        print("%8s" % (e), end="") 
    print()
    # content
    for zeile in node_list:
        print("%4s" % (zeile), end="")
        for spalte in node_list:
            print("%8s" % (data[(zeile, spalte)]), end="") 
        print()

def relativize_matrix(node_list, data_old):
    # relativized by line
    data = data_old.copy() 
    for zeile in node_list:
        base = float(data[zeile, zeile])
        data[zeile, zeile] = float(1.000)
        for spalte in node_list:
            if zeile != spalte:
                value = round(base / data[zeile, spalte], 1)
                data[zeile, spalte] = value
    return data

def run():
    data = dict() # key: (zeile, spalte), value: Mbyte/s
    node_list = get_numa_domains_from_topo(load_topo_prints())
    for a, b in itertools.product(node_list, node_list):
        if (a >= b):
            result = get_throughput_from_copy_mem(load_copy_mem_prints(a,b))
            data[(a, b)] = result
            data[(b, a)] = result
    plot_throughput_matrix(node_list, data)
    print()
    plot_throughput_matrix(node_list, relativize_matrix(node_list, data))

    # node_list, data = load_dummy_matrix()
    # plot_throughput_matrix(node_list, data)
    # print()
    # plot_throughput_matrix(node_list, relativize_matrix(node_list, data))

if __name__ == "__main__":
    run()
