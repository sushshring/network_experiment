from typing import List, Tuple
import numpy as np
from plotter import plotter
from matplotlib.axes import Axes


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

    @plotter('Plot')
    def plot(self, ax: Axes):
        rtts_w_flooder, rtts_wo_flooder = self.rtts()
        ax.plot(rtts_w_flooder, label='Plot with flooder')
        ax.plot(rtts_wo_flooder, label='Plot without flooder')
        ax.legend()
        pass

    @staticmethod
    def __get_without_start_time(elements: List[Tuple[float, float]]) -> List[float]:
        if len(elements) == 0:
            return []
        return list(list(zip(*elements))[1])
