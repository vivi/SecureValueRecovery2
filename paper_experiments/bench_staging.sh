#!/bin/bash
set -x
set -e

FOLDER_NAME=$(date +"%Y-%m-%d_%H-%M-%S_staging")

mkdir "$FOLDER_NAME"
echo "Created folder $FOLDER_NAME"

mkdir -p $FOLDER_NAME/e2e

./svr3-staging-client --password pwd123 --statfile staging.csv --authpwd osdi24artifact

python3 process_e2e.py $FOLDER_NAME/e2e

./plot_staging.sh $FOLDER_NAME

echo "Benchmarks are in `pwd`/$FOLDER_NAME"
echo "Figures are in `pwd`/$FOLDER_NAME/fig"
