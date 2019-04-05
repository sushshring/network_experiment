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
}
deps
configure > /home/cfg.tmp.log
