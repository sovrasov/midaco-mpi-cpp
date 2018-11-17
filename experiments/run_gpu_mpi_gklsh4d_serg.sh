#!/bin/bash
#SBATCH --time=36:00:00
#SBATCH --partition=gpu
#SBATCH -N 12 

srun -n 1 ../build/bin/mpi_example -s -a 0.031622776601683 -e 250000 -p 1 -d 4 -c gklsH -f gpu_mpi/gklsh4d_serg/midaco_1_1_01.json --delay 0.1
srun -n 2 ../build/bin/mpi_example -s -a 0.031622776601683 -e 250000 -p 1 -d 4 -c gklsH -f gpu_mpi/gklsh4d_serg/midaco_2_1_01.json --delay 0.1
srun -n 2 ../build/bin/mpi_example -s -a 0.031622776601683 -e 250000 -p 16 -d 4 -c gklsH -f gpu_mpi/gklsh4d_serg/midaco_2_16_01.json --delay 0.1
srun -n 4 ../build/bin/mpi_example -s -a 0.031622776601683 -e 250000 -p 1 -d 4 -c gklsH -f gpu_mpi/gklsh4d_serg/midaco_4_1_01.json --delay 0.1
srun -n 4 ../build/bin/mpi_example -s -a 0.031622776601683 -e 250000 -p 16 -d 4 -c gklsH -f gpu_mpi/gklsh4d_serg/midaco_4_16_01.json --delay 0.1
srun -n 8 ../build/bin/mpi_example -s -a 0.031622776601683 -e 250000 -p 1 -d 4 -c gklsH -f gpu_mpi/gklsh4d_serg/midaco_8_1_01.json --delay 0.1
srun -n 8 ../build/bin/mpi_example -s -a 0.031622776601683 -e 250000 -p 16 -d 4 -c gklsH -f gpu_mpi/gklsh4d_serg/midaco_8_16_01.json --delay 0.1
srun -n 12 ../build/bin/mpi_example -s -a 0.031622776601683 -e 250000 -p 1 -d 4 -c gklsH -f gpu_mpi/gklsh4d_serg/midaco_12_1_01.json --delay 0.1
srun -n 12 ../build/bin/mpi_example -s -a 0.031622776601683 -e 250000 -p 16 -d 4 -c gklsH -f gpu_mpi/gklsh4d_serg/midaco_12_16_01.json --delay 0.1

srun -n 1 ../build/bin/mpi_example -s -a 0.031622776601683 -e 250000 -p 1 -d 4 -c gklsH -f gpu_mpi/gklsh4d_serg/midaco_1_1_05.json --delay 0.5
srun -n 2 ../build/bin/mpi_example -s -a 0.031622776601683 -e 250000 -p 1 -d 4 -c gklsH -f gpu_mpi/gklsh4d_serg/midaco_2_1_05.json --delay 0.5
srun -n 2 ../build/bin/mpi_example -s -a 0.031622776601683 -e 250000 -p 16 -d 4 -c gklsH -f gpu_mpi/gklsh4d_serg/midaco_2_16_05.json --delay 0.5
srun -n 4 ../build/bin/mpi_example -s -a 0.031622776601683 -e 250000 -p 1 -d 4 -c gklsH -f gpu_mpi/gklsh4d_serg/midaco_4_1_05.json --delay 0.5
srun -n 4 ../build/bin/mpi_example -s -a 0.031622776601683 -e 250000 -p 16 -d 4 -c gklsH -f gpu_mpi/gklsh4d_serg/midaco_4_16_05.json --delay 0.5
srun -n 8 ../build/bin/mpi_example -s -a 0.031622776601683 -e 250000 -p 1 -d 4 -c gklsH -f gpu_mpi/gklsh4d_serg/midaco_8_1_05.json --delay 0.5
srun -n 8 ../build/bin/mpi_example -s -a 0.031622776601683 -e 250000 -p 16 -d 4 -c gklsH -f gpu_mpi/gklsh4d_serg/midaco_8_16_05.json --delay 0.5
srun -n 12 ../build/bin/mpi_example -s -a 0.031622776601683 -e 250000 -p 1 -d 4 -c gklsH -f gpu_mpi/gklsh4d_serg/midaco_12_1_05.json --delay 0.5
srun -n 12 ../build/bin/mpi_example -s -a 0.031622776601683 -e 250000 -p 16 -d 4 -c gklsH -f gpu_mpi/gklsh4d_serg/midaco_12_16_05.json --delay 0.5

srun -n 1 ../build/bin/mpi_example -s -a 0.031622776601683 -e 250000 -p 1 -d 4 -c gklsH -f gpu_mpi/gklsh4d_serg/midaco_1_1_1.json --delay 1
srun -n 2 ../build/bin/mpi_example -s -a 0.031622776601683 -e 250000 -p 1 -d 4 -c gklsH -f gpu_mpi/gklsh4d_serg/midaco_2_1_1.json --delay 1
srun -n 2 ../build/bin/mpi_example -s -a 0.031622776601683 -e 250000 -p 16 -d 4 -c gklsH -f gpu_mpi/gklsh4d_serg/midaco_2_16_1.json --delay 1
srun -n 4 ../build/bin/mpi_example -s -a 0.031622776601683 -e 250000 -p 1 -d 4 -c gklsH -f gpu_mpi/gklsh4d_serg/midaco_4_1_1.json --delay 1
srun -n 4 ../build/bin/mpi_example -s -a 0.031622776601683 -e 250000 -p 16 -d 4 -c gklsH -f gpu_mpi/gklsh4d_serg/midaco_4_16_1.json --delay 1
srun -n 8 ../build/bin/mpi_example -s -a 0.031622776601683 -e 250000 -p 1 -d 4 -c gklsH -f gpu_mpi/gklsh4d_serg/midaco_8_1_1.json --delay 1
srun -n 8 ../build/bin/mpi_example -s -a 0.031622776601683 -e 250000 -p 16 -d 4 -c gklsH -f gpu_mpi/gklsh4d_serg/midaco_8_16_1.json --delay 1
srun -n 12 ../build/bin/mpi_example -s -a 0.031622776601683 -e 250000 -p 1 -d 4 -c gklsH -f gpu_mpi/gklsh4d_serg/midaco_12_1_1.json --delay 1
srun -n 12 ../build/bin/mpi_example -s -a 0.031622776601683 -e 250000 -p 16 -d 4 -c gklsH -f gpu_mpi/gklsh4d_serg/midaco_12_16_1.json --delay 1
