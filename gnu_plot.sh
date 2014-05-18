#!/usr/bin/env gnuplot
set terminal png
set output "plot.png"
set style data lines
plot "first_fit.dat", "best_fit.dat"
show output
