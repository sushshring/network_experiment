# -*- mode: ruby -*-
# vi: set ft=ruby :

# Vagrantfile API/syntax version. Don't touch unless you know what you're doing!
VAGRANTFILE_API_VERSION = "2"

Vagrant.configure(VAGRANTFILE_API_VERSION) do |config|
  config.vm.define :target_vm_k8worker do |target_vm_k8worker|
    target_vm_k8worker.vm.box = "generic/ubuntu1604"
    target_vm_k8worker.vm.network :forwarded_port, host: 6000, guest: 30129, host_ip: "*"
    target_vm_k8worker.vm.network :private_network,
      :libvirt__network_name => "default"
  end
  config.vm.provision :shell, path: "bootstrap.sh"
    # target_vm_k8worker.vm.network :public_network, :dev => "eno1"
  config.vm.provider :libvirt do |libvirt|
    libvirt.cpus = 4
    libvirt.memory = 8192
  end

  config.vm.define :adv_vm_k8worker do |adv_vm_k8worker|
    adv_vm_k8worker.vm.box = "generic/ubuntu1604"
    adv_vm_k8worker.vm.network :private_network,
      :libvirt__network_name => "default"
  end
end
