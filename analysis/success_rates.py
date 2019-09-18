#!/usr/bin/env python3.7
from analyzer import Analyzer, plotter
from orchestrationplatform import OrchestrationPlatform
from matplotlib.axes import Axes
from matplotlib import lines
from matplotlib import pyplot as plt
import sys

@plotter('Adv score')
def main(with_cr_globber: str, without_cr_globber: str, ax: Axes):
    with_cr_orchestration_platform = OrchestrationPlatform("K8 with Co-residency and separation", "quad", with_cr_globber)
    without_cr_orchestration_platform = OrchestrationPlatform("K8 without Co-residency and separation", "quad", without_cr_globber)

    ax.hist([x[1] for x in with_cr_orchestration_platform.adv_score.values()], alpha=0.5, label='with co-residency', bins=1000)
    ax.hist([x[1] for x in without_cr_orchestration_platform.adv_score.values()], alpha=0.5, label='without co-residency', bins=1000)
    ax.legend()
    print('\n'.join(["{}: {}".format(k, v[1]) for k, v in with_cr_orchestration_platform.adv_score.items()]))
    print('\n'.join(["{}: {}".format(k, v[1]) for k, v in without_cr_orchestration_platform.adv_score.items()]))
    draw_timeline(with_cr_orchestration_platform.adv_score)
    tp = len([x for x in with_cr_orchestration_platform.adv_score.values() if x[2] > 5.37]) / len(with_cr_orchestration_platform.adv_score.values())
    tn = len([x for x in without_cr_orchestration_platform.adv_score.values() if x[2] <= 5.37]) / len(without_cr_orchestration_platform.adv_score.values())

    print("True positive: {:.2f}, false positive: {:.2f}, true negative: {:.2f}, false negative: {:.2f}".format(tp, 1 - tp, tn,1 - tn))
    plt.show()


@plotter('Adv scores timeline')
def draw_timeline(file_score_dict: dict, ax: Axes):
    runs = sorted(file_score_dict.keys())
    ax.scatter(runs, [file_score_dict[x][2] for x in runs], s=10)
    ax.add_line(lines.Line2D([0, len(runs)], [5.37, 5.37]))


if __name__ == '__main__':
    main(sys.argv[1], sys.argv[2])