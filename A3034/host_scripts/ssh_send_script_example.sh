#!/bin/bash
# Exammple of using ssh to call a script
ssh -i ../ssh_keys/id_rsa pi@172.16.1.24 'bash -s 3 1' < ../scripts/stream_1232.sh
