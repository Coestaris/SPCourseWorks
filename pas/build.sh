#!/bin/bash

OUTDIR="bin"
FILE_LIST="Main.pas"
OUT_FILE="Main"

clear=0
build=1
run=1

if [ $# -eq 1 ]
then
    if [ $1 == "clear" ]
    then
        clear=1
        build=0
        run=0
    elif [ $1 == "build" ]
    then
        clear=0
        build=1
        run=0
    else
        echo "Wrong input command. Try \"clear\" or \"build\"" >&2
        exit 1
    fi
else
    clear=0
    build=1
    run=1
fi

if [ ${clear} -eq 1 ]
then
    echo "===================================="
    echo "Clearing project directory ${OUTDIR}"
    echo "===================================="
    rm -rf ${OUTDIR}
fi

if [ ${build} -eq 1 ]
then
    echo "===================================="
    echo "Building project"
    echo "===================================="
    mkdir -p ${OUTDIR}
    fpc ${FILE_LIST} -FU${OUTDIR}/ -o${OUTDIR}/ -Sd
fi

if [ $? -eq 0 ]
then
  if [ ${run} -eq 1 ]
  then 
    echo "===================================="
    echo "Running project"
    echo "===================================="
    exec ./${OUTDIR}/${OUT_FILE}
  fi
else
  echo "Compile error" >&2
fi