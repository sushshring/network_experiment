import os
from glob import glob
from typing import Iterable, TextIO

from matplotlib.axes import Axes

from analyzer import Analyzer, plotter
from parser.data_parser import Parser


class OrchestrationPlatform:
    def __init__(self, name, flooding_level, glob=None):
        self._adv_score = None
        self.name = name
        self.flooding_level = flooding_level
        if glob is None:
            self.glob = '../data/%s/client_times_%s_%s_intf_control_*' % (self.name, self.name, self.flooding_level)
        else:
            self.glob = glob

    def __repr__(self):
        return "name: {}, flooding_level: {}, glob: {}".format(self.name, self.flooding_level, self.glob)

    def get_files(self) -> Iterable[TextIO]:
        files = glob(self.glob)
        for file in files:
            if not is_non_zero_file(file):
                continue
            with open(file) as fh:
                yield fh
        pass

    def get_adv_score(self):
        scores = {}
        for file in self.get_files():
            parser = Parser(file)
            analyzer = Analyzer(parser.rtts, parser.rtts_control)
            # analyzer.ks_test()
            scores[file.name] = analyzer.get_cr_detection_score()
        return scores

    @plotter('Platform adversary score distribution')
    def plot_adv_score(self, ax: Axes):
        scores = zip(*self.adv_score)
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


def is_non_zero_file(fpath):
    return os.path.isfile(fpath) and os.path.getsize(fpath) > 0
