#!/bin/sh
set -x
# WARNING: this script doesn't check for errors, so you have to enhance it in case any of the commands
# below fail.
rmmod sys_vector1
rmmod sys_vector2
rmmod sys_vector3
rmmod vec_ioctl
lsmod
