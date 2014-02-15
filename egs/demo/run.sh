#!/usr/bin/env bash

BARISTAROOT=../..
export PATH=$BARISTAROOT/build:$PATH

barista conf/actors.ini conf/graph.dot &> log
