#!/usr/bin/env bash

# We only do one decoding pass, so there is no point caching the
# CMVN stats-- we make them part of a pipe.
# feats="ark:compute-cmvn-stats --spk2utt=ark:$mydata/spk2utt scp:$mydata/feats.scp ark:- | apply-cmvn --norm-vars=false --utt2spk=ark:$mydata/utt2spk ark:- scp:$mydata/feats.scp ark:- | add-deltas ark:- ark:- |"
# 
# gmm-latgen-faster --max-active=7000 --beam=13.0 --lattice-beam=6.0 --acoustic-scale=0.083333 \
#   --allow-partial=true --word-symbol-table=$graphdir/words.txt \
#   $srcdir/final.mdl $graphdir/HCLG.fst "$feats" "ark:|gzip -c > $dir/lat.$jobid.gz" \
#      2> $dir/decode$jobid.log || exit 1;

KALDIROOT=../../..
export PATH=$KALDIROOT/src/featbin:$KALDIROOT/src/gmmbin:$KALDIROOT/tools/sph2pipe_v2.5:$PATH

modeldir=../model
confdir=../conf
outdir=.

compute-mfcc-feats  --verbose=2 --config=$confdir/mfcc.conf scp:test.scp ark,scp:$outdir/feats.ark,$outdir/feats.scp

feats="ark:compute-cmvn-stats scp:$outdir/feats.scp ark:- | apply-cmvn --norm-vars=false ark:- scp:$outdir/feats.scp ark:- | add-deltas ark:- ark:- |"

gmm-decode-faster --max-active=7000 --beam=13.0 --acoustic-scale=0.083333\
  --allow-partial=true --word-symbol-table=$modeldir/words.txt \
  $modeldir/final.mdl $modeldir/HCLG.fst "$feats" ark,t:$outdir/test.tra \
     2> $outdir/decode.log || exit 1;

# gmm-decode-faster-online --max-active=7000 --beam=13.0 --acoustic-scale=0.083333 \
#   --allow-partial=true --word-symbol-table=$modeldir/words.txt \
#   $modeldir/final.mdl $modeldir/HCLG.fst "$feats" ark,t:$outdir/test-online.tra \
#      2> $outdir/test-online.log || exit 1;

