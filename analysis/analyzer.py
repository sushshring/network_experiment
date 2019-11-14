import math
from typing import List

import matplotlib
import numpy as np
from matplotlib.axes import Axes
from scipy import stats, signal
from sklearn.metrics import mean_squared_error
from itertools import *
from outliers_filter_mode import OutliersFilterMode
from parser.arg_parser import ArgParser
from parser.rtt import Rtt, RTTTYPE

matplotlib.use('Qt5Agg')
matplotlib.rc('xtick', labelsize=16)
matplotlib.rc('ytick', labelsize=16)
matplotlib.rcParams.update({'font.size': 20})
# matplotlib.use('pgf')
import matplotlib.pyplot as plt
# plt.rcParams.update({
#     # "pgf.texsystem": "pdflatex",
#     "font.family": "serif",  # use serif/main font for text elements
#     "text.usetex": True,     # use inline math for ticks
#     "pgf.rcfonts": False,    # don't setup fonts from rc parameters
#     "pgf.preamble": [
#          "\\usepackage{unicode-math}",   # unicode math setup
#          r"\setmainfont{DejaVu Serif}",  # serif font via preamble
#          ]
# })
import matplotlib.ticker as mtick

# np.set_printoptions(formatter={'float': lambda x: format(x, '6.3E')})

class plotter:
    def __init__(self, label: str):
        self.label = label

    def __call__(self, func):
        def plot(*args, **kwargs):
            ax = plotter.get_new_subplot(self.label)
            ax.yaxis.grid(True)
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
            self.plot_rtts(arg_parser.remove_outliers,
                           arg_parser.outlier_removal_type)
            self.plot_control_rtts(
                arg_parser.remove_outliers, arg_parser.outlier_removal_type)
        if arg_parser.show_histogram:
            self.plot_histogram(full_histogram=arg_parser.full_histogram, remove_outliers=arg_parser.remove_outliers,
                                outliers_mode=arg_parser.outlier_removal_type)
        if arg_parser.show_control_histogram:
            self.plot_histogram_control(full_histogram=arg_parser.full_histogram,
                                        remove_outliers=arg_parser.remove_outliers,
                                        outliers_mode=arg_parser.outlier_removal_type)
        if arg_parser.cr_detection:
            print(self.get_cr_detection_score())
        if arg_parser.ks_test:
            self.ks_test_dual()
        plt.show()
        # plt.savefig('../k8_thesis/figures/{0}_control.pgf'.format(arg_parser.title))
        pass

    # Plotting methods
    @plotter('RTT histogram')
    def plot_histogram(self, full_histogram: bool, remove_outliers: bool, outliers_mode: OutliersFilterMode, ax: Axes):
        """

        :param ax: Axes object to plot on
        :type full_histogram: bool Print the full histogram instead of two separate histograms based on whether the
        flooder was on or off
        """
        rtts, rtts_w_flooder, rtts_wo_flooder = self.data.non_normalized(rtt_type=RTTTYPE.ALL), self.data.non_normalized(
            rtt_type=RTTTYPE.WITH), self.data.non_normalized(rtt_type=RTTTYPE.WITHOUT)
        if remove_outliers:
            if outliers_mode == OutliersFilterMode.PERCENTAGE:
                rtts = Analyzer._remove_outliers_pct(rtts)
                rtts_w_flooder = Analyzer._remove_outliers_pct(rtts_w_flooder)
                rtts_wo_flooder = Analyzer._remove_outliers_pct(
                    rtts_wo_flooder)
            elif outliers_mode == OutliersFilterMode.FILTER:
                rtts = Analyzer._filter_data(rtts)
                rtts_w_flooder = Analyzer._filter_data(rtts_w_flooder)
                rtts_wo_flooder = Analyzer._filter_data(rtts_wo_flooder)
            else:
                rtts = Analyzer._remove_outliers_mode(rtts)
                rtts_wo_flooder = Analyzer._remove_outliers_mode(
                    rtts_w_flooder)
                rtts_wo_flooder = Analyzer._remove_outliers_mode(
                    rtts_wo_flooder)
        if full_histogram:
            ax.hist(rtts, bins=1000, alpha=0.5,
                    label='Round trip time frequency')
        else:
            ax.hist(rtts_w_flooder, color='Orange', bins=1000,
                    alpha=0.5, label='Round trip time with flooder')
            ax.hist(rtts_wo_flooder, color='Blue', bins=1000,
                    alpha=0.5, label='Round trip time without flooder')
        ax.legend()
        # ax.set_title('RTT histogram')
        ax.set_ylabel('Frequency')
        ax.set_xlabel('Normalized Request RTT')

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
            rtts = [i/1000000 for i in rtts]
            ax.plot(rtts, color='black')
        else:
            rtts, rtts_w_flooder, rtts_wo_flooder = self.data.rtts_with_start_times()
            ax.plot(*zip(*rtts), color='r', label='Rtts', alpha=0.5)
            ax.plot(*zip(*rtts_w_flooder), color='b',
                    label='Rtts with flooder', alpha=0.5)
            ax.plot(*zip(*rtts_wo_flooder), color='g',
                    label='Rtts without flooder', alpha=0.5)
        # ax.legend()
        # ax.yaxis.set_major_formatter(mtick.FormatStrFormatter('%.0e'))
        ax.ticklabel_format(axis='y', style='sci', useMathText=True)
        # ax.set_title(
        #     'Round Trip Time measured by the client for one minute of server activity with flooder running', fontsize=18)
        ax.set_ylabel('Round-trip Time (ms)', fontsize=18)
        ax.set_xlabel('Request number', fontsize=18)

    @plotter('RTT control plot')
    def plot_control_rtts(self, remove_outliers: bool, outliers_mode: OutliersFilterMode, ax: Axes):
        if remove_outliers:
            data, _, _ = self.control.rtts()
            if outliers_mode == OutliersFilterMode.PERCENTAGE:
                rtts = Analyzer._remove_outliers_pct(data)
            elif outliers_mode == OutliersFilterMode.FILTER:
                rtts = Analyzer._filter_data(data)
            else:
                rtts = Analyzer._remove_outliers_mode(data)
            rtts = [i/1000000 for i in rtts]
            ax.plot(rtts, color='black')
        else:
            rtts, rtts_w_flooder, rtts_wo_flooder = self.control.rtts_with_start_times()
            ax.plot(*zip(*rtts), color='r', label='Rtts', alpha=0.5)
            ax.plot(*zip(*rtts_w_flooder), color='b',
                    label='Rtts with flooder', alpha=0.5)
            ax.plot(*zip(*rtts_wo_flooder), color='g',
                    label='Rtts without flooder', alpha=0.5)
        # ax.legend()
        # ax.set_title(
        #     'Round Trip Time measured by the client for one minute of server activity without flooder running')
        ax.set_ylabel('Round-trip Time (ms)', fontsize=18)
        ax.set_xlabel('Request number', fontsize=18)
        # ax.yaxis.set_major_formatter(mtick.FormatStrFormatter('%.0e'))
        ax.ticklabel_format(axis='y', style='sci', useMathText=True)


    @plotter('Histogram with control')
    def plot_histogram_control(self, full_histogram: bool, remove_outliers: bool, outliers_mode: OutliersFilterMode,
                               ax: Axes):
        """

               :param ax: Axes object to plot on
               :type full_histogram: bool Print the full histogram instead of two separate histograms based on whether the
               flooder was on or off
               """
        ax.set_ylabel('Frequency')
        ax.set_xlabel('Normalized Request RTT')
        rtts, rtts_w_flooder, rtts_wo_flooder = self.control.non_normalized(rtt_type=RTTTYPE.ALL), self.control.non_normalized(
            rtt_type=RTTTYPE.WITH), self.control.non_normalized(rtt_type=RTTTYPE.WITHOUT)
        if remove_outliers:
            if outliers_mode == OutliersFilterMode.PERCENTAGE:
                rtts = Analyzer._remove_outliers_pct(rtts)
                rtts_w_flooder = Analyzer._remove_outliers_pct(rtts_w_flooder)
                rtts_wo_flooder = Analyzer._remove_outliers_pct(
                    rtts_wo_flooder)
            elif outliers_mode == OutliersFilterMode.FILTER:
                rtts = Analyzer._filter_data(rtts)
                rtts_w_flooder = Analyzer._filter_data(rtts_w_flooder)
                rtts_wo_flooder = Analyzer._filter_data(rtts_wo_flooder)
            else:
                rtts = Analyzer._remove_outliers_mode(rtts)
                rtts_wo_flooder = Analyzer._remove_outliers_mode(
                    rtts_w_flooder)
                rtts_wo_flooder = Analyzer._remove_outliers_mode(
                    rtts_wo_flooder)
        if full_histogram:
            ax.hist(rtts, bins=1000, alpha=0.5,
                    label='Round trip time frequency')
        else:
            ax.hist(rtts_w_flooder, color='Orange', bins=1000,
                    alpha=0.5, label='Round trip time with flooder')
            ax.hist(rtts_wo_flooder, color='Blue', bins=1000,
                    alpha=0.5, label='Round trip time without flooder')
        ax.legend()
        ax.ticklabel_format(axis='x', style='sci', useMathText=True)
        # ax.set_title('RTT histogram with control', fontsize=18)

    def get_cr_detection_score(self):
        mse_scale = 69.45
        rtts, rtts_control = self.get_comparison_rtts()
        # rtts = sorted(rtts)
        # rtts_control = sorted(rtts_control)
        rtts, rtts_control = zip(
            *zip_longest(rtts, rtts_control, fillvalue=np.mean(rtts)))
        # Generate histogram
        (hist, _), (hist_control, _) = np.histogram(
            rtts, bins=1000), np.histogram(rtts_control, bins=1000)
        # Remove zeros
        hist = [0.0000000001 if x == 0 else x for x in hist]
        hist_control = [0.0000000001 if x == 0 else x for x in hist_control]

        # Get all scores
        mse_score = mean_squared_error(rtts_control, rtts)
        # mse_score_hist = mean_squared_error(hist_control, hist)
        wass_metric = stats.wasserstein_distance(hist, hist_control)
        energy_distance = stats.energy_distance(hist, hist_control)
        kl_dist = stats.entropy(hist, hist_control)

        rtts_watermarked, rtts_watermarked_control = self.get_comparison_rtts(rtt_type=RTTTYPE.WITH)
        rtts_idling, rtts_idling_control = self.get_comparison_rtts(rtt_type=RTTTYPE.WITHOUT)
        hist_watermarked, _ = np.histogram(rtts_watermarked, bins=1000)
        hist_idling, _ = np.histogram(rtts_idling, bins=1000)
        hist_watermarked_control, _ = np.histogram(rtts_watermarked_control, bins=1000)
        hist_idling_control, _ = np.histogram(rtts_idling_control, bins=1000)
        mse_score_hist = mean_squared_error(hist_watermarked, hist_idling) - mean_squared_error(hist_watermarked_control, hist_idling_control)


        # ax = plotter.get_new_subplot('Probabily densities') ax.plot(hist, 'r+') ax.plot(hist_control, 'b+') print(
        # 'mse score: %f, mse_score_hist: %f, wasserstein distance: %f, energy distance: %f, kl_distance: %f' %(
        # mse_score, mse_score_hist, wass_metric, energy_distance, kl_dist))
        return mse_score, mse_score_hist, wass_metric, energy_distance, kl_dist

    def mann_whitney_test(self):
        rtts = Analyzer._filter_data(self.data.normalized())
        rtts_control = Analyzer._filter_data(self.control.normalized())
        stat, pval = stats.mannwhitneyu(
            rtts, rtts_control, alternative='greater')
        return "Mann Whitney Test: statistic value: %0.2f, pvalue: %0.2f" % (stat, pval)

    @plotter('Kolmogorov Smirnov Test')
    def ks_test(self, ax: Axes):
        rtts, rtts_control = self.get_comparison_rtts()
        stat, pval = stats.ks_2samp(rtts, rtts_control)
        ax.hist(rtts, color='Orange', bins=1000, alpha=0.5,
                label='Normalized RTTs with flooder')
        ax.hist(rtts_control, color='Blue', bins=1000,
                alpha=0.5, label='Normalized RTTs under control')
        ax.legend()
        ax.set_title('RTT histogram normalized by mean and stdev')

        hist = stats.rv_histogram(np.histogram(rtts_control, bins=1000))
        ax1 = plotter.get_new_subplot(
            'QQ Plot for RTTs with flooder against control')
        stats.probplot(rtts, plot=ax1, fit=True, dist=hist)
        ax1.set_title('RTTs QQ Plot')
        return "Kolmogorov Smirnov Two Sample Test: statistic value: %0.2f, pvalue: %0.2f" % (stat, pval)

    @plotter('Kolmogorov Smirnov Test Dual')
    def ks_test_dual(self, ax: Axes):
        rtts_w_flooder, rtts_w_control = self.get_comparison_rtts(RTTTYPE.WITH)
        rtts_wo_flooder, rtts_wo_control = self.get_comparison_rtts(
            RTTTYPE.WITHOUT)
        stat, pval = stats.ks_2samp(rtts_w_flooder, rtts_wo_flooder)
        print("Kolmogorov Smirnov Two Sample Test: statistic value: %0.2f, pvalue: %0.2f" % (
            stat, pval))
        stat, pval = stats.ks_2samp(rtts_w_control, rtts_wo_control)
        print("Kolmogorov Smirnov Two Sample Test: statistic value: %0.2f, pvalue: %0.2f" % (
            stat, pval))

    def get_comparison_rtts(self, rtt_type=RTTTYPE.ALL):
        rtts = Analyzer._remove_outliers_pct(Analyzer._filter_data(
            self.data.non_normalized(rtt_type)), lower=1, upper=95)
        rtts_control = Analyzer._remove_outliers_pct(Analyzer._filter_data(self.control.non_normalized(rtt_type)), lower=1,
                                                     upper=99)
        return rtts, rtts_control
