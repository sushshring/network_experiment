#!/usr/bin/env python
from argparse import ArgumentParser
from glob import glob
from parsing.cycle_parser import CycleParser
from orchestrationplatform import is_non_zero_file
from typing import List
from sys import maxsize
import numpy as np
times = ['2s', '3s', '4s', '5s', '6s', '7s', '8s', '9s', '10s']


def calculate_signals(directory: str):
    globs = [
        '{}/client_times_duration_sensitivity_new_blades_{}_*'.format(directory, x) for x in times]
    # print(globs)
    minnum = get_limiting_run_number(globs)
    for globstr in globs:
        cycles = get_cycles_for_data_files(globstr, minnum)
        successful_cycles = [x for x in cycles if x.score() > 0]
        print('For glob {}, there were {} cycles that had successful detection. The amplitude is {}. Total possible cycles were {}'.format(
            globstr, len(successful_cycles), np.abs(np.mean([x.score() for x in cycles])), len(cycles)))
    pass


def get_limiting_run_number(globs: List[str]):
    minnum = maxsize
    for globstr in globs:
        files = glob(globstr)
        # print(files)
        if minnum > len(files):
            minnum = len(files)
    return minnum


def get_cycles_for_data_files(globstr: str, minnum: int):
    cycles = []
    i = 0
    for file in get_files_for_glob(globstr):
        if i < minnum:
            cycles.extend(CycleParser(file).cycles)
        i += 1

    return cycles


def get_files_for_glob(globstr: str):
    files = glob(globstr)
    for file in files:
        if not is_non_zero_file(file):
            continue
        with open(file) as fh:
            yield fh
    pass


def main():
    parser = ArgumentParser(
        description="This program finds the signal for each flooding duration in the provided data directory")
    # data directory
    parser.add_argument('directory', metavar='dir', type=str,
                        help='Path to directory containing data files')
    args = parser.parse_args()
    calculate_signals(args.directory)
    pass


if __name__ == "__main__":
    main()
