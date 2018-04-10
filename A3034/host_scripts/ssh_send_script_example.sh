#!/bin/bash
# Exammple of using ssh to call a script
# In this example, "3" is an argument that is passed to stream_1232. It specifies the exposure compensation
ssh -i /Users/collins/OSI/VBT/A3034/ssh_keys/id_rsa pi@10.0.0.234 'bash -s 3' < /Users/collins/OSI/VBT/A3034/scripts/stream_1232.sh

