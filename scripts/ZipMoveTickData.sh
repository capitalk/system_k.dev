#!/bin/bash

remotetickdir="/home/timir"
localtickdir="/mnt/raid/tickdata"

# FOR TESTING - use localhost
#remoteserver="127.0.0.1"
remoteserver="172.16.2.101"

remoteuser="timir"
#bindir="/home/timir/capitalk.repo/src/dataCollector"
bindir="/home/timir/collectors/collect-fix"

# Pass MIC names to script to move associated ticks and logs
while [ $# -gt 0 ]
do 
MIC=$1
#ls $localtickdir/$MIC

yesterday="$(date +%Y_%m_%d --date "-1 day")"
echo "Yesterday: $yesterday"

today="$(date +%Y_%m_%d)"
echo "Today: $today"

# FOR TESTING - uncomment so we can use today's data 
#yesterday=$today

# Dispaly some info
ssh $remoteuser@$remoteserver "df -h $remotetickdir/$MIC/$yesterday/"

# Stop the collector
echo "STOPPING collect"
ssh $remoteuser@$remoteserver "cat $bindir/collect.$MIC.pid | xargs kill -15"

# Rename raw message logs while collector stopped
echo "Renaming logs"
ssh $remoteuser@$remoteserver "cd $remotetickdir/$MIC/log && for w in *.log; do mv \$w $yesterday-\$w; done"

# Restart the collector
echo "RESTARTING collect" 
# FOR TESTING - remove the uncomment below and comment the next line
#ssh $remoteuser@$remoteserver "cd $bindir && $bindir/collect.$MIC.dev.sh"
ssh $remoteuser@$remoteserver "cd $bindir && $bindir/collect.$MIC.prod.sh"

# Zip up tick files and copy them over to local
echo "MOVING tick files" 
ssh $remoteuser@$remoteserver "nice -n 1 gzip $remotetickdir/$MIC/$yesterday/*.csv"
ssh $remoteuser@$remoteserver "sh s3_move_dir.sh $remotetickdir/$MIC/$yesterday/ capk-TESTCRON"
scp -rp $remoteuser@$remoteserver:$remotetickdir/$MIC/$yesterday $localtickdir/$MIC/

# Zip up log files and copy them over to local
echo "MOVING log files" 
ssh $remoteuser@$remoteserver "nice -n 1 gzip $remotetickdir/$MIC/log/$yesterday-*.log"
mkdir -p $localtickdir/$MIC/$yesterday/log
scp $remoteuser@$remoteserver:$remotetickdir/$MIC/log/$yesterday-*.log.gz $localtickdir/$MIC/$yesterday/log

# Delete log files that are ZIPPED ONLY - collector will be running again at this point
echo "DELETE $remotetickdir/$MIC/log/*.log.gz"
ssh $remoteuser@$remoteserver "rm -rf $remotetickdir/$MIC/log/*.log.gz"

# Delete yesterdays ticks
echo "DELETE $remotetickdir/$MIC/$yesterday"
ssh $remoteuser@$remoteserver "rm -rf $remotetickdir/$MIC/$yesterday"

# Dispaly some info
echo "SPACE REPORT AFTER MOVE AND DELETE"
ssh $remoteuser@$remoteserver "df -h $remotetickdir"

shift
done

