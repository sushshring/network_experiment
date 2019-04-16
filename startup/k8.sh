#!/bin/bash
function deps {
    apt update
    apt install -y software-properties-common \
	    net-tools \
	    wget \
	    vim \
	    git \
	    libltdl7
}

function configure_docker {
    cat > /etc/docker/daemon.json <<EOF
{
  "exec-opts": ["native.cgroupdriver=systemd"],
  "log-driver": "json-file",
  "log-opts": {
    "max-size": "100m"
  },
  "storage-driver": "overlay2"
}
EOF

    mkdir -p /etc/systemd/system/docker.service.d

    # Restart docker.
    systemctl daemon-reload
    systemctl restart docker
}

function configure_kubelet {
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
ExecStart=/usr/bin/kubelet $KUBELET_KUBECONFIG_ARGS $KUBELET_CONFIG_ARGS $KUBELET_KUBEADM_ARGS $KUBELET_EXTRA_ARGS
[Install]
WantedBy=multi-user.target" > /etc/systemd/system/kubelet.service
}

function k8 {
    cd kubernetes
    make
    cp ./_output/bin/* /usr/bin/
    cd ..
    configure_kubelet
    systemctl enable kubelet
}
function configure {
    wget https://download.docker.com/linux/ubuntu/dists/xenial/pool/stable/arm64/docker-ce_18.06.1~ce~3-0~ubuntu_arm64.deb 
    add-apt-repository ppa:longsleep/golang-backports -y
    apt update
    apt install -y net-tools \
        build-essential libgcrypt-dev libcurl4-gnutls-dev \
        vim python3 \
        htop iftop \
        apt-transport-https ca-certificates curl gnupg-agent software-properties-common \
        golang-go
    dpkg -i docker-ce_18.06.1~ce~3-0~ubuntu_arm64.deb
    git clone https://github.com/sushshring/network_experiment.git
    git clone https://github.com/sushshring/kubernetes.git
    apt install -y socat ebtables
    configure_docker
    k8
    echo "Finished :)"
}
deps
configure > /home/cfg.tmp.log
