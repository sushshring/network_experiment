#!/usr/bin/env python
from typing import TextIO, List, Tuple
from parsing.cycle import Cycle
from sortedcontainers import SortedDict


class CycleParser:
    cycles: List[Cycle]
    cycles_control: List[Cycle]
    file_path: str
    with_start_times: bool

    def __init__(self, file: TextIO, with_start: bool = False):
        self.with_start_times = with_start
        self.file = file
        self.cycles = []
        self.cycles_control = []
        self.parse()

    def parse(self):
        sorted_times = self._parse_all_times(self.file)
        self.cycles, self.cycles_control = self._parse_cycles(sorted_times)

        # flooding_cycles, non_flooding_cycles, flooding_cycles_control, non_flooding_cycles_control = self._parse_cycles(
        #     self.file)
        # assert len(flooding_cycles) == len(non_flooding_cycles)
        # assert len(flooding_cycles_control) == len(non_flooding_cycles_control)

        # for i in range(len(flooding_cycles)):
        #     self.cycles.append(
        #         Cycle(flooding_cycles[i], non_flooding_cycles[i]))
        # for i in range(len(flooding_cycles_control)):
        #     self.cycles_control.append(
        #         Cycle(flooding_cycles_control[i], non_flooding_cycles_control[i]))

        # @staticmethod
        # def _parse_cycles(times: TextIO) -> (
        #     List[List[Tuple[float, float]]], List[List[Tuple[float, float]]
        #                                           ], List[List[Tuple[float, float]]], List[List[Tuple[float, float]]]
        # ):
        #     flooding_cycles = []
        #     non_flooding_cycles = []
        #     flooding_cycles_control = []
        #     non_flooding_cycles_control = []
        #     control = False
        #     while True:
        #         line = times.readline()
        #         if line.startswith('FLOODER_START'):
        #             break

        #     while True:
        #         result = CycleParser._parse_next_cycle(times)
        #         if not result:
        #             break
        #         rtts_w_flooder, rtts_wo_flooder, start_of_control = result[0], result[1], result[2]
        #         control = control or start_of_control
        #         if control:
        #             flooding_cycles_control.append(rtts_w_flooder)
        #             non_flooding_cycles_control.append(rtts_wo_flooder)
        #         else:
        #             flooding_cycles.append(rtts_w_flooder)
        #             non_flooding_cycles.append(rtts_wo_flooder)
        #     return flooding_cycles, non_flooding_cycles, flooding_cycles_control, non_flooding_cycles_control

        # @staticmethod
        # def _parse_next_cycle(times: TextIO) -> (
        #         List[Tuple[float, float]], List[Tuple[float, float]]):
        #     """
        #     This function should be called after the FLOODER_START line has been read already. It will
        #     exit after the next FLOODER_START line.
        #     """
        #     rtts_wo_flooder = []
        #     rtts_w_flooder = []
        #     flooder_on = True
        #     stime = 0
        #     first_start = None
        #     control = False
        #     while True:
        #         line = times.readline()
        #         if not line:
        #             return None
        #         # Other cycle specific exit conditions
        #         if line.startswith("FLOODER_START"):
        #             break
        #         if line.startswith("FLOODER_CONTROL"):
        #             control = True
        #         if line.startswith("FLOODER_END"):
        #             flooder_on = False
        #         if line.startswith("START"):
        #             stime = float(line.split()[1])
        #             if not first_start:
        #                 first_start = stime
        #         if line.startswith("END"):
        #             if not first_start:
        #                 continue
        #             etime = float(line.split()[1])
        #             rtt = etime - stime
        #             datapt = ((stime - first_start) / (10**9), rtt)
        #             if flooder_on:
        #                 rtts_w_flooder.append(datapt)
        #             else:
        #                 rtts_wo_flooder.append(datapt)
        #     return rtts_w_flooder, rtts_wo_flooder, control

    @staticmethod
    def _parse_all_times(times: TextIO) -> (SortedDict):
        sorted_times = SortedDict()
        stime = 0
        first_start = None
        while True:
            line: str = times.readline()
            if not line:
                break
            if line.startswith("FLOODER_START"):
                time = int(line.split()[1].strip())
                while time in sorted_times:
                    time += 1
                sorted_times[time] = "FLOODER_START"
            if line.startswith("FLOODER_END"):
                time = int(line.split()[1].strip())
                while time in sorted_times:
                    time += 1
                sorted_times[time] = "FLOODER_END"
            if line.startswith("FLOODER_CONTROL"):
                time = int(line.split()[1].strip())
                while time in sorted_times:
                    time += 1
                sorted_times[time] = "FLOODER_CONTROL"
            if line.startswith("START"):
                stime = int(line.split()[1])
            if line.startswith("END"):
                assert stime != 0
                etime = int(line.split()[1])
                rtt = etime - stime
                sorted_times[etime] = rtt / 10**6
        return sorted_times

    @staticmethod
    def _parse_cycles(times: SortedDict) -> (List[Cycle], List[Cycle]):
        flooding_cycle = []
        no_flooding_cycle = []

        cycles = []
        control = []

        flooder_on = None
        flooder_control = False
        mark_cycle_start_time = False
        cycle_start_time = 0
        for time, value in times.items():
            if value == "FLOODER_CONTROL":
                flooder_control = True
            elif value == "FLOODER_START":
                mark_cycle_start_time = True
                if flooder_on is None:
                    flooder_on = True
                    continue
                cycle = Cycle(flooding_cycle, no_flooding_cycle)
                flooding_cycle = []
                no_flooding_cycle = []
                if not flooder_control:
                    cycles.append(cycle)
                else:
                    control.append(cycle)
                flooder_on = True
            elif value == "FLOODER_END":
                flooder_on = False
            if flooder_on is None:
                continue
            if type(value) != str:
                # if mark_cycle_start_time:
                #     cycle_start_time = time
                # mark_cycle_start_time = False
                if flooder_on:
                    flooding_cycle.append(
                        ((time - cycle_start_time) / 10**9, value))
                else:
                    no_flooding_cycle.append(
                        ((time - cycle_start_time) / 10**9, value))
        return cycles, control
        pass
