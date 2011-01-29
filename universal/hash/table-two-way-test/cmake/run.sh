#!/bin/bash

ulimit -S -t unlimited
./TableTwoWayTest --appendix=`hostname` $@
