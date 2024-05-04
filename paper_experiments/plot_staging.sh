#!/bin/bash
set -x

FOLDER_NAME=$1
mkdir -p $FOLDER_NAME/fig

gnuplot -e "folder='$FOLDER_NAME/e2e' ; outputname='$FOLDER_NAME/fig/fig12a.pdf'" latency_cdf_e2e_staging.gnuplot
gnuplot -e "folder='$FOLDER_NAME/e2e' ; outputname='$FOLDER_NAME/fig/fig12b.pdf'" latency_breakdown_staging.gnuplot
