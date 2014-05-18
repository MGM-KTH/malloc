#!/usr/bin/env gnuplot

set terminal pngcairo size 450,350 enhanced font 'Verdana,10'

# define axis
# remove border on top and right and set color to gray
set style line 11 lc rgb '#808080' lt 1
set border 3 back ls 11
set tics nomirror
set key top left
# define grid
set style line 12 lc rgb '#808080' lt 0 lw 1
set grid back ls 12

# colors
set style line 1 lc rgb '#8b1a0e' pt 1 ps 1 lt 1 lw 2 # --- red
set style line 2 lc rgb '#5e9c36' pt 6 ps 1 lt 1 lw 2 # --- green

set xlabel 'block size'
set ylabel 'memory (KB)'

set output 'memory_plot.png'

# memory plot
plot 'first_fit.dat' u 1:2 t 'First fit' w lp ls 1, \
     'best_fit.dat' u 1:2 t 'Best fit' w lp ls 2
# plot "first_fit.dat" linestyle 1 , "best_fit.dat" linestyle 2 
show output

set ylabel 'milliseconds'
set output 'time_plot.png'

# time plot
plot 'first_fit.dat' u 1:3 t 'First fit' w lp ls 1, \
     'best_fit.dat' u 1:3 t 'Best fit' w lp ls 2

show output