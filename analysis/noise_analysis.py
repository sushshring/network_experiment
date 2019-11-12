#!/usr/bin/env python3.7
from orchestrationplatform import OrchestrationPlatform
from analyzer import plotter
from matplotlib import pyplot as plt


@plotter('')
def main(ax):
    data_dict = { 
        0: OrchestrationPlatform('zero noise', 'quad', '../data/noise_sensitivity/client_times_noise_sensitivity_quad_noise_zero*', False),
        10: OrchestrationPlatform('zero noise', 'quad', '../data/noise_sensitivity/client_times_noise_sensitivity_quad_noise_10*', False),
        20: OrchestrationPlatform('zero noise', 'quad', '../data/noise_sensitivity/client_times_noise_sensitivity_quad_noise_20*', False),
        30: OrchestrationPlatform('zero noise', 'quad', '../data/noise_sensitivity/client_times_noise_sensitivity_quad_noise_30*', False),
        40: OrchestrationPlatform('zero noise', 'quad', '../data/noise_sensitivity/client_times_noise_sensitivity_quad_noise_40*', False),
        50: OrchestrationPlatform('zero noise', 'quad', '../data/noise_sensitivity/client_times_noise_sensitivity_quad_noise_50*', False),
        60: OrchestrationPlatform('zero noise', 'quad', '../data/noise_sensitivity/client_times_noise_sensitivity_quad_noise_60*', False),
        70: OrchestrationPlatform('zero noise', 'quad', '../data/noise_sensitivity/client_times_noise_sensitivity_quad_noise_70*', False),
        80: OrchestrationPlatform('zero noise', 'quad', '../data/noise_sensitivity/client_times_noise_sensitivity_quad_noise_80*', False),
        90: OrchestrationPlatform('zero noise', 'quad', '../data/noise_sensitivity/client_times_noise_sensitivity_quad_noise_90*', False),
        100: OrchestrationPlatform('zero noise', 'quad', '../data/noise_sensitivity/client_times_noise_sensitivity_quad_noise_100*', False),
    }

    print(data_dict[100].adv_score)

    result_dict = {
        0: None,
        10: None,
        20: None,
        30: None,
        40: None,
        50: None,
        60: None,
        70: None,
        80: None,
        90: None,
        100: None,
    }

    for k in data_dict:
        succ = get_succ(data_dict[k])
        ax.scatter(k, succ, label=data_dict[k].name, marker='o')
        # ax.plot([k, k+10], [median, median])
        # ax.plot([k, k+10], [mean, mean])
        pass
    plt.show()
    pass

def get_succ(p: OrchestrationPlatform):
    mse_values = [ i[1] for i in p.get_adv_score().values()]
    return None if not len(mse_values) > 0 else len([i for i in mse_values if i > 50]) / len(mse_values)

if __name__ == '__main__':
    main()