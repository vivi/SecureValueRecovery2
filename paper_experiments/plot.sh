#!/bin/bash
set -x

FOLDER_NAME=$1
mkdir -p $FOLDER_NAME/fig

gnuplot -e "folder='$FOLDER_NAME' ; outputname='$FOLDER_NAME/fig/5.pdf'" latency_throughput.gnuplot

gnuplot -e "folder='$FOLDER_NAME/nitro/leader' ; outputname='$FOLDER_NAME/fig/6a.pdf'" latency_cdf_restore_gte5_scriptable.gnuplot
gnuplot -e "folder='$FOLDER_NAME/nitro/follower' ; outputname='$FOLDER_NAME/fig/6b.pdf'" latency_cdf_restore_gte5_scriptable.gnuplot

gnuplot -e "folder='$FOLDER_NAME/sgx/leader' ; outputname='$FOLDER_NAME/fig/7a.pdf'" latency_cdf_restore_gte5_scriptable.gnuplot
gnuplot -e "folder='$FOLDER_NAME/sgx/follower' ; outputname='$FOLDER_NAME/fig/7b.pdf'" latency_cdf_restore_gte5_scriptable.gnuplot

gnuplot -e "folder='$FOLDER_NAME/e2e' ; outputname='$FOLDER_NAME/fig/fig12a.pdf'" latency_cdf_e2e.gnuplot
gnuplot -e "folder='$FOLDER_NAME/e2e' ; outputname='$FOLDER_NAME/fig/fig12b.pdf'" latency_breakdown.gnuplot
