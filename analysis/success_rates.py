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

    mse_hist_withcr = [x[1] for x in with_cr_orchestration_platform.adv_score.values()]
    mse_hist_withoutcr = [x[1] for x in without_cr_orchestration_platform.adv_score.values()]
    ax.hist(mse_hist_withcr, alpha=0.5, label='with co-residency', bins=10)
    ax.hist(mse_hist_withoutcr, alpha=0.5, label='without co-residency', bins=10)
    ax.legend()
    print('\n'.join(["{}: {}".format(k, v[1]) for k, v in with_cr_orchestration_platform.adv_score.items()]))
    print('\n'.join(["{}: {}".format(k, v[1]) for k, v in without_cr_orchestration_platform.adv_score.items()]))
    draw_timeline(with_cr_orchestration_platform.adv_score)
    minimal = min(mse_hist_withcr + mse_hist_withoutcr)
    maximal = max(mse_hist_withcr + mse_hist_withoutcr)
    tps = []
    fps = []
    for i in range(int(minimal), int(maximal)):
        tps.append(len([x for x in mse_hist_withcr if x > i]) / len(mse_hist_withcr))
        fps.append(1 - (len([x for x in mse_hist_withoutcr if x <= i]) / len(mse_hist_withoutcr)))

    plot_roc(tps, fps)
    tp = len([x for x in mse_hist_withcr if x > 724]) / len(mse_hist_withcr)
    tn = len([x for x in mse_hist_withoutcr if x <= 724]) / len(mse_hist_withoutcr)

    print("True positive: {:.2f}, false positive: {:.2f}, true negative: {:.2f}, false negative: {:.2f}".format(tp, 1 - tn, tn,1 - tp))
    plt.show()

@plotter('ROC Curve')
def plot_roc(tps, fps, ax):
    ax.plot(fps, tps)
    pass

@plotter('Adv scores timeline')
def draw_timeline(file_score_dict: dict, ax: Axes):
    runs = sorted(file_score_dict.keys())
    ax.scatter(runs, [file_score_dict[x][2] for x in runs], s=10)
    ax.add_line(lines.Line2D([0, len(runs)], [5.37, 5.37]))


if __name__ == '__main__':
    main(sys.argv[1], sys.argv[2])