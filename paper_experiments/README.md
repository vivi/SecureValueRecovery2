# SVR3 OSDI 2024 Artifact Evalutation

# Open-source software
1. Server code: https://github.com/signalapp/SecureValueRecovery2/tree/main
2. Client code: https://github.com/privacyresearchgroup/svr3-osdi24-client
3. TLA proof: https://github.com/signalapp/SecureValueRecovery2/blob/main/docs/svr2.tla
4. Artifact evaluation experiment scripts: https://github.com/vivi/SecureValueRecovery2/tree/artifact/paper_experiments

# System setup
Due to the overhead and costs in setting up another staging cluster distributed across multiple enclave types and replica clusters within each enclave type (and making this available to those external to Signal), the machine that the artifact committee has access to is slightly different from what is deployed on staging at Signal (graphs/numbers the paper reports):

- The artifact machine has 3 nitro enclaves and 3 SGX simulation-mode enclaves, all on the same machine. The system has been prefilled with 10M users.
- The paper’s experimental setup hits what is deployed on staging at Signal (3 Nitro enclaves, 3 SGX enclaves, 3 AMD SEV-SNP enclaves, all geographically distributed across different datacenters)

We believe that the artifact machine’s setup, though not identical to the setup reported in the paper, is still sufficient to faithfully reproduce the main results in the paper.

**All binaries are already built on the artifact machine, please do not `rm -rf` anything!**

# Artifact Evaluation

## Artifact server

First, `cd` into the proper directory:

```sh
cd ~/SecureValueRecovery2/paper_experiments
```
### Remote attestation with artifact server

**Remote attestation** lets you verify that the code running on our artifact server hashes to the hash published on the Signal client: https://github.com/signalapp/libsignal/blob/main/rust/attest/src/constants.rs#L32

We use this in lieu of the artifact evaluator having to build from source. The server code is open-sourced at https://github.com/signalapp/SecureValueRecovery2/tree/main.

```sh
./svr3-benchmark-client --password pwd123 --statfile out.csv --enclave-secret AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA= --attest-doc nitro_attest.bin
```

Get the root certificate from AWS:

```sh
wget https://aws-nitro-enclaves.amazonaws.com/AWS_NitroEnclaves_Root-G1.zip

gunzip AWS_NitroEnclaves_Root-G1.zip

rm AWS_NitroEnclaves_Root-G1.zip

mv root.pem nitro_attestation_pki.pem
```

Run the check:

```sh
python3 check_nitro_attestation.py
```

Expected output (the PCRs should match):

```
Certificate chain validated successfully!

Expected PCRs (from https://github.com/signalapp/libsignal/blob/main/rust/attest/src/constants.rs#L32):

PCR 0: 24e56baabe26dcedc58f5753ee979e2f74750df31f43a18f0cf4e08f8ad8c0cd304142cf3441945c3568f4096cb69c66

PCR 1: 52b919754e1643f4027eeee8ec39cc4a2cb931723de0c93ce5cc8d407467dc4302e86490c01c0d755acfe10dbf657546

PCR 2: ec540f3f7f673ab65582d96cf26a747beffcc9392e82f48cfa4ceec47a6ad69a63f9102fc7e1fae37a83a9741814210f

PCRs from attestation document:

PCR 0: 24e56baabe26dcedc58f5753ee979e2f74750df31f43a18f0cf4e08f8ad8c0cd304142cf3441945c3568f4096cb69c66

PCR 1: 52b919754e1643f4027eeee8ec39cc4a2cb931723de0c93ce5cc8d407467dc4302e86490c01c0d755acfe10dbf657546

PCR 2: ec540f3f7f673ab65582d96cf26a747beffcc9392e82f48cfa4ceec47a6ad69a63f9102fc7e1fae37a83a9741814210f
```

## Run benchmark

This script runs the benchmarks and produces plots:

```sh

./bench.sh

```

It will take approx 6 min to run. 

Example output:

```sh

... (output from running benchmark)

+ echo 'Benchmarks are in 2024-04-25_18-51-17'

Benchmarks are in 2024-04-25_18-51-17

+ echo 'Figures are in 2024-04-25_18-51-17/fig'

Figures are in 2024-04-25_18-51-17/fig

```

Copy over the figures from the folder that the output specifies (it’s a timestamp). Open it in a PDF reader and compare it to the reference figures.

## Staging cluster

**We are currently getting internal clearance so that the artifact evaluation committee can evaluate the E2E numbers from the staging cluster. That should be available within the next week.**

## Reference figures

Reference figures are located in:

https://github.com/vivi/SecureValueRecovery2/tree/artifact/paper_experiments/reference_figures

Given the setup of the artifact machine, you can expect the latencies to be much smaller than the ones reported in the paper because all of the enclaves are co-located on the same machine, instead of being geographically distributed. Thus, there is effectively no network latency.