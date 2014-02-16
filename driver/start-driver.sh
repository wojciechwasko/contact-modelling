#!/bin/bash

# NOTE directories in the following lists, if relative, have to be relative to
# the build directory.
rtai_base="/opt/realtime/modules/"
driver_dir="driver/driver-build/"
driver_name="skin_test_device"
calib_cache="data-copy/calibration.cache"
regio_cache="data-copy/regionalisation.cache"

# implementation below
driver_file=$driver_dir$driver_name.ko
doexit=0

if [ ! -e $driver_file ]; then
  echo "Couldn't find the kernel module file at '$driver_file'" >&2
  doexit=1
fi

if [ ! -e $calib_cache ]; then
  echo "Couldn't find the calibration cache file at '$calib_cache'" >&2
  doexit=1
fi

if [ ! -e $regio_cache ]; then
  echo "Couldn't find the regionalisation cache file at '$regio_cache'" >&2
  doexit=1
fi

if [ $doexit -ne 0 ]; then
  echo "Exiting due to errors..."
  exit 1
fi

insmod "${rtai_base}rtai_hal.ko"
insmod "${rtai_base}rtai_sched.ko"
insmod "${rtai_base}rtai_fifos.ko"
insmod "${rtai_base}rtai_sem.ko"
insmod "${rtai_base}rtai_mbx.ko"
insmod "${rtai_base}rtai_msg.ko"
insmod "${rtai_base}rtai_netrpc.ko"
insmod "${rtai_base}rtai_shm.ko"


insmod `skin-config --module-dir`/skin_kernel.ko
insmod $driver_file
while [ ! -e /sys/skin_kernel/registered_devices ]; do
  sleep 0.2
done
while ! grep $driver_name /sys/skin_kernel/registered_devices > /dev/null 2>&1; do
  sleep 0.2
done
`skin-config --device-finalizer`
`skin-config --calibrator` $calib_cache
`skin-config --regionalizer` $regio_cache

