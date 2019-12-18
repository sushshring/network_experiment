from typing import List, Tuple
import numpy as np
from plotter import plotter
from matplotlib.axes import Axes
from matplotlib.lines import Line2D
from scipy.ndimage import gaussian_filter
from scipy.signal import convolve
from analyzer import Analyzer


class Cycle:
    _rtts_wo_flooder: List[Tuple[float, float]]
    _rtts_w_flooder: List[Tuple[float, float]]

    def __init__(self, with_flooder: List[Tuple[float, float]],
                 wo_flooder: List[Tuple[float, float]]):
        self._rtts_w_flooder = with_flooder
        self._rtts_wo_flooder = wo_flooder
        pass

    def rtts(self) -> (List[float], List[float]):
        return Cycle.__get_without_start_time(self._rtts_w_flooder), \
            Cycle.__get_without_start_time(self._rtts_wo_flooder)

    def rtts_with_start_times(self) -> \
            (List[Tuple[float, float]], List[Tuple[float, float]], List[Tuple[float, float]]):
        return self._rtts_w_flooder, self._rtts_wo_flooder

    def normalized(self):
        rtts_w, rtts_wo = self.rtts()
        return np.divide(np.subtract(rtts_w, np.mean(rtts_w)), np.std(rtts_w)), np.divide(np.subtract(rtts_wo, np.mean(rtts_wo)), np.std(rtts_wo))

    def score(self):
        on, off = self.rtts()
        if len(on) > 0 and len(off) > 0:
            average_flooding_rtt = np.mean(on)
            average_off_rtt = np.mean(off)
            return average_flooding_rtt - average_off_rtt
        return None

    @plotter('Plot')
    def plot(self, ax: Axes):
        rtts, noflood = self.rtts_with_start_times()
        if len(rtts) > 0 and len(noflood) > 0:
            times_w_flooder, rtts_w_flooder = zip(*rtts)
            rtts_w_flooder = Analyzer._filter_data(rtts_w_flooder)
            times_wo_flooder, rtts_wo_flooder = zip(*noflood)
            rtts_wo_flooder = Analyzer._filter_data(rtts_wo_flooder)
            mintime = min(times_w_flooder + times_wo_flooder)
            maxtime = max(times_w_flooder + times_wo_flooder)
            withflooder_avgline = Line2D([mintime, maxtime], [
                np.mean(rtts_w_flooder), np.mean(rtts_w_flooder)], color='Blue')
            ax.plot(times_w_flooder, rtts_w_flooder, label='Plot with flooder')
            # ax.set_xlabel('Time (s) ->')
            # ax.set_ylabel('RTT (ms)')
            ax.add_line(withflooder_avgline)
            woflooder_avgline = Line2D([mintime, maxtime], [
                np.mean(rtts_wo_flooder), np.mean(rtts_wo_flooder)], color='Orange', label='Average RTT baseline')
            ax.add_line(woflooder_avgline)
            ax.plot(times_wo_flooder, rtts_wo_flooder,
                    label='Plot without flooder')
        # ax.legend()
        pass

    @staticmethod
    @plotter('Plot')
    def plot_multiple(cycles, ax: Axes):
        for cycle in cycles:
            rtts, noflood = cycle.rtts_with_start_times()
            if len(rtts) > 0 and len(noflood) > 0:
                times_w_flooder, rtts_w_flooder = zip(*rtts)
                rtts_w_flooder = gaussian_filter(rtts_w_flooder, 2)
                # , [-1, 0, 1], mode='same')
                times_wo_flooder, rtts_wo_flooder = zip(*noflood)
                rtts_wo_flooder = gaussian_filter(rtts_wo_flooder, 2)
                mintime = min(times_w_flooder + times_wo_flooder)
                maxtime = max(times_w_flooder + times_wo_flooder)
                # , [-1, 0, 1], mode='same')
                ax.plot(times_w_flooder, rtts_w_flooder,
                        label='Plot with flooder', color='Blue')
                withflooder_avgline = Line2D([mintime, maxtime], [
                    np.mean(rtts_w_flooder), np.mean(rtts_w_flooder)], color='Blue')
                ax.plot(times_wo_flooder, rtts_wo_flooder,
                        label='Plot without flooder', color='Orange')
                woflooder_avgline = Line2D([mintime, maxtime], [
                    np.mean(rtts_wo_flooder), np.mean(rtts_wo_flooder)], color='Orange', label='Average RTT baseline')
                ax.add_line(woflooder_avgline)
                ax.add_line(withflooder_avgline)

    @staticmethod
    def __get_without_start_time(elements: List[Tuple[float, float]]) -> List[float]:
        if len(elements) == 0:
            return []
        return list(list(zip(*elements))[1])
