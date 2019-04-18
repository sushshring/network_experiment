import math
from typing import List

import matplotlib
import numpy as np
from matplotlib.axes import Axes
from scipy import stats, signal
from sklearn.metrics import mean_squared_error

from outliers_filter_mode import OutliersFilterMode
from parser.arg_parser import ArgParser
from parser.rtt import Rtt

matplotlib.use('Qt5Agg')
import matplotlib.pyplot as plt


class plotter:
    def __init__(self, label: str):
        self.label = label

    def __call__(self, func):
        def plot(*args, **kwargs):
            ax = plotter.get_new_subplot(self.label)
            return func(*args, **kwargs, ax=ax)
        return plot

    @staticmethod
    def get_new_subplot(label: str) -> Axes:
        n = len(plt.gcf().axes)
        for i in range(n):
            plt.gcf().axes[i].change_geometry(max(math.ceil(math.sqrt(n + 1)), 1),
                                              max(round(math.sqrt(n + 1)), 1),
                                              i + 1)
        ax = plt.subplot(max(math.ceil(math.sqrt(n + 1)), 1),
                         max(round(math.sqrt(n + 1)), 1),
                         n + 1,
                         label=label)
        return ax



class Analyzer:
    def __init__(self, data: Rtt, control: Rtt):
        self.data = data
        self.control = control

    @staticmethod
    def _remove_outliers_mode(elements):
        mode = stats.mode(elements, axis=None)[0]
        stdev = np.std(elements)
        ht, lt = mode + 2 * stdev, mode - 2 * stdev
        return list(filter(lambda x: ht >= x >= lt, elements))

    @staticmethod
    def _remove_outliers_pct(elements, lower=5, upper=95):
        lt, ht = np.percentile(elements, lower), np.percentile(elements, upper)
        return list(filter(lambda x: ht >= x >= lt, elements))

    @staticmethod
    def _filter_data(data: List[float]) -> List[float]:
        b, a = signal.butter(3, 0.05)
        return signal.lfilter(b, a, data)

    def analyze(self, arg_parser: ArgParser):
        fig = plt.figure()
        fig.suptitle(arg_parser.title)
        if arg_parser.show_rtts:
            self.plot_rtts(arg_parser.remove_outliers, arg_parser.outlier_removal_type)
        if arg_parser.show_histogram:
            self.plot_histogram(full_histogram=arg_parser.full_histogram, remove_outliers=arg_parser.remove_outliers,
                                outliers_mode=arg_parser.outlier_removal_type)
        if arg_parser.show_control_histogram:
            self.plot_histogram_control(full_histogram=arg_parser.full_histogram,
                                        remove_outliers=arg_parser.remove_outliers,
                                        outliers_mode=arg_parser.outlier_removal_type)
            self.get_cr_detection_score()
        plt.show()
        pass

    # Plotting methods
    @plotter('RTT histogram')
    def plot_histogram(self, full_histogram: bool, remove_outliers: bool, outliers_mode: OutliersFilterMode, ax: Axes):
        """

        :param ax: Axes object to plot on
        :type full_histogram: bool Print the full histogram instead of two separate histograms based on whether the
        flooder was on or off
        """
        rtts, rtts_w_flooder, rtts_wo_flooder = self.data.rtts()
        if remove_outliers:
            if outliers_mode == OutliersFilterMode.PERCENTAGE:
                rtts = Analyzer._remove_outliers_pct(rtts)
                rtts_w_flooder = Analyzer._remove_outliers_pct(rtts_w_flooder)
                rtts_wo_flooder = Analyzer._remove_outliers_pct(rtts_wo_flooder)
            elif outliers_mode == OutliersFilterMode.FILTER:
                rtts = Analyzer._filter_data(rtts)
                rtts_w_flooder = Analyzer._filter_data(rtts_w_flooder)
                rtts_wo_flooder = Analyzer._filter_data(rtts_wo_flooder)
            else:
                rtts = Analyzer._remove_outliers_mode(rtts)
                rtts_wo_flooder = Analyzer._remove_outliers_mode(rtts_w_flooder)
                rtts_wo_flooder = Analyzer._remove_outliers_mode(rtts_wo_flooder)
        if full_histogram:
            ax.hist(rtts, bins=1000, alpha=0.5, label='Round trip time frequency')
        else:
            ax.hist(rtts_w_flooder, color='Orange', bins=1000, alpha=0.5, label='Round trip time with flooder')
            ax.hist(rtts_wo_flooder, color='Blue', bins=1000, alpha=0.5, label='Round trip time without flooder')
        ax.legend()
        ax.set_title('RTT histogram')

    @plotter('RTT plot')
    def plot_rtts(self, remove_outliers: bool, outliers_mode: OutliersFilterMode, ax: Axes):
        if remove_outliers:
            data, _, _ = self.data.rtts()
            if outliers_mode == OutliersFilterMode.PERCENTAGE:
                rtts = Analyzer._remove_outliers_pct(data)
            elif outliers_mode == OutliersFilterMode.FILTER:
                rtts = Analyzer._filter_data(data)
            else:
                rtts = Analyzer._remove_outliers_mode(data)
            ax.plot(rtts, color='r', label=str.format('Rtts with outliers removed by {}', outliers_mode.name))
        else:
            rtts, rtts_w_flooder, rtts_wo_flooder = self.data.rtts_with_start_times()
            ax.plot(*zip(*rtts), color='r', label='Rtts', alpha=0.5)
            ax.plot(*zip(*rtts_w_flooder), color='b', label='Rtts with flooder', alpha=0.5)
            ax.plot(*zip(*rtts_wo_flooder), color='g', label='Rtts without flooder', alpha=0.5)
        ax.legend()
        ax.set_title('RTT plot')

    @plotter('Histogram with control')
    def plot_histogram_control(self, full_histogram: bool, remove_outliers: bool, outliers_mode: OutliersFilterMode,
                               ax: Axes):
        """

               :param ax: Axes object to plot on
               :type full_histogram: bool Print the full histogram instead of two separate histograms based on whether the
               flooder was on or off
               """
        rtts, rtts_w_flooder, rtts_wo_flooder = self.control.rtts()
        if remove_outliers:
            if outliers_mode == OutliersFilterMode.PERCENTAGE:
                rtts = Analyzer._remove_outliers_pct(rtts)
                rtts_w_flooder = Analyzer._remove_outliers_pct(rtts_w_flooder)
                rtts_wo_flooder = Analyzer._remove_outliers_pct(rtts_wo_flooder)
            elif outliers_mode == OutliersFilterMode.FILTER:
                rtts = Analyzer._filter_data(rtts)
                rtts_w_flooder = Analyzer._filter_data(rtts_w_flooder)
                rtts_wo_flooder = Analyzer._filter_data(rtts_wo_flooder)
            else:
                rtts = Analyzer._remove_outliers_mode(rtts)
                rtts_wo_flooder = Analyzer._remove_outliers_mode(rtts_w_flooder)
                rtts_wo_flooder = Analyzer._remove_outliers_mode(rtts_wo_flooder)
        if full_histogram:
            ax.hist(rtts, bins=1000, alpha=0.5, label='Round trip time frequency')
        else:
            ax.hist(rtts_w_flooder, color='Orange', bins=1000, alpha=0.5, label='Round trip time with flooder')
            ax.hist(rtts_wo_flooder, color='Blue', bins=1000, alpha=0.5, label='Round trip time without flooder')
        ax.legend()
        ax.set_title('RTT histogram with control')

    def get_cr_detection_score(self):
        print(self.mann_whitney_test())
        print(self.ks_test())
        rtts, rtts_control = self.get_comparison_rtts()
        print('MSE score: %f' % mean_squared_error())
        pass

    def mann_whitney_test(self):
        rtts = Analyzer._filter_data(self.data.normalized())
        rtts_control = Analyzer._filter_data(self.control.normalized())
        stat, pval = stats.mannwhitneyu(rtts, rtts_control, alternative='greater')
        return "Mann Whitney Test: statistic value: %0.2f, pvalue: %0.2f" % (stat, pval)

    @plotter('Kolmogorov Smirnov Test')
    def ks_test(self, ax: Axes):
        rtts, rtts_control = self.get_comparison_rtts()
        stat, pval = stats.ks_2samp(rtts, rtts_control)
        ax.hist(rtts, color='Orange', bins=1000, alpha=0.5, label='Normalized RTTs with flooder')
        ax.hist(rtts_control, color='Blue', bins=1000, alpha=0.5, label='Normalized RTTs under control')
        ax.legend()
        ax.set_title('RTT histogram normalized by mean and stdev')

        hist = stats.rv_histogram(np.histogram(rtts_control, bins=1000))
        ax1 = plotter.get_new_subplot('QQ Plot for RTTs with flooder against control')
        stats.probplot(rtts, plot=ax1, fit=True, dist=hist)
        ax1.set_title('RTTs QQ Plot')
        return "Kolmogorov Smirnov Two Sample Test: statistic value: %0.2f, pvalue: %0.2f" % (stat, pval)

    def get_comparison_rtts(self):
        rtts = Analyzer._remove_outliers_pct(Analyzer._filter_data(self.data.normalized()), lower=1, upper=99)
        rtts_control = Analyzer._remove_outliers_pct(Analyzer._filter_data(self.control.normalized()), lower=1,
                                                     upper=99)
        return rtts, rtts_control
