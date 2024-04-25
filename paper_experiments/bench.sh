#!/bin/bash
set -x
set -e

FOLDER_NAME=$(date +"%Y-%m-%d_%H-%M-%S")

mkdir "$FOLDER_NAME"
echo "Created folder $FOLDER_NAME"

mkdir -p "$FOLDER_NAME/sgx"
mkdir -p "$FOLDER_NAME/nitro"

for thread in 5 10 20 50 100
do
./svr3client loadtest \
    --authKey AAAAAAAAAAAAAAAAAAAAAA== \
    --count 10000 \
    --enclaveId artifact \
    --filename "${FOLDER_NAME}/nitro/p${thread}follower10M.csv" \
    --host 127.0.0.1:8000,127.0.0.1:8010,127.0.0.1:8020 \
    --parallel ${thread} \
    --useTLS=false 2>&1 \
    | grep running

./svr3client loadtest \
    --authKey AAAAAAAAAAAAAAAAAAAAAA== \
    --count 10000 \
    --enclaveId artifact \
    --filename "${FOLDER_NAME}/nitro/p${thread}leader10M.csv" \
    --host 127.0.0.1:8000 \
    --parallel ${thread} \
    --useTLS=false 2>&1 \
    | grep running

./svr3client loadtest \
    --authKey AAAAAAAAAAAAAAAAAAAAAA== \
    --count 10000 \
    --enclaveId artifact \
    --filename "${FOLDER_NAME}/sgx/p${thread}follower10M.csv" \
    --host 127.0.0.1:8100,127.0.0.1:8110,127.0.0.1:8120 \
    --parallel ${thread} \
    --useTLS=false 2>&1 \
    | grep running

./svr3client loadtest \
    --authKey AAAAAAAAAAAAAAAAAAAAAA== \
    --count 10000 \
    --enclaveId artifact \
    --filename "${FOLDER_NAME}/sgx/p${thread}leader10M.csv" \
    --host 127.0.0.1:8100 \
    --parallel ${thread} \
    --useTLS=false 2>&1 \
    | grep running
done

mkdir -p $FOLDER_NAME/e2e

./svr3-benchmark-client --password pwd123 --statfile "${FOLDER_NAME}/e2e/e2e_latency_breakdown.csv" --enclave-secret AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA=

python3 process.py $FOLDER_NAME/sgx leader
python3 process.py $FOLDER_NAME/sgx follower
python3 process.py $FOLDER_NAME/nitro leader
python3 process.py $FOLDER_NAME/nitro follower
python3 process_e2e.py $FOLDER_NAME/e2e

./plot.sh $FOLDER_NAME

echo "Benchmarks are in $FOLDER_NAME"
echo "Figures are in $FOLDER_NAME/fig"
