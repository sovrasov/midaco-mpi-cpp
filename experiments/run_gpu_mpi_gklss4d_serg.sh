#!/bin/bash
#SBATCH --time=36:00:00
#SBATCH --partition=gpu
#SBATCH --ntasks-per-node=1
srun -N 1 -n 1 ../build/bin/mpi_example -s -a 0.031622776601683 -e 150000 -p 1 -d 4 -c gklsS -f gpu_mpi/gklss4d_serg/midaco_1_1_01.json --delay 0.1
srun -N 2 -n 1 ../build/bin/mpi_example -s -a 0.031622776601683 -e 150000 -p 1 -d 4 -c gklsS -f gpu_mpi/gklss4d_serg/midaco_2_1_01.json --delay 0.1
srun -N 2 -n 16 ../build/bin/mpi_example -s -a 0.031622776601683 -e 150000 -p 16 -d 4 -c gklsS -f gpu_mpi/gklss4d_serg/midaco_2_16_01.json --delay 0.1
srun -N 4 -n 1 ../build/bin/mpi_example -s -a 0.031622776601683 -e 150000 -p 1 -d 4 -c gklsS -f gpu_mpi/gklss4d_serg/midaco_4_1_01.json --delay 0.1
srun -N 4 -n 16 ../build/bin/mpi_example -s -a 0.031622776601683 -e 150000 -p 16 -d 4 -c gklsS -f gpu_mpi/gklss4d_serg/midaco_4_16_01.json --delay 0.1
srun -N 8 -n 1 ../build/bin/mpi_example -s -a 0.031622776601683 -e 150000 -p 1 -d 4 -c gklsS -f gpu_mpi/gklss4d_serg/midaco_8_1_01.json --delay 0.1
srun -N 8 -n 16 ../build/bin/mpi_example -s -a 0.031622776601683 -e 150000 -p 16 -d 4 -c gklsS -f gpu_mpi/gklss4d_serg/midaco_8_16_01.json --delay 0.1
srun -N 12 -n 1 ../build/bin/mpi_example -s -a 0.031622776601683 -e 150000 -p 1 -d 4 -c gklsS -f gpu_mpi/gklss4d_serg/midaco_12_1_01.json --delay 0.1
srun -N 12 -n 16 ../build/bin/mpi_example -s -a 0.031622776601683 -e 150000 -p 16 -d 4 -c gklsS -f gpu_mpi/gklss4d_serg/midaco_12_16_01.json --delay 0.1

srun -N 1 -n 1 ../build/bin/mpi_example -s -a 0.031622776601683 -e 150000 -p 1 -d 4 -c gklsS -f gpu_mpi/gklss4d_serg/midaco_1_1_05.json --delay 0.5
srun -N 2 -n 1 ../build/bin/mpi_example -s -a 0.031622776601683 -e 150000 -p 1 -d 4 -c gklsS -f gpu_mpi/gklss4d_serg/midaco_2_1_05.json --delay 0.5
srun -N 2 -n 16 ../build/bin/mpi_example -s -a 0.031622776601683 -e 150000 -p 16 -d 4 -c gklsS -f gpu_mpi/gklss4d_serg/midaco_2_16_05.json --delay 0.5
srun -N 4 -n 1 ../build/bin/mpi_example -s -a 0.031622776601683 -e 150000 -p 1 -d 4 -c gklsS -f gpu_mpi/gklss4d_serg/midaco_4_1_05.json --delay 0.5
srun -N 4 -n 16 ../build/bin/mpi_example -s -a 0.031622776601683 -e 150000 -p 16 -d 4 -c gklsS -f gpu_mpi/gklss4d_serg/midaco_4_16_05.json --delay 0.5
srun -N 8 -n 1 ../build/bin/mpi_example -s -a 0.031622776601683 -e 150000 -p 1 -d 4 -c gklsS -f gpu_mpi/gklss4d_serg/midaco_8_1_05.json --delay 0.5
srun -N 8 -n 16 ../build/bin/mpi_example -s -a 0.031622776601683 -e 150000 -p 16 -d 4 -c gklsS -f gpu_mpi/gklss4d_serg/midaco_8_16_05.json --delay 0.5
srun -N 12 -n 1 ../build/bin/mpi_example -s -a 0.031622776601683 -e 150000 -p 1 -d 4 -c gklsS -f gpu_mpi/gklss4d_serg/midaco_12_1_05.json --delay 0.5
srun -N 12 -n 16 ../build/bin/mpi_example -s -a 0.031622776601683 -e 150000 -p 16 -d 4 -c gklsS -f gpu_mpi/gklss4d_serg/midaco_12_16_05.json --delay 0.5

srun -N 1 -n 1 ../build/bin/mpi_example -s -a 0.031622776601683 -e 150000 -p 1 -d 4 -c gklsS -f gpu_mpi/gklss4d_serg/midaco_1_1_1.json --delay 0.5
srun -N 2 -n 1 ../build/bin/mpi_example -s -a 0.031622776601683 -e 150000 -p 1 -d 4 -c gklsS -f gpu_mpi/gklss4d_serg/midaco_2_1_1.json --delay 1
srun -N 2 -n 16 ../build/bin/mpi_example -s -a 0.031622776601683 -e 150000 -p 16 -d 4 -c gklsS -f gpu_mpi/gklss4d_serg/midaco_2_16_1.json --delay 1
srun -N 4 -n 1 ../build/bin/mpi_example -s -a 0.031622776601683 -e 150000 -p 1 -d 4 -c gklsS -f gpu_mpi/gklss4d_serg/midaco_4_1_1.json --delay 1
srun -N 4 -n 16 ../build/bin/mpi_example -s -a 0.031622776601683 -e 150000 -p 16 -d 4 -c gklsS -f gpu_mpi/gklss4d_serg/midaco_4_16_1.json --delay 1
srun -N 8 -n 1 ../build/bin/mpi_example -s -a 0.031622776601683 -e 150000 -p 1 -d 4 -c gklsS -f gpu_mpi/gklss4d_serg/midaco_8_1_1.json --delay 1
srun -N 8 -n 16 ../build/bin/mpi_example -s -a 0.031622776601683 -e 150000 -p 16 -d 4 -c gklsS -f gpu_mpi/gklss4d_serg/midaco_8_16_1.json --delay 1
srun -N 12 -n 1 ../build/bin/mpi_example -s -a 0.031622776601683 -e 150000 -p 1 -d 4 -c gklsS -f gpu_mpi/gklss4d_serg/midaco_12_1_1.json --delay 1
srun -N 12 -n 16 ../build/bin/mpi_example -s -a 0.031622776601683 -e 150000 -p 16 -d 4 -c gklsS -f gpu_mpi/gklss4d_serg/midaco_12_16_1.json --delay 1