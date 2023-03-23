#!/usr/bin/env bash

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

sudo apt-get update

sudo DEBIAN_FRONTEND=noninteractive apt-get install -y build-essential cmake libmysqlcppconn-dev vim \
    emacs tree tmux git gdb valgrind python3-dev libffi-dev libssl-dev mysql-server \
    clang-format clang-tidy iperf3 tshark iproute2 iputils-ping net-tools tcpdump cppcheck python-is-python3 \
    cloc siege libboost-all-dev curl llvm

sudo DEBIAN_FRONTEND=noninteractive apt-get install -y python3 python3-pip \
    python-tk libpython3.10-dev libcairo2 libcairo2-dev
