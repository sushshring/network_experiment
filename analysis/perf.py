#!/usr/bin/env python3
import subprocess
import sys
import time
import csv
import signal
from functools import reduce


def get_int_from_string(string: str):
    return int(''.join(c for c in string if c.isdigit()))


def parse_bytes(byte_string: str):
    if 'K' in byte_string:
        return get_int_from_string(byte_string) * (2**10)
    elif 'M' in byte_string:
        return get_int_from_string(byte_string) * (2**20)
    return int(byte_string)


class perf_details(object):
    def __init__(self):
        self._cpu_load = self._update_cpu_load()
        self._memory_usage = self._update_memory_usage()
        self._cpu_usage = self._update_cpu_usage()

    def _update_cpu_usage(self):
        with open('/proc/stat', 'r') as cpufile:
            stats = cpufile.readline()
            cpunums = [float(x) for x in stats.split() if x.isdigit()]
            usage = cpunums[0] + cpunums[1] + cpunums[2]
            total = sum(cpunums)
            return usage * 100.0 / total
        pass

    def _update_cpu_load(self):
        with open('/proc/loadavg', 'r') as loadfile:
            stats = loadfile.readline()
            return float(stats.split()[0])
        pass

    def _update_memory_usage(self):
        with open('/proc/meminfo', 'r') as memfile:
            memtotal = memfile.readline()
            memfree = memfile.readline()
            return int(memtotal.split()[1]) - int(memfree.split()[1])

    def __repr__(self):
        return "cpu load: %f, cpu usage: %f, memory usage %d" % (self._cpu_load, self._cpu_usage,self._memory_usage)

perfs = []


def Average(lst):
    if len(lst) == 0:
        return None
    return reduce(lambda a, b: a + b, lst) / len(lst)

virttopproc: subprocess.Popen = None
# iostats = open('io_output', 'w')
def _update_io_usage():
    io_cmd = "virt-top --script --block-in-bytes --no-csv-cpu --no-csv-mem --csv io_output"# | tail -1 | awk '{print $3\",\"$4\",\"$5\",\"$6}'"
    global virttopproc
    virttopproc = subprocess.Popen(io_cmd, shell=True)
        # return sum([parse_bytes(r) for r in response.split(',')])
    pass


def exit_handler(sig, frame):
    cpu_load_avg = perfs[-1]._cpu_load
    cpu_usage = Average(list(map(lambda x: x._cpu_usage, perfs)))
    mem_usage = Average(list(map(lambda x: x._memory_usage, perfs)))
    io_usage = 0
    virttopproc.kill()
    # iostats.flush()
    with open('io_output', 'r') as csvfile:
        reader = csv.DictReader(csvfile)
        relevant_rows = [row for row in reader if "target" in row["Domain name"]]
        print(relevant_rows)
        for row in relevant_rows:
            io_usage += sum([int(r) for r in [row["Block RDBY"], row["Block WRBY"], row["Net RXBY"], row["Net TXBY"]]])
        csvfile.truncate(0)

    # response = subprocess.check_output("cat io_output | awk '{print $3\",\"$4\",\"$5\",\"$6}'; rm io_output", shell=True).decode()
    # print(response)
    # for i in response.split('\n'):
        # print(i)
        # io_usage += sum([parse_bytes(r) for r in i.split(',')])
    print("cpu load: %f, cpu usage: %f, memory usage %d, io_bytes: %d" % (cpu_load_avg, cpu_usage, mem_usage, io_usage))
    sys.exit(0)


def get_perf_details():
    _update_io_usage()
    while True:
        # time.sleep(1)
        perfs.append(perf_details())

if __name__ == '__main__':
    signal.signal(signal.SIGINT, exit_handler)
    signal.signal(signal.SIGTERM, exit_handler)
    get_perf_details()
