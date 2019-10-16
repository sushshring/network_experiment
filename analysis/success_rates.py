#!/usr/bin/env python3.7
from analyzer import Analyzer, plotter
from orchestrationplatform import OrchestrationPlatform
from matplotlib.axes import Axes
from matplotlib import lines
from matplotlib import pyplot as plt
import argparse
import sys

@plotter('Adv score')
def main(with_cr_globber: str, without_cr_globber: str, threshold: int, ax: Axes):
    with_cr_orchestration_platform = OrchestrationPlatform("K8 with Co-residency and separation", "quad", with_cr_globber)
    without_cr_orchestration_platform = OrchestrationPlatform("K8 without Co-residency and separation", "quad", without_cr_globber)

    mse_hist_withcr = [x[1] for x in with_cr_orchestration_platform.adv_score.values()]
    mse_hist_withoutcr = [x[1] for x in without_cr_orchestration_platform.adv_score.values()]
    ax.hist(mse_hist_withcr, alpha=0.5, label='with co-residency', bins=100)
    ax.hist(mse_hist_withoutcr, alpha=0.5, label='without co-residency', bins=100)
    ax.legend()
    print('\n'.join(["{}: {:.2f}".format(k, v[1]) for k, v in with_cr_orchestration_platform.adv_score.items()]))
    print('\n'.join(["{}: {:.2f}".format(k, v[1]) for k, v in without_cr_orchestration_platform.adv_score.items()]))
    draw_timeline(with_cr_orchestration_platform.adv_score, threshold)
    minimal = min(mse_hist_withcr + mse_hist_withoutcr)
    maximal = max(mse_hist_withcr + mse_hist_withoutcr)
    tps = []
    fps = []
    for i in range(int(minimal), int(maximal)):
        tps.append(len([x for x in mse_hist_withcr if x > i]) / len(mse_hist_withcr))
        fps.append(1 - (len([x for x in mse_hist_withoutcr if x <= i]) / len(mse_hist_withoutcr)))

    plot_roc(tps, fps)
    tp = len([x for x in mse_hist_withcr if x > threshold]) / len(mse_hist_withcr)
    tn = len([x for x in mse_hist_withoutcr if x <= threshold]) / len(mse_hist_withoutcr)

    print("True positive: {:.2f}, false positive: {:.2f}, true negative: {:.2f}, false negative: {:.2f}".format(tp, 1 - tn, tn,1 - tp))
    plt.show()

@plotter('ROC Curve')
def plot_roc(tps, fps, ax):
    ax.plot(fps, tps)
    pass

@plotter('Adv scores timeline')
def draw_timeline(file_score_dict: dict, threshold: int, ax: Axes):
    runs = sorted(file_score_dict.keys())
    ax.plot(range(1, len(runs) + 1), [file_score_dict[x][1] for x in runs], '-o')
    ax.set_xlabel('Runs')
    ax.set_ylabel('MSE detection score')
    ax.set_title('MSE detection score over time')
    ax.add_line(lines.Line2D([0, len(runs)], [threshold, threshold]))


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Analyze the success rates for detection')
    parser.add_argument('with_cr_globber', metavar='crglobber', type=str, help='file glob pattern to match all data files for the run with co-resident flooder')
    parser.add_argument('without_cr_globber', metavar='ncrglobber', type=str, help='file glob pattern to match all data files for the run without co-resident flooder')
    parser.add_argument('--threshold', metavar='threshold', type=int, default=1000)
    args = parser.parse_args()
    main(args.with_cr_globber, args.without_cr_globber, args.threshold)