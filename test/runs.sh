#/usr/bin/bash

for i in dynamic guided static;
do
  for j in 2 4 8 12 16 20 24
  do
    echo $i $j
    python3 ./test/run_all.py -e "./parallel-rabbits-and-foxes" -d "OMP_NUM_THREADS=$j OMP_SCHEDULE=$i"
  done
done

python3 ./test/run_all.py -e "./serial-rabbits-and-foxes"
