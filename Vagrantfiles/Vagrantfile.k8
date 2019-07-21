# -*- mode: ruby -*-
# vi: set ft=ruby :

# Vagrantfile API/syntax version. Don't touch unless you know what you're doing!
VAGRANTFILE_API_VERSION = "2"

Vagrant.configure(VAGRANTFILE_API_VERSION) do |config|
  config.vm.define :target_vm do |target_vm|
    target_vm.vm.box = "generic/ubuntu1604"
    target_vm.vm.network :forwarded_port, host: 6000, guest: 30129, host_ip: "*"
    target_vm.vm.network :private_network,
      :libvirt__network_name => "default"
  end
  config.vm.provision :shell, path: "bootstrap.sh"
    # target_vm.vm.network :public_network, :dev => "eno1"
  config.vm.provider :libvirt do |libvirt|
    libvirt.cpus = 4
    libvirt.memory = 8192
  end

  config.vm.define :adv_vm do |adv_vm|
    adv_vm.vm.box = "generic/ubuntu1604"
    adv_vm.vm.network :private_network,
      :libvirt__network_name => "default"
  end
end
