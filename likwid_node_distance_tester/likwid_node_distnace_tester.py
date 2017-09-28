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
    return execute_cmd("likwid-bench -t copy_mem -w " + g_a + ":1GB-0:" + g_b + ",1:" + g_b)

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

def run():
    data = dict() # key: (a,b), value: Mbyte/s
    node_list = get_numa_domains_from_topo(load_topo_prints())
    for a, b in itertools.product(node_list, node_list):
        if (a >= b):
            result = get_throughput_from_copy_mem(load_copy_mem_prints(a,b))
            data[(a, b)] = result
            data[(b, a)] = result
    plot_throughput_matrix(node_list, data)

if __name__ == "__main__":
    run()
