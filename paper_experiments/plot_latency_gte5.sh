#!/bin/bash
set -x
gnuplot -e "folder='2024-04-24_03-26-32/sgx/leader' ; outputname='fig/latency_cdf_sgx_10m_restore_create_leader.pdf'" latency_cdf_restore_gte5_scriptable.gnuplot
gnuplot -e "folder='2024-04-24_03-26-32/sgx/follower' ; outputname='fig/latency_cdf_sgx_10m_restore_create_follower.pdf'" latency_cdf_restore_gte5_scriptable.gnuplot

gnuplot -e "folder='2024-04-24_03-26-32/nitro/leader' ; outputname='fig/latency_cdf_nitro_10m_restore_create_leader.pdf'" latency_cdf_restore_gte5_scriptable.gnuplot
gnuplot -e "folder='2024-04-24_03-26-32/nitro/follower' ; outputname='fig/latency_cdf_nitro_10m_restore_create_follower.pdf'" latency_cdf_restore_gte5_scriptable.gnuplot