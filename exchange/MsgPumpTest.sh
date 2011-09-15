#!/bin/bash

for (( i=0 ; i<2000; i++)) 
do
    ./MsgPumpTest --i fixtest.20.log
    ./MsgPumpTest --i fixtest.20.log 
done

