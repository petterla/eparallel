#!/bin/bash


#protoc --cpp_out=. -I=../proto ../proto/xylx.proto
protoc --cpp_out=. -I=. xylx.proto

cmake .
gmake clean;gmake
