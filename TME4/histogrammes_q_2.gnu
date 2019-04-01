set terminal postscript eps enhanced
set output 'Histogramme_q_2.eps'
set style histogram                      
set style data histograms
set xtic 0,100
set style histogram cluster gap 1
plot 'histogrammes.txt' using 2:xtic(100)
