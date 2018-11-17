#!/bin/bash
#SBATCH --time=36:00:00
#SBATCH --partition=gpu
#SBATCH -N 12

declare -a n_nodes=("1" "2" "4" "8" "12")
declare -a delays=("0.1" "0.5" "1")

eps=0.031622776601683
iters=250000
class="gklsH"
folder="gpu_mpi/gklsh4d_serg"

for delay in "${delays[@]}"
do
  delay_str=${delay//[.]/}
  for n_num in "${n_nodes[@]}"
  do
    srun -n $n_num ../build/bin/mpi_example -s -a $eps -e $iters -p 1 -d 4 -c $class -f $folder/midaco_"$n_num"_1_$delay_str.json --delay $delay
    srun -n $n_num ../build/bin/mpi_example -s -a $eps -e $iters -p 16 -d 4 -c $class -f $folder/midaco_"$n_num"_16_$delay_str.json --delay $delay
  done
done