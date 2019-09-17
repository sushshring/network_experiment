#!/usr/bin/env python3.7
from analyzer import Analyzer, plotter
from orchestrationplatform import OrchestrationPlatform
from matplotlib.axes import Axes
from matplotlib import pyplot as plt
import sys

@plotter('Adv score')
def main(with_cr_globber: str, without_cr_globber: str, ax: Axes):
    with_cr_orchestration_platform = OrchestrationPlatform("K8 with Co-residency and separation", "quad", with_cr_globber)
    without_cr_orchestration_platform = OrchestrationPlatform("K8 without Co-residency and separation", "quad", without_cr_globber)

    ax.hist([x[1] for x in with_cr_orchestration_platform.adv_score], alpha=0.5, label='with co-residency', bins=1000)
    ax.hist([x[1] for x in without_cr_orchestration_platform.adv_score], alpha=0.5, label='without co-residency', bins=1000)
    ax.legend()
    plt.show()
    print([x[1] for x in with_cr_orchestration_platform.adv_score])
    print([x[1] for x in without_cr_orchestration_platform.adv_score])

    tp = len([x for x in with_cr_orchestration_platform.adv_score if x[2] > 5.37]) / len(with_cr_orchestration_platform.adv_score)
    tn = len([x for x in without_cr_orchestration_platform.adv_score if x[2] <= 5.37]) / len(without_cr_orchestration_platform.adv_score)

    print("True positive: {:.2f}, false positive: {:.2f}, true negative: {:.2f}, false negative: {:.2f}").format(tp, 1 - tp, tn,1 - tn)


if __name__ == '__main__':
    main(sys.argv[1], sys.argv[2])