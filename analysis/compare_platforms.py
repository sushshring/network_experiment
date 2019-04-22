#!/usr/bin/env python3.7
import matplotlib
matplotlib.use('Qt5Agg')
import numpy as np

from orchestrationplatform import OrchestrationPlatform
from concurrent import futures

import matplotlib.pyplot as plt


def main():
    plt.figure()
    platforms = [OrchestrationPlatform(name='k8', flooding_level='quad'),
                 OrchestrationPlatform(name='helios', flooding_level='quad'),
                 OrchestrationPlatform(name='swarm', flooding_level='quad')]
    for platform in platforms:
        execute_platform_analysis(platform)
    # with futures.ProcessPoolExecutor() as pool:
    #     for _ in pool.map(execute_platform_analysis, platforms):
    #         pass
    plt.show()


def execute_platform_analysis(platform):
    print('Platform: %s, adversary score: %f' % (platform.name, np.mean(platform.adv_score)))
    platform.plot_adv_score()


if __name__ == '__main__':
    main()
