#!/bin/bash
# Install dependencies and configure the environment for CI build testing

set -ex

PKGS="build-essential gawk ccache genromfs libc6-i386 \
      python-argparse python-empy python-serial python-pexpect python-dev python-pip zlib1g-dev gcc-4.9 g++-4.9 cmake cmake-data clang-3.7"

ARM_ROOT="gcc-arm-none-eabi-4_9-2015q3"
ARM_TARBALL="$ARM_ROOT-20150921-linux.tar.bz2"

RPI_ROOT="master"
RPI_TARBALL="$RPI_ROOT.tar.gz"

read -r UBUNTU_CODENAME <<<$(lsb_release -c -s)

sudo add-apt-repository ppa:ubuntu-toolchain-r/test -y

if [ "$UBUNTU_CODENAME" = "precise" ]; then
    sudo add-apt-repository ppa:george-edison55/precise-backports -y
elif [ "$UBUNTU_CODENAME" = "trusty" ]; then
    sudo add-apt-repository ppa:george-edison55/cmake-3.x -y
fi

wget -q -O - http://llvm.org/apt/llvm-snapshot.gpg.key | sudo apt-key add -
sudo add-apt-repository "deb http://llvm.org/apt/${UBUNTU_CODENAME}/ llvm-toolchain-${UBUNTU_CODENAME}-3.7 main" -y
sudo apt-get -qq -y update
sudo apt-get -qq -y remove clang llvm
sudo apt-get -y install $PKGS
sudo update-alternatives --install /usr/bin/clang clang /usr/bin/clang-3.7 37 \
    --slave /usr/bin/clang++ clang++ /usr/bin/clang++-3.7
sudo pip install mavproxy
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-4.9 90 \
    --slave /usr/bin/g++ g++ /usr/bin/g++-4.9

pushd $HOME

mkdir -p $HOME/opt
pushd $HOME/opt

# PX4 toolchain
compiler=$ARM_ROOT
if [ ! -d "$HOME/opt/$compiler" ]; then
  wget http://firmware.ardupilot.org/Tools/PX4-tools/$ARM_TARBALL
  tar -xf $ARM_TARBALL
fi

# RPi/BBB toolchain
compiler="tools-master/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian-x64"
if [ ! -d "$HOME/opt/$compiler" ]; then
  wget http://firmware.ardupilot.org/Tools/Travis/NavIO/$RPI_TARBALL
  tar -xf $RPI_TARBALL
fi

popd

mkdir -p $HOME/bin

# configure ccache
ln -s /usr/bin/ccache ~/bin/g++
ln -s /usr/bin/ccache ~/bin/gcc
ln -s /usr/bin/ccache ~/bin/arm-none-eabi-g++
ln -s /usr/bin/ccache ~/bin/arm-none-eabi-gcc
ln -s /usr/bin/ccache ~/bin/arm-linux-gnueabihf-g++
ln -s /usr/bin/ccache ~/bin/arm-linux-gnueabihf-gcc

exportline="export PATH=$HOME/bin"
exportline="${exportline}:$HOME/opt/gcc-arm-none-eabi-4_9-2015q3/bin"
exportline="${exportline}:$HOME/opt/tools-master/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian-x64/bin"
exportline="${exportline}:\$PATH"

if grep -Fxq "$exportline" ~/.profile; then
    echo nothing to do;
else
    echo $exportline >> ~/.profile;
fi

. ~/.profile

popd

