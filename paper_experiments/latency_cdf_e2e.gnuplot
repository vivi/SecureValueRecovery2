set terminal pdf size 1.6,1.3
set terminal pdfcairo font "Times New Roman,8" fontscale 0.75
set termopt enhanced

set lmargin 3.5
set rmargin 2
set rmargin 2.2
set tmargin 0.5

set key spacing 1.1
set key bottom
set key samplen 1
#set key vertical maxrows 1
set key off
set datafile separator ','
#set ylabel "CDF"
set xlabel "E2E Latency (ms)"
#set xtics 20
set yrange[-0.01:1.01]
#set xrange[50:2200]
# set xtics 400
# set xtics 5
#set xrange[:40]
#set format y "%.0s %c"
set tics font ", 7"

set output outputname
set logscale x
set tics front
set format x "10^{%L}"

plot    \
    folder."/cdf_total.csv" using 1:2 skip 1 title "Restore" with line lw 2          lc 2,
