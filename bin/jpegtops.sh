#!/bin/sh

cat $1 | jpegtopnm | pnmtops > ${1}.ps
