#!/bin/bash

rm -fr CMakeCache.txt
rm -fr CMakeFiles

cmake .
gmake clean;gmake
