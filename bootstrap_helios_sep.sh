#!/usr/bin/env bash

apt-get update
apt-get install -y \
    apt-transport-https \
    ca-certificates \
    curl \
    gnupg-agent \
    software-properties-common \
    maven \
    default-jdk \
    zookeeperd

curl -fsSL https://download.docker.com/linux/ubuntu/gpg | apt-key add -
add-apt-repository \
   "deb [arch=amd64] https://download.docker.com/linux/ubuntu \
   $(lsb_release -cs) \
   stable"
apt-get update
apt-get install -y docker-ce=18.06.0~ce~3-0~ubuntu \
	git
git clone https://github.com/spotify/helios.git
cd helios
mvn clean package -DskipTests
cp bin/helios /usr/bin/helios
cp bin/helios-master /usr/bin/helios-master
cp bin/helios-agent /usr/bin/helios-agent
