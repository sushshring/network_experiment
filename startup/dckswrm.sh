#!/bin/bash
curl -fsSL https://download.docker.com/linux/ubuntu/gpg | apt-key add -
add-apt-repository \
   "deb [arch=amd64] https://download.docker.com/linux/ubuntu \
   $(lsb_release -cs) \
   stable"
apt update
apt install -y net-tools \
    build-essential libgcrypt-dev libcurl4-gnutls-dev \
    vim python3 \
    htop iftop \
    apt-transport-https ca-certificates curl gnupg-agent software-properties-common \
    docker.io
git clone https://github.com/sushshring/network_experiment.git
