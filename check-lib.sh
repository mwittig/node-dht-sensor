#!/bin/sh
LIBNAME=wiringPi

gcc -l$LIBNAME 2>&1 | grep -q "cannot find -l$LIBNAME"
if [ $? -eq 0 ] ; then
   echo "+---------------------------------------------------------------------------------+"
   echo -e "| \033[1;31mERROR: Library wiringPi not found!\033[0m                             |"
   echo "| Make sure to follow instructions from http://wiringpi.com/download-and-install/ |"
   echo "| to install the wiringPi library on your system first then install this addon.   |"
   echo "+---------------------------------------------------------------------------------+"
   exit 0
else
   echo "Library $LIBNAME found."
fi
