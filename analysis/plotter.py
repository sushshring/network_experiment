#!/usr/bin/env python
import matplotlib
import math
matplotlib.use('Qt5Agg')
# matplotlib.rc('xtick', labelsize=16)
# matplotlib.rc('ytick', labelsize=16)
# matplotlib.rcParams.update({'font.size': 25})
# matplotlib.use('pgf')
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

# np.set_printoptions(formatter={'float': lambda x: format(x, '6.3E')})
import matplotlib.ticker as mtick
import matplotlib.pyplot as plt
from matplotlib.axes import Axes


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
