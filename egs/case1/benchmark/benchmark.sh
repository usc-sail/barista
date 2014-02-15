#!/usr/bin/env bash

BARISTAROOT=../../..
KALDISRC=$BARISTAROOT/tools/kaldi/src
export PATH=$BARISTAROOT/build:$KALDISRC/bin:$PATH

modeldir=../../models/wsj

barista conf/actors.ini conf/graph.dot

perl int2sym.pl --ignore-first-field $modeldir/words.txt test.tra > test.hyp

compute-wer --text --mode=present ark:test.ref ark:test.hyp > test.wer
