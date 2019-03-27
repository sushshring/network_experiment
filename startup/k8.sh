#!/bin/bash
https://download.docker.com/linux/ubuntu/dists/xenial/pool/stable/arm64/docker-ce_18.06.1~ce~3-0~ubuntu_arm64.deb -o docker.deb
add-apt-repository ppa:longsleep/golang-backports
apt update
apt install -y net-tools \
    build-essential libgcrypt-dev libcurl4-gnutls-dev \
    vim python3 \
    htop iftop \
    apt-transport-https ca-certificates curl gnupg-agent software-properties-common \
    golang-go
dpkg -i docker.deb
git clone https://github.com/sushshring/network_experiment.git
git clone https://github.com/sushshring/kubernetes.git
apt install -y socat ebtables
cd kubernetes
make
cp _output/bin/* /usr/local/bin
cd ..
echo "[Unit]
Description=Kubelet service
After=docker.service
[Service]
Environment="KUBELET_KUBECONFIG_ARGS=--bootstrap-kubeconfig=/etc/kubernetes/bootstrap-kubelet.conf --kubeconfig=/etc/kubernetes/kubelet.conf"
Environment="KUBELET_CONFIG_ARGS=--config=/var/lib/kubelet/config.yaml"
# This is a file that "kubeadm init" and "kubeadm join" generates at runtime, populating
#the KUBELET_KUBEADM_ARGS variable dynamically
EnvironmentFile=-/var/lib/kubelet/kubeadm-flags.env
# This is a file that the user can use for overrides of the kubelet args as a last resort. Preferably,
#the user should use the .NodeRegistration.KubeletExtraArgs object in the configuration files instead.
# KUBELET_EXTRA_ARGS should be sourced from this file.
EnvironmentFile=-/etc/default/kubelet
ExecStart=
ExecStart=/usr/local/bin/kubelet $KUBELET_KUBECONFIG_ARGS $KUBELET_CONFIG_ARGS $KUBELET_KUBEADM_ARGS $KUBELET_EXTRA_ARGS

[Install]
WantedBy=multi-user.target" > /etc/systemd/system/kubelet.service
sudo systemctl enable kubelet
kubeadm init
