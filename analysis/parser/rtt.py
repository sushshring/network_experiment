from typing import List, Tuple
from enum import Enum
import numpy as np

class RTTTYPE(Enum):
    ALL = 1
    WITH = 2
    WITHOUT = 3


class Rtt:
    rtts_wo_flooder: List[Tuple[float, float]]
    rtts_w_flooder: List[Tuple[float, float]]

    def __init__(self, rtts: List[Tuple[float, float]], with_flooder: List[Tuple[float, float]],
                 wo_flooder: List[Tuple[float, float]]):
        self._rtts = rtts
        self._rtts_w_flooder = with_flooder
        self._rtts_wo_flooder = wo_flooder
        pass

    def rtts(self) -> (List[float], List[float], List[float]):
        return Rtt.__get_without_start_time(self._rtts), \
               Rtt.__get_without_start_time(self._rtts_w_flooder), \
               Rtt.__get_without_start_time(self._rtts_wo_flooder)

    def rtts_with_start_times(self) -> \
            (List[Tuple[float, float]], List[Tuple[float, float]], List[Tuple[float, float]]):
        return self._rtts, self._rtts_w_flooder, self._rtts_wo_flooder

    def normalized(self, rtt_type: RTTTYPE = RTTTYPE.ALL):
        if rtt_type == RTTTYPE.ALL:
            rtts = self.rtts()[0]
        elif rtt_type == RTTTYPE.WITH:
            rtts = self.rtts()[1]
        elif rtt_type == RTTTYPE.WITHOUT:
            rtts = self.rtts()[2]
        else:
            rtts = self.rtts()[0]
        return np.divide(np.subtract(rtts, np.mean(rtts)), np.std(rtts))

    def non_normalized(self, rtt_type: RTTTYPE = RTTTYPE.ALL):
        if rtt_type == RTTTYPE.ALL:
            rtts = self.rtts()[0]
        elif rtt_type == RTTTYPE.WITH:
            rtts = self.rtts()[1]
        elif rtt_type == RTTTYPE.WITHOUT:
            rtts = self.rtts()[2]
        else:
            rtts = self.rtts()[0]
        return rtts

    @staticmethod
    def __get_without_start_time(elements: List[Tuple[float, float]]) -> List[float]:
        if len(elements) == 0:
            return []
        return list(list(zip(*elements))[1])
