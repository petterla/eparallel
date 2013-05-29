#!/bin/bash


protoc --cpp_out=. -I=../proto ../proto/xylx.proto

cmake .
gmake clean;gmake
