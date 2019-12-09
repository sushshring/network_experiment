#!/usr/bin/env python
from typing import TextIO, List, Tuple
from parsing.rtt import Rtt


class Parser:
    rtts: Rtt
    rtts_control: Rtt
    file_path: str
    with_start_times: bool

    def __init__(self, file: TextIO, with_start: bool = False):
        self.with_start_times = with_start
        self.file = file
        self.parse()

    def parse(self):
        rtts, rtts_w, rttw_wo, ctrlrtt, ctrl_rttw, ctrl_rttswo = self._parse_client_times(
            self.file)
        self.rtts = Rtt(rtts, rtts_w, rttw_wo)
        self.rtts_control = Rtt(ctrlrtt, ctrl_rttw, ctrl_rttswo)

    @staticmethod
    def _parse_client_times(times: TextIO) -> (
            List[Tuple[float, float]], List[Tuple[float, float]], List[Tuple[float, float]]):
        rtts_wo_flooder = []
        rtts_w_flooder = []
        rtts = []
        ctrl_rtts = []
        ctrl_rtts_w_flooder = []
        ctrl_rtts_wo_flooder = []
        control = False
        flooder_on = True
        stime = 0
        first_start = None
        while True:
            line = times.readline()
            if not line:
                break
            if line.startswith('FLOODER_START'):
                flooder_on = True
            if line.startswith('FLOODER_END'):
                flooder_on = False
            if line.startswith('FLOODER_CONTROL'):
                control = True
            if line.startswith('START'):
                stime = float(line.split()[1])
                if not first_start:
                    first_start = stime
            if line.startswith('END'):
                etime = float(line.split()[1])
                rtt = etime - stime
                datapt = ((stime - first_start) / (10 ** 9), rtt)
                rtts.append(
                    datapt) if not control else ctrl_rtts.append(datapt)
                if flooder_on:
                    rtts_w_flooder.append(
                        datapt) if not control else ctrl_rtts_w_flooder.append(datapt)
                else:
                    rtts_wo_flooder.append(
                        datapt) if not control else ctrl_rtts_wo_flooder.append(datapt)
        return rtts, rtts_w_flooder, rtts_wo_flooder, ctrl_rtts, ctrl_rtts_w_flooder, ctrl_rtts_wo_flooder
