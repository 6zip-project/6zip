#!/usr/bin/env bash
# Copyright (c) 2018-2023 The Dash Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
# use testnet settings,  if you need mainnet,  use ~/.zipcore/zipd.pid file instead
export LC_ALL=C

zip_pid=$(<~/.zipcore/testnet3/zipd.pid)
sudo gdb -batch -ex "source debug.gdb" zipd ${zip_pid}
