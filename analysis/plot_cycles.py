#!/usr/bin/env python
from argparse import ArgumentParser
from glob import glob
from parsing.cycle_parser import CycleParser
from orchestrationplatform import is_non_zero_file
from matplotlib import pyplot as plt
from typing import List
from sys import maxsize
# from matplotlib.backends.backend_pdf import PdfPages
import numpy as np


def plotting(file_glob: str):
    for file in glob(file_glob):
        file_opened = open(file, 'r')
        c = CycleParser(file_opened)
        file_opened.close()
        for cycle in c.cycles:
            if cycle.score():
                cycle.plot()
    plt.show()
    # pdf = PdfPages('longplot.pdf')
    # pdf.savefig()
    # pdf.close()
    pass


def main():
    parser = ArgumentParser(
        description="This program plots the cycles for provided data file")
    # data directory
    parser.add_argument('file', metavar='file', type=str,
                        help='Path to data file')
    args = parser.parse_args()
    plotting(args.file)
    pass


if __name__ == "__main__":
    main()
