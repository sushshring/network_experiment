from typing import TextIO, List, Tuple
from parser.rtt import Rtt


class Parser:
    rtts: Rtt
    file_path: str
    with_start_times: bool

    def __init__(self, file: TextIO, with_start: bool = False):
        self.with_start_times = with_start
        self.file = file
        self.parse()

    def parse(self):
        self.rtts = Rtt(*self._parse_client_times(self.file))

    @staticmethod
    def _parse_client_times(times: TextIO) -> (
            List[Tuple[float, float]], List[Tuple[float, float]], List[Tuple[float, float]]):
        rtts_wo_flooder = []
        rtts_w_flooder = []
        rtts = []
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
            if line.startswith('START'):
                stime = float(line.split()[1])
                if not first_start:
                    first_start = stime
            if line.startswith('END'):
                etime = float(line.split()[1])
                rtt = etime - stime
                rtts.append(((stime - first_start) / (10 ** 9), rtt))
                if flooder_on:
                    rtts_w_flooder.append(((stime - first_start) / (10 ** 9), rtt))
                else:
                    rtts_wo_flooder.append(((stime - first_start) / (10 ** 9), rtt))
        return rtts, rtts_w_flooder, rtts_wo_flooder
