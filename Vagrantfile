# -*- mode: ruby -*-
# vi: set ft=ruby :

# Vagrantfile API/syntax version. Don't touch unless you know what you're doing!
VAGRANTFILE_API_VERSION = "2"

Vagrant.configure(VAGRANTFILE_API_VERSION) do |config|
  config.vm.define :target_vm_heliosworker do |target_vm_heliosworker|
    target_vm_heliosworker.vm.box = "generic/ubuntu1604"
    target_vm_heliosworker.vm.network :forwarded_port, host: 6000, guest: 30129, host_ip: "*"
    target_vm_heliosworker.vm.network :private_network,
      :libvirt__network_name => "default"
  end
  config.vm.provision :shell, path: "bootstrap_helios.sh"
    # target_vm_heliosworker.vm.network :public_network, :dev => "eno1"
  config.vm.provider :libvirt do |libvirt|
    libvirt.cpus = 4
    libvirt.memory = 8192
  end
end
