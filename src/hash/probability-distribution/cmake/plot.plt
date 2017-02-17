set terminal png
set datafile separator ','


set output 'p:b=12:m=256-modular.png' 
plot 'p:b=12:m=256.csv' using 1:2 with lines
set output 'p:b=14:m=256-modular.png' 
plot 'p:b=14:m=256.csv' using 1:2 with lines
set output 'p:b=16:m=512-modular.png' 
plot 'p:b=16:m=512.csv' using 1:2 with lines
set output 'p:b=12:m=256:x=32:y=63-modular.png' 
plot 'p:b=12:m=256:x=32:y=63.csv' using 1:2 with lines


set output 'p:b=12:m=256-multiply-shift.png' 
plot 'p:b=12:m=256.csv' using 1:3 with lines
set output 'p:b=14:m=256-multiply-shift.png' 
plot 'p:b=14:m=256.csv' using 1:3 with lines
set output 'p:b=16:m=512-multiply-shift.png' 
plot 'p:b=16:m=512.csv' using 1:3 with lines
set output 'p:b=12:m=256:x=32:y=63-multiply-shift.png' 
plot 'p:b=12:m=256:x=32:y=63.csv' using 1:3 with lines


set output 'p:b=12:m=256-transformations.png' 
plot 'p:b=12:m=256.csv' using 1:4 with lines
set output 'p:b=14:m=256-transformations.png' 
plot 'p:b=14:m=256.csv' using 1:4 with lines
set output 'p:b=16:m=512-transformations.png' 
plot 'p:b=16:m=512.csv' using 1:4 with lines
set output 'p:b=12:m=256:x=32:y=63-transformations.png' 
plot 'p:b=12:m=256:x=32:y=63.csv' using 1:4 with lines

