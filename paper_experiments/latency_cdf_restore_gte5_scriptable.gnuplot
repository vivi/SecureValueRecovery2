set terminal pdf size 1.1,1.1
# set terminal pdf size 1.6,1.3
set terminal pdfcairo font "Times New Roman,8" fontscale 0.75
set termopt enhanced

#set lmargin at screen 0.13
#set tmargin at screen 0.97
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
set xlabel "Latency (ms)"
#set xtics 20
set yrange[-0.01:1.01]
# set xrange[1:500]
set xrange[1:2500]
#set xtics 400
# set xtics 50
# set xtics 100
# set xtics 100
#set xrange[:40]
#set format y "%.0s %c"
set tics font ", 7"

set output outputname
#"../fig/latency_cdf_nitro_10m_restore_create_parallel.pdf"
set logscale x
set tics front
set format x "10^{%L}"

plot    \
    NaN title "{/:Bold Restore}" lt -3, \
    folder."/cdf_p5_restore.csv" using 1:2 skip 1 title "5" with line lw 1          lc 3, \
    folder."/cdf_p10_restore.csv" using 1:2 skip 1 title "10" with line lw 1        lc 4, \
    folder."/cdf_p20_restore.csv" using 1:2 skip 1 title "20" with line lw 1        lc 5, \
    folder."/cdf_p50_restore.csv" using 1:2 skip 1 title "50" with line lw 1        lc 6, \
    folder."/cdf_p100_restore.csv" using 1:2 skip 1 title "100" with line lw 1     lc 7, \
    NaN title "{/:Bold Create}" lt -3, \
    folder."/cdf_p5_create.csv" using 1:2 skip 1 title "5" with line lw 1             lc 3 dashtype (3, 2.5), \
    folder."/cdf_p10_create.csv" using 1:2 skip 1 title "10" with line lw 1           lc 4 dashtype (3, 2.5), \
    folder."/cdf_p20_create.csv" using 1:2 skip 1 title "20" with line lw 1           lc 5 dashtype (3, 2.5), \
    folder."/cdf_p50_create.csv" using 1:2 skip 1 title "50" with line lw 1           lc 6 dashtype (3, 2.5), \
    folder."/cdf_p100_create.csv" using 1:2 skip 1 title "100" with line lw 1        lc 7 dashtype (3, 2.5), \
