import os
from glob import glob
from typing import Iterable, TextIO

from matplotlib.axes import Axes

from analyzer import Analyzer, plotter
from parser.data_parser import Parser


class OrchestrationPlatform:
    def __init__(self, name, flooding_level):
        self._adv_score = None
        self.name = name
        self.flooding_level = flooding_level

    def get_files(self) -> Iterable[TextIO]:
        glob_pattern = '../data/client_times_%s_%s_intf_control_*' % (self.name, self.flooding_level)
        files = glob(glob_pattern)
        for file in files:
            if not is_non_zero_file(file):
                continue
            with open(file) as fh:
                yield fh
        pass

    def get_adv_score(self):
        scores = []
        for file in self.get_files():
            parser = Parser(file)
            analyzer = Analyzer(parser.rtts, parser.rtts_control)
            # analyzer.ks_test()
            scores.append(analyzer.get_cr_detection_score())
        return scores

    @plotter('Platform adversary score distribution')
    def plot_adv_score(self, ax: Axes):
        scores = self.adv_score
        print(scores)
        ax.hist(scores, color='Blue', alpha=0.5, label=self.name)
        ax.set_title('Adversarial distribution for %s with %s flooding' % (self.name, self.flooding_level))
        pass

    @property
    def adv_score(self):
        if self._adv_score is None:
            self._adv_score = self.get_adv_score()
        return self._adv_score


def is_non_zero_file(fpath):
    return os.path.isfile(fpath) and os.path.getsize(fpath) > 0
