#!/usr/bin/env python3.7
import matplotlib
matplotlib.use('Qt5Agg')
import numpy as np

from orchestrationplatform import OrchestrationPlatform

import matplotlib.pyplot as plt


def main():
    plt.figure()
    # platforms = [OrchestrationPlatform(name='k8_with_separation', flooding_level='quad'),
    #             #  OrchestrationPlatform(name='k8', flooding_level='none', glob='../data/client_times_%s_%s_intf_control_*' % ('k8', 'no')),
    #              OrchestrationPlatform(name='k8', flooding_level='quad'),
    #             #  OrchestrationPlatform(name='helios', flooding_level='none'),
    #              OrchestrationPlatform(name='swarm_with_separation', flooding_level='quad'),
    #              OrchestrationPlatform(name='swarm', flooding_level='quad'),
    #              OrchestrationPlatform(name='helios_with_separation', flooding_level='quad'),
    #              OrchestrationPlatform(name='helios', flooding_level='quad')]
    platforms = [
        OrchestrationPlatform(name='Kubernetes', flooding_level='quad', glob='../data/k8_with_separation_blades_real_cooldown/*'),
        OrchestrationPlatform(name='Helios', flooding_level='quad', glob='../data/helios_with_separation_blades_real_cooldown/*'),
        OrchestrationPlatform(name='Docker Swarm', flooding_level='quad', glob='../data/swarm_with_separation_blades_real_cooldown/*'),
        # OrchestrationPlatform(name='Kubernetes', flooding_level='quad', glob='../data/k8_with_separation_blades_cooldown_noisy/client_times_k8_with_separation_blades_cooldown_noisy_quad_cr*'),
        # OrchestrationPlatform(name='Helios', flooding_level='quad', glob='../data/helios_with_separation_blades_cooldown_noisy/client_times_helios_with_separation_blades_cooldown_noisy_quad_cr*'),
        # OrchestrationPlatform(name='Docker Swarm', flooding_level='quad', glob='../data/swarm_with_separation_blades_cooldown_noisy/client_times_swarm_with_separation_blades_cooldown_noisy_quad_cr*'),
    ]
    scores = {}
    for platform in platforms:
        scores[platform.name] = execute_platform_analysis(platform)
    # with futures.ProcessPoolExecutor() as pool:
    #     for _ in pool.map(execute_platform_analysis, platforms):
    #         pass
    plot_bar_chart(scores)
    plt.show()


def plot_bar_chart(scores_dict: dict):
    platforms = scores_dict.keys()
    scores = [scores_dict[x][2] for x in platforms]
    x_pos = [i for i, _ in enumerate(platforms)]

    plt.bar(platforms, scores, color='orange')
    plt.xlabel('Platform type')
    plt.ylabel('Wasserstein metric score')
    plt.title('Wasserstein metric score with Master node separation and benchmark noise on SIIS cluster')

    plt.xticks(x_pos, platforms)

def execute_platform_analysis(platform):
    scores = [np.mean(x) for x in zip(*platform.adv_score.values())]
    print('Platform: %s, flooding: %s' % (platform.name, platform.flooding_level))
    print(scores)
    # platform.plot_adv_score()
    return scores


if __name__ == '__main__':
    main()
