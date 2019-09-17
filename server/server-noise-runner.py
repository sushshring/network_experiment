#!/usr/bin/env python3.7
import subprocess
import threading


def run_benchmark():
    while True:
        cmd = ["/ycsb-0.15.0/bin/ycsb.sh", "load", "memcached",  "-s", "-P", "/network-exp/server/memcached-workload", "-p", "memcached.hosts=127.0.0.1"]
        subprocess.run(cmd, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
        cmd = ["/ycsb-0.15.0/bin/ycsb.sh", "run", "memcached",  "-s", "-P", "/network-exp/server/memcached-workload", "-p", "memcached.hosts=127.0.0.1"]
        subprocess.run(cmd, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)


def main():
    thread = threading.Thread(target=run_benchmark, args=())
    thread.daemon=True
    memcached_cmd = ["memcached"]
    subprocess.Popen(memcached_cmd)
    thread.start()
    cmd = ["./server", "-v", "6000"]
    subprocess.run(cmd)

if __name__ == "__main__":
    main()