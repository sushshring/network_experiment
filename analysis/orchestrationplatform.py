import os
import multiprocessing
from glob import glob
from typing import Iterable, TextIO

from matplotlib.axes import Axes

from analyzer import Analyzer, plotter
from parser.data_parser import Parser
import numpy as np


class OrchestrationPlatform:
    def __init__(self, name, flooding_level, glob=None, sample=False):
        self._adv_score = None
        self._control_mean_dist = None
        self.name = name
        self.flooding_level = flooding_level
        self.sample = sample
        if glob is None:
            self.glob = '../data/%s/client_times_%s_%s_intf_control_*' % (self.name, self.name, self.flooding_level)
        else:
            self.glob = glob

    def __repr__(self):
        return "name: {}, flooding_level: {}, glob: {}".format(self.name, self.flooding_level, self.glob)

    def get_files(self) -> Iterable[TextIO]:
        files = glob(self.glob)
        for file in files:
            if self.sample and np.random.randint(0, 10) >= 5:
                continue
            if not is_non_zero_file(file):
                continue
            with open(file) as fh:
                yield fh
        pass

    def get_adv_score(self):
        scores = {}
        if os.getenv('MP_ENABLE', False):
            p = multiprocessing.Pool(processes = multiprocessing.cpu_count()-1)
            scors = p.map(self.parse_file, glob(self.glob))
            for i, file in enumerate(self.get_files()):
                scores[file.name] = scors[i]
        else:
            for file in self.get_files():
                scores[file.name] = self.parse_file(file.name)
        return scores

    def parse_file(self, filename):
        file = open(filename, 'r')
        parser = Parser(file)
        analyzer = Analyzer(parser.rtts, parser.rtts_control)
        # analyzer.ks_test()
        return analyzer.get_cr_detection_score()

    @plotter('Platform adversary score distribution')
    def plot_adv_score(self, ax: Axes):
        scores = zip(*self.adv_score.values())
        for score in scores:
            ax.hist(score, alpha=0.5, label=self.name)
        ax.set_title('Adversarial distribution for %s with %s flooding' % (self.name, self.flooding_level))
        ax.legend()
        pass

    @property
    def adv_score(self) -> dict:
        if self._adv_score is None:
            self._adv_score = self.get_adv_score()
        return self._adv_score

    @property
    def control_mean_dist(self):
        if self._control_mean_dist is None:
            self._control_mean_dist = self.get_control_mean_dist()
        return self._control_mean_dist

    def get_control_mean_dist(self):
        scores = {}
        means = []
        for file in self.get_files():
            parser = Parser(file)
            means.append(np.average(parser.rtts_control.rtts()[0]))
        return means
        pass


def is_non_zero_file(fpath):
    return os.path.isfile(fpath) and os.path.getsize(fpath) > 0
