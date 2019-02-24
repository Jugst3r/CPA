set terminal postscript eps enhanced
set title "Distribution de degrée "
set output "tas_union_arbre.eps" 
set ylabel "nombre de noeuds"
set xlabel "degrée"
set style data lines
plot "emails.dat" using 1:2 with lines title ""
