#!/usr/bin/env python
import unittest
from parsing.cycle_parser import CycleParser
from matplotlib import pyplot as plt


class TestCycles(unittest.TestCase):

    def __init__(self, *args, **kwargs):
        super(TestCycles, self).__init__(*args, **kwargs)
        self.data_file = open(
            '../data/new_data/duration_sensitivity_new_blades/client_times_duration_sensitivity_new_blades_2s_intf_control_12112019_1406')

    def test_plotting(self):
        c = CycleParser(self.data_file)
        for cycle in c.cycles:
            cycle.plot()
        # for cycle in c.cycles_control:
        #     cycle.plot()
        plt.show()
        pass


if __name__ == '__main__':
    unittest.main()
