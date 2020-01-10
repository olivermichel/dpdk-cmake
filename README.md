
## DPDK Example Applications

Basic DPDK example applications for Linux using the cmake build system.

### Compile Example Applications

    mkdir build && cd build
    cmake -DCMAKE_BUILD_TYPE=Release ..
    make -j$(nproc)

### Enable DPDK on an interface

    sudo su

    export RTE_SDK=/home/ec2-user/opt/dpdk-19.11
    export RTE_TARGET=x86_64-native-linuxapp-gcc

    ./setup.sh
    ./bind.sh eth1

    exit

### Usage example

    ./tx -- -m 02:3a:a4:9a:3d:a4 -s 172.19.20.11 -d 172.19.20.12