#!/usr/bin/env python3.7
import matplotlib
matplotlib.use('Qt5Agg')
import numpy as np

from orchestrationplatform import OrchestrationPlatform

import matplotlib.pyplot as plt


def main():
    plt.figure()
    platforms = [OrchestrationPlatform(name='k8_with_separation', flooding_level='quad'),
                #  OrchestrationPlatform(name='k8', flooding_level='none', glob='../data/client_times_%s_%s_intf_control_*' % ('k8', 'no')),
                 OrchestrationPlatform(name='k8', flooding_level='quad'),
                #  OrchestrationPlatform(name='helios', flooding_level='none'),
                 OrchestrationPlatform(name='swarm_with_separation', flooding_level='quad'),
                 OrchestrationPlatform(name='swarm', flooding_level='quad'),
                 OrchestrationPlatform(name='helios_with_separation', flooding_level='quad'),
                 OrchestrationPlatform(name='helios', flooding_level='quad')]
    for platform in platforms:
        execute_platform_analysis(platform)
    # with futures.ProcessPoolExecutor() as pool:
    #     for _ in pool.map(execute_platform_analysis, platforms):
    #         pass
    plt.show()


def execute_platform_analysis(platform):
    scores = [np.mean(x) for x in zip(*platform.adv_score)]
    print('Platform: %s, flooding: %s' % (platform.name, platform.flooding_level))
    print(scores)
    platform.plot_adv_score()


if __name__ == '__main__':
    main()
