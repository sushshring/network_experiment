#!/usr/bin/env python
import unittest
from parsing.cycle_parser import CycleParser
from matplotlib import pyplot as plt


class TestCycles(unittest.TestCase):

    def __init__(self, *args, **kwargs):
        super(TestCycles, self).__init__(*args, **kwargs)
        self.data_file = open(
            '../data/duration_sensitivity/client_times_duration_sensitivity_5s_intf_control_1252019_1135')

    def test_parsing(self):
        c = CycleParser(self.data_file)
        assert len(c.cycles) == 3
        assert len(c.cycles_control) == 3

    def test_plotting(self):
        c = CycleParser(self.data_file)
        for cycle in c.cycles:
            cycle.plot()
        for cycle in c.cycles_control:
            cycle.plot()
        plt.show()
        pass


if __name__ == '__main__':
    unittest.main()
