#!/usr/bin/env bash

KALDISRC=../../../tools/kaldi/src
export PATH=$KALDISRC/bin:$KALDISRC/featbin:$KALDISRC/gmmbin:$PATH

modeldir=../../models/wsj

mkfifo feats

feats="ark:compute-mfcc-feats --config=mfcc.conf scp:test.scp ark:- | \
  tee feats | compute-cmvn-stats ark:- ark:- | \
  apply-cmvn --norm-vars=false ark:- ark:feats ark:- | \
  add-deltas ark:- ark:- |"

gmm-decode-faster --max-active=7000 --beam=13.0 --acoustic-scale=0.083333 \
  --allow-partial=true --word-symbol-table=$modeldir/words.txt \
  $modeldir/final.mdl $modeldir/HCLG.fst "$feats" ark,t:test.tra || exit 1;

rm feats

perl int2sym.pl --ignore-first-field $modeldir/words.txt test.tra > test.hyp

compute-wer --text --mode=present ark:test.ref ark:test.hyp > test.wer
