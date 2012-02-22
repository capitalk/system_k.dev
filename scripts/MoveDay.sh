#!/bin/bash

# Move a single day of data to local disk

remotetickdir="/home/timir/"
localtickdir="/mnt/raid/tickdata"

MIC=$1
DATE=$2
if [ -n $DATE ] 
then 
    yesterday=$2
    echo "Using yesterday as: $yesterday"
else
    yesterday="$(date +%Y_%m_%d --date "-1 day")"
    echo "Using yesterday as: $yesterday"
fi

#today="$(date +%Y_%m_%d)"
#echo "Today: $today"

ssh timir@172.16.2.101 "ls -alh $remotetickdir/$MIC/$yesterday/"
ssh timir@172.16.2.101 "gzip $remotetickdir/$MIC/$yesterday/*.csv"
scp -rp timir@172.16.2.101:$remotetickdir/$MIC/$yesterday $localtickdir/$MIC/
