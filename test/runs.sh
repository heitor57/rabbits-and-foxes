#/usr/bin/bash

python3 ./test/run_all.py -e "./serial-rabbits-and-foxes"

for i in static dynamic guided "static, 400" "dynamic, 400" "guided, 400";
do
  for j in 4 8 12
  do
    echo $i $j
    python3 ./test/run_all.py -e "./parallel-rabbits-and-foxes" -d "OMP_NUM_THREADS='$j' OMP_SCHEDULE='$i'"
  done
done

