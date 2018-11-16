#!/bin/bash
#SBATCH --time=36:00:00
#SBATCH --partition=gpu
#SBATCH --ntasks-per-node=1
srun -n 1 -c 1 ../build/bin/mpi_example -s -e 250000 -p 1 -d 4 -c gklsH -f gpu_mpi/gklsh4d/midaco_1_1_01.json --delay 0.1
srun -n 2 -c 1 ../build/bin/mpi_example -s -e 250000 -p 1 -d 4 -c gklsH -f gpu_mpi/gklsh4d/midaco_2_1_01.json --delay 0.1
srun -n 2 -c 16 ../build/bin/mpi_example -s -e 250000 -p 16 -d 4 -c gklsH -f gpu_mpi/gklsh4d/midaco_2_16_01.json --delay 0.1
srun -n 4 -c 1 ../build/bin/mpi_example -s -e 250000 -p 1 -d 4 -c gklsH -f gpu_mpi/gklsh4d/midaco_4_1_01.json --delay 0.1
srun -n 4 -c 16 ../build/bin/mpi_example -s -e 250000 -p 16 -d 4 -c gklsH -f gpu_mpi/gklsh4d/midaco_4_16_01.json --delay 0.1
srun -n 8 -c 1 ../build/bin/mpi_example -s -e 250000 -p 1 -d 4 -c gklsH -f gpu_mpi/gklsh4d/midaco_8_1_01.json --delay 0.1
srun -n 8 -c 16 ../build/bin/mpi_example -s -e 250000 -p 16 -d 4 -c gklsH -f gpu_mpi/gklsh4d/midaco_8_16_01.json --delay 0.1
srun -n 12 -c 1 ../build/bin/mpi_example -s -e 250000 -p 1 -d 4 -c gklsH -f gpu_mpi/gklsh4d/midaco_12_1_01.json --delay 0.1
srun -n 12 -c 16 ../build/bin/mpi_example -s -e 250000 -p 16 -d 4 -c gklsH -f gpu_mpi/gklsh4d/midaco_12_16_01.json --delay 0.1

srun -n 1 -c 1 ../build/bin/mpi_example -s -e 250000 -p 1 -d 4 -c gklsH -f gpu_mpi/gklsh4d/midaco_1_1_05.json --delay 0.5
srun -n 2 -c 1 ../build/bin/mpi_example -s -e 250000 -p 1 -d 4 -c gklsH -f gpu_mpi/gklsh4d/midaco_2_1_05.json --delay 0.5
srun -n 2 -c 16 ../build/bin/mpi_example -s -e 250000 -p 16 -d 4 -c gklsH -f gpu_mpi/gklsh4d/midaco_2_16_05.json --delay 0.5
srun -n 4 -c 1 ../build/bin/mpi_example -s -e 250000 -p 1 -d 4 -c gklsH -f gpu_mpi/gklsh4d/midaco_4_1_05.json --delay 0.5
srun -n 4 -c 16 ../build/bin/mpi_example -s -e 250000 -p 16 -d 4 -c gklsH -f gpu_mpi/gklsh4d/midaco_4_16_05.json --delay 0.5
srun -n 8 -c 1 ../build/bin/mpi_example -s -e 250000 -p 1 -d 4 -c gklsH -f gpu_mpi/gklsh4d/midaco_8_1_05.json --delay 0.5
srun -n 8 -c 16 ../build/bin/mpi_example -s -e 250000 -p 16 -d 4 -c gklsH -f gpu_mpi/gklsh4d/midaco_8_16_05.json --delay 0.5
srun -n 12 -c 1 ../build/bin/mpi_example -s -e 250000 -p 1 -d 4 -c gklsH -f gpu_mpi/gklsh4d/midaco_12_1_05.json --delay 0.5
srun -n 12 -c 16 ../build/bin/mpi_example -s -e 250000 -p 16 -d 4 -c gklsH -f gpu_mpi/gklsh4d/midaco_12_16_05.json --delay 0.5

srun -n 1 -c 1 ../build/bin/mpi_example -s -e 250000 -p 1 -d 4 -c gklsH -f gpu_mpi/gklsh4d/midaco_1_1_1.json --delay 1
srun -n 2 -c 1 ../build/bin/mpi_example -s -e 250000 -p 1 -d 4 -c gklsH -f gpu_mpi/gklsh4d/midaco_2_1_1.json --delay 1
srun -n 2 -c 16 ../build/bin/mpi_example -s -e 250000 -p 16 -d 4 -c gklsH -f gpu_mpi/gklsh4d/midaco_2_16_1.json --delay 1
srun -n 4 -c 1 ../build/bin/mpi_example -s -e 250000 -p 1 -d 4 -c gklsH -f gpu_mpi/gklsh4d/midaco_4_1_1.json --delay 1
srun -n 4 -c 16 ../build/bin/mpi_example -s -e 250000 -p 16 -d 4 -c gklsH -f gpu_mpi/gklsh4d/midaco_4_16_1.json --delay 1
srun -n 8 -c 1 ../build/bin/mpi_example -s -e 250000 -p 1 -d 4 -c gklsH -f gpu_mpi/gklsh4d/midaco_8_1_1.json --delay 1
srun -n 8 -c 16 ../build/bin/mpi_example -s -e 250000 -p 16 -d 4 -c gklsH -f gpu_mpi/gklsh4d/midaco_8_16_1.json --delay 1
srun -n 12 -c 1 ../build/bin/mpi_example -s -e 250000 -p 1 -d 4 -c gklsH -f gpu_mpi/gklsh4d/midaco_12_1_1.json --delay 1
srun -n 12 -c 16 ../build/bin/mpi_example -s -e 250000 -p 16 -d 4 -c gklsH -f gpu_mpi/gklsh4d/midaco_12_16_1.json --delay 1