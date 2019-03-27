set terminal postscript eps enhanced
set output 'Histogramme2_q_1.eps'
set style histogram                      
set style data histograms
set xtic 0,50000
set style histogram cluster gap 1
plot 'histogramme2_q_1.txt' using 2:xtic(50000)
