#!/usr/bin/bash
if ./block-tiff "$@"; then
    while getopts "vi:o:k:p:n:r:l:L:m:s:S:t" opt; do
	case ${opt} in
	    o )
		TARGET="${OPTARG%.*}"
		echo "$TARGET"
		;;
	    \? ) echo " "
		;;
	esac
    done
    zpaq add "$TARGET".kmns block-means.out block-mean-means.out block-mean-id.out block-class.out -m5
else echo 'No output produced'
fi
