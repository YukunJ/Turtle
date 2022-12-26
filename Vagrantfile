# -*- mode: ruby -*-
# vi: set ft=ruby :

$VAGRANT_EXTRA_STEPS = <<~SCRIPT
  git config --global --add safe.directory '*'
  echo "cd /vagrant" >> /home/vagrant/.bashrc
SCRIPT

$SET_NETWORK = <<~'SCRIPT'
  IFNAME=$(ifconfig | grep -B1 10.0.1. | grep -o "^\w*")
  echo "export IFNAME=$IFNAME" >> /home/vagrant/.bashrc
  sudo echo "export IFNAME=$IFNAME" >> /root/.bashrc

  sudo sed -i 's/PasswordAuthentication no/PasswordAuthentication yes/g' \
      /etc/ssh/sshd_config
  sudo service sshd restart
SCRIPT

Vagrant.configure(2) do |config|
  config.ssh.forward_agent = true
  config.vm.synced_folder "Turtle", "/vagrant/Turtle"
  config.vm.provision "shell",
        inline: "sudo /vagrant/Turtle/setup/setup.sh"
  config.vm.provision "shell", inline: $VAGRANT_EXTRA_STEPS

  config.ssh.insert_key = false

  config.vm.provider "docker" do |docker, override|
    override.vm.box = nil
    docker.image = "rofrano/vagrant-provider:ubuntu-22.04"
    docker.remains_running = true
    docker.has_ssh = true
    docker.privileged = true
    docker.create_args = ["--cgroupns=host"]
    docker.volumes = ["/sys/fs/cgroup:/sys/fs/cgroup:rw"]
  end

  config.vm.provider "virtualbox" do |v, override|
    override.vm.box = "ubuntu/jammy64"
  end

  config.vm.define :developer do |host|
    host.vm.hostname = "developer"
    host.vm.network "private_network", ip: "10.0.1.1", netmask: 8,
        mac: "08002722471c", virtualbox__intnet: "15441"
    host.vm.provision "shell", inline: $SET_NETWORK
  end
end
