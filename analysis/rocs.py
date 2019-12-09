#!/usr/bin/env python3.7
from plotter import plotter
from matplotlib import pyplot as plt
from matplotlib.pyplot import Axes
import pickle


@plotter('ROC curves for various environments')
def main(ax: Axes):
    k8_local = pickle.load(open('../data/roc/k8_local.p', 'rb'))
    swarm_local = pickle.load(open('../data/roc/swarm_local.p', 'rb'))
    helios_local = pickle.load(open('../data/roc/helios_local.p', 'rb'))
    k8_blades = pickle.load(open('../data/roc/k8_blades.p', 'rb'))
    swarm_blades = pickle.load(open('../data/roc/swarm_blades.p', 'rb'))
    helios_blades = pickle.load(open('../data/roc/helios_blades.p', 'rb'))
    aws = pickle.load(open('../data/roc/aws_initial.p', 'rb'))
    azure = pickle.load(open('../data/roc/azure_initial.p', 'rb'))
    ax.plot(*reversed(k8_local), 's',
            label="Kubernetes on local cluster", markersize=12)
    ax.plot(*reversed(swarm_local), '*',
            label="Docker Swarm on local cluster", markersize=12)
    ax.plot(*reversed(helios_local), 'o',
            label="Helios on local cluster", markersize=12)
    ax.plot(*reversed(k8_blades), 'v',
            label="Kubernetes on cloud cluster", markersize=12)
    ax.plot(*reversed(swarm_blades), '^',
            label="Docker Swarm on cloud cluster", markersize=12)
    ax.plot(*reversed(helios_blades), '<',
            label="Helios on cloud cluster", markersize=12)
    ax.plot(*reversed(aws), 'd', label="Docker on AWS", markersize=12)
    ax.plot(*reversed(azure), 'x', label="Docker on Azure", markersize=12)
    ax.set_xlabel('False Positives')
    ax.legend()
    ax.set_ylabel('True Positives')
    plt.show()


if __name__ == '__main__':
    main()
