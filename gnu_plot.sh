#!/usr/bin/env gnuplot

set terminal pngcairo size 450,350 enhanced font 'Verdana,10'

# define axis
# remove border on top and right and set color to gray
set style line 11 lc rgb '#808080' lt 1
set style line 12 lc rgb '#808080' lt 0 lw 1
set border 3 back ls 11
set tics nomirror
# define grid

set grid back ls 12

# colors
set style line 1 lc rgb '#8b1a0e' pt 5 ps 1 lt 1 lw 2 # --- red
set style line 2 lc rgb '#5e9c36' pt 4 ps 1 lt 1 lw 2 # --- green
set style line 3 lc rgb '#1e1e1e' pt 6 ps 1 lt 1 lw 2 # --- blackish
set style line 4 lc rgb '#1e4eae' pt 7 ps 1 lt 1 lw 2 # --- blue

set xlabel 'block size'
set ylabel 'memory (KB)'
set key center left
set output 'data/memory_plot_small.png'

#
# small
#

# memory plot
plot 'data/firstfit_small.dat' u (log($1)):2:xtic(1) t 'First fit' w lp ls 1, \
     'data/bestfit_small.dat' u (log($1)):2:xtic(1) t 'Best fit' w lp ls 2, \
     'data/worstfit_small.dat' u (log($1)):2:xtic(1) t 'Worst fit' w lp ls 4
     #'data/system_malloc_small.dat' u (log($1)):2:xtic(1) t 'System' w lp ls 3, \

show output

set ylabel 'milliseconds'
set key top right
set output 'data/time_plot_small.png'

# time plot
plot 'data/firstfit_small.dat' u (log($1)):3:xtic(1) t 'First fit' w lp ls 1, \
     'data/bestfit_small.dat' u (log($1)):3:xtic(1) t 'Best fit' w lp ls 2, \
     'data/system_malloc_small.dat' u (log($1)):3:xtic(1) t 'System' w lp ls 3, \
     'data/worstfit_small.dat' u (log($1)):3:xtic(1) t 'Worst fit' w lp ls 4, \

show output



#
# big
#

set ylabel 'memory (KB)'
set key top left
set output 'data/memory_plot_big.png'

# memory plot
plot 'data/firstfit_big.dat' u (log($1)):2:xtic(1) t 'First fit' w lp ls 1, \
     'data/bestfit_big.dat' u (log($1)):2:xtic(1) t 'Best fit' w lp ls 2, \
     'data/worstfit_big.dat' u (log($1)):2:xtic(1) t 'Worst fit' w lp ls 4
     #'data/system_malloc_big.dat' u (log($1)):2:xtic(1) t 'System' w lp ls 3, \

show output

set ylabel 'milliseconds'
set output 'data/time_plot_big.png'

# time plot
plot 'data/firstfit_big.dat' u (log($1)):3:xtic(1) t 'First fit' w lp ls 1, \
     'data/bestfit_big.dat' u (log($1)):3:xtic(1) t 'Best fit' w lp ls 2, \
     'data/system_malloc_big.dat' u (log($1)):3:xtic(1) t 'System' w lp ls 3, \
     'data/worstfit_big.dat' u (log($1)):3:xtic(1) t 'Worst fit' w lp ls 4, \

show output