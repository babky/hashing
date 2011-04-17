#!/bin/bash

ulimit -S -t unlimited
nice -n 6 ./TableTwoWayTest --appendix=`hostname` $@
