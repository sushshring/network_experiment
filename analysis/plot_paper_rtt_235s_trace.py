from parsing.cycle_parser import CycleParser
from parsing.cycle import Cycle
from matplotlib import pyplot as plt
from glob import glob
glob3s = "../data/new_data/duration_sensitivity_new_blades/client_times_duration_sensitivity_new_blades_3s*"
cycles3s = []
for file in glob(glob3s):
    f = open(file)
    c = CycleParser(f)
    cycles3s.extend(c.cycles)

Cycle.plot_multiple(cycles3s)

glob5s = "../data/new_data/duration_sensitivity_new_blades/client_times_duration_sensitivity_new_blades_5s*"

cycles5s = []
for file in glob(glob5s):
    f = open(file)
    c = CycleParser(f)
    cycles5s.extend(c.cycles)

Cycle.plot_multiple(cycles5s)
plt.show()
