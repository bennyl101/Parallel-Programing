# Parallel-Programing
This repo contains my coursework for parallel programming. The code is only meant to be run on the UCI HPC cluster. 

#### Sorting Algorithms
This script compares the run time of two sorting algorithms over a large integer array. When run on the HPC cluster, the algorithms will have the work split depending on the number of cores specified.

#### Job Division
This script compares two different methods of task division in parallel programming. The task is to decode and reconstruct an image of a Mandelbrot set. Joe's algorithm divides the data into blocks. A core is assigned to each block for parallel processing. Susie's algorithm divides the data into rows of the image, assiging one core to one row. Once a core finishes processing their assigned row, a new row is assigned to the core. The sequential algorithm goes through the data one at a time to reconstruct the image.

#### Messaging Methods
This script showcases the different techniques for optimizing code to run in parallel such as for loop unrolling. The script uses the NVIDIA CUDA library. The code also compares messaging passing methods. 
