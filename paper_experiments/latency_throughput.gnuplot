set terminal pdf size 3.2,1.8
set terminal pdfcairo font "Times New Roman,8" fontscale 0.75
set termopt enhanced

set lmargin 9.5
set rmargin 2
set rmargin 2.5
set tmargin .5

set key spacing 1.1
set key samplen 1
# set key top
# set key vertical maxrows 5

# set key outside right
set datafile separator ','
set ylabel "Throughput (req/s)"
set xlabel "Average latency (ms)"
#set xtics 20
#set yrange[-0.01:1.01]
set yrange[0:4000]
#set xrange[:40]
#set format y "%.0s %c"

set output outputname

plot    \
    NaN title "{/:Bold Recover}" lt -3, \
    folder."/sgx/latency_v_throughput_processed.csv" using ($2/1000):3 skip 2 title "SGX 10M" with linespoints lw 1          ps .7 lc 1 pt 4, \
    folder."/nitro/latency_v_throughput_processed.csv" using ($2/1000):3 skip 2 title "Nitro 10M" with linespoints lw 1      ps .7 lc 2 pt 2, \
    NaN title "{/:Bold Store}" lt -3, \
    folder."/sgx/latency_v_throughput_processed.csv" using ($4/1000):5 skip 2 title "SGX 10M" with linespoints lw 1          ps .7 lc 1 pt 4 dt (3, 2.5), \
    folder."/nitro/latency_v_throughput_processed.csv" using ($4/1000):5 skip 2 title "Nitro 10M" with linespoints lw 1      ps .7 lc 2 pt 2 dt (3, 2.5), \
    #folder."/cdf_p5_create.csv" using 1:2 skip 1 title "5" with line lw 1             lc 3 dashtype (3, 2.5), \
    #folder."/cdf_p10_create.csv" using 1:2 skip 1 title "10" with line lw 1           lc 4 dashtype (3, 2.5), \
    #folder."/cdf_p20_create.csv" using 1:2 skip 1 title "20" with line lw 1           lc 5 dashtype (3, 2.5), \
    #folder."/cdf_p50_create.csv" using 1:2 skip 1 title "50" with line lw 1           lc 6 dashtype (3, 2.5), \
    #folder."/cdf_p100_create.csv" using 1:2 skip 1 title "100" with line lw 1        lc 7 dashtype (3, 2.5), \
