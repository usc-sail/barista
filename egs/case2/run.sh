#!/usr/bin/env bash

BARISTAROOT=../..
export PATH=$BARISTAROOT/bin:$PATH

wsjdir=../models/wsj
hub4dir=../models/hub4
icsidir=../models/icsi

barista conf/actors.ini conf/graph.dot &> log

perl int2sym.pl --ignore-first-field $wsjdir/words.txt wsj.tra > wsj.hyp
perl int2sym.pl --ignore-first-field $hub4dir/words.txt hub4.tra > hub4.hyp
perl int2sym.pl --ignore-first-field $icsidir/words.txt icsi.tra > icsi.hyp
