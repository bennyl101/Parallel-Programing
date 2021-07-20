#include <stdlib.h>
#include <stdio.h>

#include "cuda_utils.h"
#include "timer.c"

typedef float dtype;
#define MAX_THREADS 256
#define MAX_BLOCKS 64
#define MIN(x,y) ((x < y) ? x : y)
#define TILE_SIZE    16
#define BLOCK_DIM  16

unsigned int nextPow2( unsigned int x ) {
    --x;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    return ++x;
}


__global__ void matTrans(dtype* AT, dtype* A, int N, int threads, int blocks){
 	__shared__ dtype tile[TILE_SIZE][TILE_SIZE + 1];
  
  	int block_width = TILE_SIZE * gridDim.x;
  	int x,y;
  	x = TILE_SIZE * blockIdx.x + threadIdx.x;
  	y = TILE_SIZE * blockIdx.y + threadIdx.y;

  	for (int i = 0; i < TILE_SIZE; i += BLOCK_DIM)
 	{
     	tile[threadIdx.x+i][threadIdx.y] = A[(x+i)*block_width + y];
  	}
	__syncthreads();

  	x = blockIdx.y * TILE_SIZE + threadIdx.y;  

  	y = blockIdx.x * TILE_SIZE + threadIdx.x;

	for (int i = 0; i < TILE_SIZE; i += BLOCK_DIM)
	{
		AT[(x+i)*block_width + y] = tile[threadIdx.x][threadIdx.y + i];
	}

}

void
parseArg (int argc, char** argv, int* N)
{
	if(argc == 2) {
		*N = atoi (argv[1]);
		assert (*N > 0);
	} else {
		fprintf (stderr, "usage: %s <N>\n", argv[0]);
		exit (EXIT_FAILURE);
	}
}

void
initArr (dtype* in, int N)
{
	int i;

	for(i = 0; i < N; i++) {
		in[i] = (dtype) rand () / RAND_MAX;
	}
}

void
cpuTranspose (dtype* A, dtype* AT, int N)
{
	int i, j;
	for(i = 0; i < N; i++) {
		for(j = 0; j < N; j++) {
			AT[j * N + i] = A[i * N + j];
		}
	}
}

int
cmpArr (dtype* a, dtype* b, int N)
{
	int cnt, i;

	cnt = 0;
	for(i = 0; i < N; i++) {
		if(abs(a[i] - b[i]) > 1e-6) cnt++;
	}

	return cnt;
}

void
gpuTranspose (dtype* A, dtype* AT, int N)
{
  struct stopwatch_t* timer = NULL;
  long double t_gpu;
	
  /* Setup timers */
  stopwatch_init ();
  timer = stopwatch_create ();
  int blocks;
  int threads; 
  threads = (N < MAX_THREADS) ? nextPow2(N) : MAX_BLOCKS;
  blocks = (N + threads - 1) / threads;

  dim3 gb(N/TILE_SIZE, N/TILE_SIZE, 1);
  dim3 tb(TILE_SIZE, BLOCK_DIM, 1);

  stopwatch_start (timer);

	matTrans <<<gb, tb>>> (AT, A, N, threads, blocks);

  cudaThreadSynchronize ();
  t_gpu = stopwatch_stop (timer);
  fprintf (stderr, "GPU transpose: %Lg secs ==> %Lg billion elements/second\n",
           t_gpu, (N * N) / t_gpu * 1e-9 );

}

int 
main(int argc, char** argv)
{


  /* variables */
	dtype *A, *ATgpu, *ATcpu;
  int err;

	int N;

  struct stopwatch_t* timer = NULL;
  long double t_cpu;

	N = -1;
	parseArg (argc, argv, &N);

  /* input and output matrices on host */
  /* output */
  ATcpu = (dtype*) malloc (N * N * sizeof (dtype));
  ATgpu = (dtype*) malloc (N * N * sizeof (dtype));

  /* input */
  A = (dtype*) malloc (N * N * sizeof (dtype));

	initArr (A, N * N);

	/* GPU transpose kernel */
	gpuTranspose (A, ATgpu, N);

  /* Setup timers */
  stopwatch_init ();
  timer = stopwatch_create ();

	stopwatch_start (timer);
  /* compute reference array */
	cpuTranspose (A, ATcpu, N);
  t_cpu = stopwatch_stop (timer);
  fprintf (stderr, "Time to execute CPU transpose kernel: %Lg secs\n",
           t_cpu);

  /* check correctness */
	err = cmpArr (ATgpu, ATcpu, N * N);
	if(err) {
		fprintf (stderr, "Transpose failed: %d\n", err);
	} else {
		fprintf (stderr, "Transpose successful\n");
	}

	free (A);
	free (ATgpu);
	free (ATcpu);

  return 0;
}
