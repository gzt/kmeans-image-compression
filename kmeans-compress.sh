#!/usr/bin/bash
if ./block-tiff "$@"; then
    zpaq add block-output.kmns block-means.out block-mean-means.out block-mean-id.out block-class.out -m5
else echo 'No output produced'
fi
