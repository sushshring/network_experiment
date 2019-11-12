#!/usr/bin/env python3.7
from orchestrationplatform import OrchestrationPlatform
from scipy import stats
import numpy as np
from statsmodels.stats import weightstats as s
from analyzer import plotter
from matplotlib import pyplot as plt
from matplotlib.pyplot import Axes
import argparse

@plotter('')
def main(orchestrator_name: str, orchestrator_glob: str, baseline_glob: str, ax: Axes):
    orchestrator = OrchestrationPlatform(orchestrator_name, 'quad', orchestrator_glob)
    baseline = OrchestrationPlatform('baseline', 'quad', baseline_glob)

    orc_mean = orchestrator.control_mean_dist
    base_mean = baseline.control_mean_dist
    
    ax.hist(orc_mean, label='orchestrator', alpha=0.5, density=True)
    ax.hist(base_mean, label='baseline', alpha=0.5, density=True)
    print(stats.ttest_ind(orc_mean, base_mean))
    print(np.average(orc_mean))
    print(np.std(orc_mean))
    print(np.average(base_mean))
    print(np.std(base_mean))


    # orc_mse = [s[1] for s in orchestrator.adv_score.values()]
    # baseline_mse = [s[1] for s in baseline.adv_score.values()]

    # ax.hist(orc_mse, label='orchestrator', alpha=0.5)
    # ax.hist(baseline_mse, label='baseline', alpha=0.5)
    ax.legend()
    # wass_metr = stats.wasserstein_distance(orc_mse, baseline_mse)

    # print("Wasserstein values for %s: %f" % (orchestrator_name, wass_metr))
    plt.show()


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Get effect of orchestrator on control baseline')
    parser.add_argument('orchestrator_glob', metavar='orcglobber', type=str, help='file glob pattern to match all data files for the run with orchestrator')
    parser.add_argument('baseline_glob', metavar='baseglobber', type=str, help='file glob pattern to match all data files for the run without orchestrator')
    parser.add_argument('orchestrator_name', metavar='orcname', type=str)
    args = parser.parse_args()
    main(args.orchestrator_name, args.orchestrator_glob, args.baseline_glob)

