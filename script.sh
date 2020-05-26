make clean
make
./simu 20

R CMD BATCH Draw.R
mv Rplots.pdf k20.pdf

R CMD BATCH Station1.R
mv Rplots.pdf k20s1.pdf

R CMD BATCH Station10.R
mv Rplots.pdf k20s10.pdf

make clean
make
./simu 22

R CMD BATCH Draw.R
mv Rplots.pdf k22.pdf

R CMD BATCH Station1.R
mv Rplots.pdf k22s1.pdf

R CMD BATCH Station10.R
mv Rplots.pdf k22s10.pdf

make clean
make
./simu 24

R CMD BATCH Draw.R
mv Rplots.pdf k24.pdf

R CMD BATCH Station1.R
mv Rplots.pdf k24s1.pdf

R CMD BATCH Station10.R
mv Rplots.pdf k24s10.pdf

