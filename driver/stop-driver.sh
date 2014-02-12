#!/bin/bash

# unload SKIN
rmmod skin_test_device
rmmod skin_kernel

# unload RTAI
rmmod rtai_shm
rmmod rtai_netrpc
rmmod rtai_msg
rmmod rtai_mbx
rmmod rtai_sem
rmmod rtai_fifos
rmmod rtai_sched
rmmod rtai_hal

