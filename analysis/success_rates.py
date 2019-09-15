#!/usr/bin/env python3.7
from analyzer import Analyzer
from orchestrationplatform import OrchestrationPlatform
import sys

def main(with_cr_globber: str, without_cr_globber: str):
    with_cr_orchestration_platform = OrchestrationPlatform("K8 with Co-residency and separation", "quad", with_cr_globber)
    without_cr_orchestration_platform = OrchestrationPlatform("K8 without Co-residency and separation", "quad", without_cr_globber)

    print([x[1] for x in with_cr_orchestration_platform.adv_score])
    print([x[1] for x in without_cr_orchestration_platform.adv_score])

    tp = len([x for x in with_cr_orchestration_platform.adv_score if x[1] > 1000]) / len(with_cr_orchestration_platform.adv_score)
    tn = len([x for x in without_cr_orchestration_platform.adv_score if x[1] <= 1000]) / len(without_cr_orchestration_platform.adv_score)

    print("True positive: {}, false positive: {}, true negative: {}, false negative: {}".format(tp, len(with_cr_orchestration_platform.adv_score) - tp, tn,len(without_cr_orchestration_platform.adv_score) - tn ))

if __name__ == '__main__':
    main(sys.argv[1], sys.argv[2])