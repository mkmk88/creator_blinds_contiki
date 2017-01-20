#!/bin/bash

git submodule update --init --recursive

rc=$?
if [ $rc = 0 ]
then
    cd LetMeCreateIoT && ./install.sh -p contiki
fi
