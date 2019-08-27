#!/usr/bin/env python3
import subprocess
import sys
import time
import signal
from functools import reduce


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

    def __str__(self):
        return "cpu load: %f, cpu usage: %f, memory usage %d" % (self._cpu_load, self._cpu_usage, self._memory_usage)

perfs = []


def Average(lst):
    if len(lst) == 0:
        return None
    return reduce(lambda a, b: a + b, lst) / len(lst)


def exit_handler(sig, frame):
    cpu_load_avg = Average(list(map(lambda x: x._cpu_load, perfs)))
    cpu_usage = Average(list(map(lambda x: x._cpu_usage, perfs)))
    mem_usage = Average(list(map(lambda x: x._memory_usage, perfs)))
    print("cpu load: %f, cpu usage: %f, memory usage %d" % (cpu_load_avg, cpu_usage, mem_usage))
    sys.exit(0)


def get_perf_details():
    while True:
        time.sleep(1)
        perfs.append(perf_details())

if __name__ == '__main__':
    signal.signal(signal.SIGINT, exit_handler)
    get_perf_details()
