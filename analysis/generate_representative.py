#!/usr/bin/env python3.7
from plotter import plotter
from matplotlib.axes import Axes
from matplotlib import pyplot as plt
import numpy as np
import sys

low = 4.0
high = 4.5


def main():
    if (len(sys.argv) > 1):
        plot_representative_square_wave()
        plot_representative_flat_wave()
    else:
        plot_flooding_histogram()
        plot_flat_histogram()
    plt.show()
    pass


@plotter('Square Wave')
def plot_representative_square_wave(ax: Axes):
    lows = np.random.normal(loc=low, scale=0.5, size=(6000,))
    highs = np.random.normal(loc=low+4, scale=0.5, size=(6000,))
    i = 0
    combines = []
    while i < 6000:
        combines.extend(lows[i:i+1000])
        combines.extend(highs[i:i+1000])
        i += 1000
    ax.set_ylabel('Round-trip Time (ms)', fontsize=18)
    ax.set_xlabel('Request number', fontsize=18)
    ax.plot(combines, color='black')


@plotter('Non-flooding repr')
def plot_representative_flat_wave(ax: Axes):
    combines = np.random.normal(loc=low, scale=0.5, size=(12000,))
    ax.set_ylabel('Round-trip Time (ms)', fontsize=18)
    ax.set_xlabel('Request number', fontsize=18)
    ax.plot(combines, color='black')


@plotter('Flooder hist')
def plot_flooding_histogram(ax: Axes):
    lows = np.random.normal(loc=low, scale=0.5, size=(6000,))
    highs = np.random.normal(loc=low+4, scale=0.5, size=(6000,))
    lows = np.subtract(lows, low+2)
    highs = np.subtract(highs, low+2)
    ax.hist(lows, color='Orange', bins=1000, alpha=0.5,
            label='Round trip time with flooder')
    ax.hist(highs, color='Blue', bins=1000, alpha=0.5,
            label='Round trip time with flooder')
    ax.set_ylabel('Frequency')
    ax.set_xlabel('Normalized Request RTT')
    ax.legend()


@plotter('Flooder hist')
def plot_flat_histogram(ax: Axes):
    lows = np.random.normal(loc=low, scale=0.5, size=(6000,))
    highs = np.random.normal(loc=low, scale=0.5, size=(6000,))
    lows = np.subtract(lows, low)
    highs = np.subtract(highs, low)
    ax.hist(lows, color='Orange', bins=1000, alpha=0.5,
            label='Round trip time with flooder')
    ax.hist(highs, color='Blue', bins=1000, alpha=0.5,
            label='Round trip time with flooder')
    ax.set_ylabel('Frequency')
    ax.set_xlabel('Normalized Request RTT')
    ax.legend()


if __name__ == '__main__':
    main()
