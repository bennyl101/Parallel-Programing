#!/bin/bash
#$ -N Mandelbrot
#$ -q free64
#$ -pe one-node-mpi 20
#$ -R y
#$ -ckpt restart

# Grid Engine Notes:
# -----------------
# 1) Use "-R y" to request job reservation otherwise single 1-core jobs
#    may prevent this multicore MPI job from running.   This is called
#    job starvation.

# Module load boost
module load boost/1.57.0

# Module load OpenMPI
module load mpich-3.0.4/gcc-4.8.3

# Run the program
mpirun -np 20 ./mandelbrot_joe 2500 2500 >> log_joe_3
mpirun -np 20 ./mandelbrot_susie 2500 2500 >> log_susie_3
mpirun -np 20 ./mandelbrot_ms 2500 2500 >>log_ms_3


