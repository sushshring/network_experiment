import matplotlib

from outliers_filter_mode import OutliersFilterMode
from parser.arg_parser import ArgParser
from parser.rtt import Rtt
from matplotlib.axes import Axes
import numpy as np
from scipy import stats

matplotlib.use('Qt5Agg')
import matplotlib.pyplot as plt


class plotter:
    def __init__(self, label: str):
        self.label = label

    def __call__(self, func):
        def plot(*args, **kwargs):
            n = len(plt.gcf().axes)
            for i in range(n):
                plt.gcf().axes[i].change_geometry(n + 1, 1, i + 1)
            ax = plt.subplot(n+1, 1, n+1, label=self.label)
            return func(*args, **kwargs, ax=ax)

        return plot


class Analyzer:
    def __init__(self, data: Rtt):
        self.data = data

    @staticmethod
    def _remove_outliers_mode(elements):
        mode = stats.mode(elements, axis=None)[0]
        stdev = np.std(elements)
        ht, lt = mode + 2 * stdev, mode - 2 * stdev
        return filter(lambda x: ht >= x >= lt, elements)

    @staticmethod
    def _remove_outliers_pct(elements):
        lt, ht = np.percentile(elements, 0.05), np.percentile(elements, 0.95)
        return filter(lambda x: ht >= x >= lt, elements)

    def analyze(self, arg_parser: ArgParser):
        plt.figure()
        if arg_parser.show_rtts:
            self.plot_rtts(arg_parser.remove_outliers, arg_parser.outlier_removal_type)
        if arg_parser.show_histogram:
            self.plot_histogram(full_histogram=arg_parser.full_histogram)
        plt.show()
        pass

    # Plotting methods
    @plotter('RTT histogram')
    def plot_histogram(self, full_histogram: bool, ax: Axes):
        """

        :param ax: Axes object to plot on
        :type full_histogram: bool Print the full histogram instead of two separate histograms based on whether the
        flooder was on or off
        """
        rtts, rtts_w_flooder, rtts_wo_flooder = self.data.rtts()
        if full_histogram:
            ax.hist(rtts, bins=1000, alpha=0.5, label='Round trip time frequency')
        else:
            ax.hist(rtts_w_flooder, color='Orange', bins=1000, alpha=0.5, label='Round trip time with flooder')
            ax.hist(rtts_w_flooder, color='Blue', bins=1000, alpha=0.5, label='Round trip time without flooder')
        ax.legend()

    @plotter('RTT plot')
    def plot_rtts(self, remove_outliers: bool, outliers_mode: OutliersFilterMode, ax: Axes):
        if remove_outliers:
            data = self.data.rtts()
            if outliers_mode == OutliersFilterMode.PERCENTAGE:
                rtts = Analyzer._remove_outliers_pct(data)
            else:
                rtts = Analyzer._remove_outliers_mode(data)
            ax.plot(rtts, color='r', label='Rtts with outliers removed by %s' % (
                'percentage' if outliers_mode == OutliersFilterMode.PERCENTAGE else 'mode'))
        else:
            rtts, rtts_w_flooder, rtts_wo_flooder = self.data.rtts_with_start_times()
            ax.plot(*zip(*rtts), color='r', label='Rtts')
            ax.plot(*zip(*rtts_w_flooder), color='b', label='Rtts with flooder')
            ax.plot(*zip(*rtts_wo_flooder), color='g', label='Rtts without flooder')
        ax.legend()
