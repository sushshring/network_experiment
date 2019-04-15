#!/usr/bin/env bash

apt-get update
apt-get install -y \
    apt-transport-https \
    ca-certificates \
    curl \
    gnupg-agent \
    software-properties-common
curl -fsSL https://download.docker.com/linux/ubuntu/gpg | apt-key add -
add-apt-repository \
   "deb [arch=amd64] https://download.docker.com/linux/ubuntu \
   $(lsb_release -cs) \
   stable"
apt-get update
apt-get install -y docker-ce=18.06.0~ce~3-0~ubuntu \
	git \
	build-essential
git clone git://github.com/sushshring/network_experiment.git
sudo apt-key adv --keyserver hkp://keys.gnupg.net:80 --recv-keys 6F75C6183FF5E93D
echo "deb https://dl.bintray.com/spotify/deb trusty main" | sudo tee -a /etc/apt/sources.list.d/helios.list
apt update
apt install -y helios-solo
