#!/usr/bin/env gnuplot
set terminal png
set output "plot.png"
set style data lines
set style line 1 linecolor rgb '#0060ad'  linetype 1 linewidth 2
set style line 2 linecolor rgb '#dd181f' linetype 1 linewidth 2
plot "first_fit.dat" linestyle 1 smooth bezier, "best_fit.dat" linestyle 2 smooth bezier
show output