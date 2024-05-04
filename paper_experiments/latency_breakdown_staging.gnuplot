set terminal pdf size 1.6,1.3
set terminal pdfcairo font "Times New Roman,8" fontscale 0.75
set termopt enhanced

set lmargin 5.5
set rmargin 2
set rmargin 2.2
set tmargin 0.5

set key spacing 1.1
set key samplen 1
set key center right
# set key vertical maxrows 2
# set key off
set datafile separator ','
set ylabel "Percentage" offset 2
set xlabel "Samples" offset 0,1.5
# set xlabel "Latency (ms)"
#set xtics 20
set yrange[-1:101]
#set xtics 400
# set xtics 5
unset xtics
set xtics format " "
set xtics scale 0
#set xrange[:40]
#set format y "%.0s %c"
set tics font ", 7"

set output outputname
set key opaque

plot    \
    folder."/percent_breakdown.csv" using 1:7:(0) skip 1 title "Create shares" with filledcurves lw 1          lc 5, \
    folder."/percent_breakdown.csv" using 1:5:(0) skip 1 title "Finalize OPRFs" with filledcurves lw 1          lc 4, \
    folder."/percent_breakdown.csv" using 1:4:(0) skip 1 title "Call servers" with filledcurves lw 1          lc 3, \
    folder."/percent_breakdown.csv" using 1:3:(0) skip 1 title "Prepare OPRFs" with filledcurves lw 1          lc 2, \
    folder."/percent_breakdown.csv" using 1:2:(0) skip 1 title "Attestation" with filledcurves lw 1          lc 1, \
    # folder."/cdf_ssp1_create.csv" using 1:2 skip 1 title "Create" with line lw 1             lc 1 dashtype (3, 2.5), \
