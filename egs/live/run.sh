#!/usr/bin/env bash

BARISTAROOT=../..
export PATH=$BARISTAROOT/bin:$PATH

barista conf/actors.ini conf/graph.dot
