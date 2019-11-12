#!/usr/bin/env python3.7
from orchestrationplatform import OrchestrationPlatform
from analyzer import plotter
from matplotlib.axes import Axes
from matplotlib import pyplot as plt
import numpy as np

base_folder = '../data/'
orchestrator = 'flooder_sensitivity_blades_high_flooding'

@plotter('Adversarial deception')
def main(ax: Axes):
    none = OrchestrationPlatform(name='none', flooding_level='none', glob=('%s/*%s_intf_control_*') %
            (base_folder+orchestrator, 'none'))
    ten = OrchestrationPlatform(name='ten', flooding_level='ten', glob=('%s/*%s_intf_control_*') %
            (base_folder+orchestrator, 'ten'))
    twenty = OrchestrationPlatform(name='twenty', flooding_level='twenty', glob=('%s/*%s_intf_control_*')
            % (base_folder+orchestrator, 'twenty'))
    thirty = OrchestrationPlatform(name='thirty', flooding_level='thirty', glob=('%s/*%s_intf_control_*')
            % (base_folder+orchestrator, 'thirty'))
    forty = OrchestrationPlatform(name='forty', flooding_level='forty', glob=('%s/*%s_intf_control_*') %
            (base_folder+orchestrator, 'forty'))
    fifty = OrchestrationPlatform(name='fifty', flooding_level='fifty', glob=('%s/*%s_intf_control_*') %
            (base_folder+orchestrator, 'fifty'))
    sixty = OrchestrationPlatform(name='sixty', flooding_level='sixty', glob=('%s/*%s_intf_control_*') %
            (base_folder+orchestrator, 'sixty'))
    seventy = OrchestrationPlatform(name='seventy', flooding_level='seventy',
            glob=('%s/*%s_intf_control_*') % (base_folder+orchestrator, 'seventy'))
    eighty = OrchestrationPlatform(name='eighty', flooding_level='eighty', glob=('%s/*%s_intf_control_*')
            % (base_folder+orchestrator, 'eighty'))
    ninety = OrchestrationPlatform(name='ninety', flooding_level='ninety', glob=('%s/*%s_intf_control_*')
            % (base_folder+orchestrator, 'ninety'))
    full = OrchestrationPlatform(name='full', flooding_level='full', glob=('%s/*%s_intf_control_*') %
            (base_folder+orchestrator, 'full'))

    data_dict = {
        0: none,
        10: ten,
        20: twenty,
        30: thirty,
        40: forty,
        50: fifty,
        60: sixty,
        70: seventy,
        80: eighty,
        90: ninety,
        100: full,
    }

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
        mse_values = [ i[1] for i in data_dict[k].get_adv_score().values()]
        result_dict[k] = None if not len(mse_values) > 0 else len([i for i in mse_values if i > 60]) / len(mse_values)
        ax.scatter([k for i in range(len(mse_values))], mse_values, label=data_dict[k].name, marker='o')
        median = np.median(mse_values)
        mean = np.average(mse_values)
        ax.plot([k, k+10], [median, median])
        ax.plot([k, k+10], [mean, mean])
        pass
    # x = [f[0] for f in arrs]
    # y = [np.mean([n[1] for n in p[1].get_adv_score().values()]) for p in arrs]
    # ax.scatter()
    # ax.scatter(x, y, )
    # plt.legend()
    print(result_dict)
    plt.show()
    pass

if __name__ == '__main__':
    main()
