/**
 *  \file mandelbrot_joe.cc
 *
 *  \brief Implement your parallel mandelbrot set in this file.
 */

#include <iostream>
#include <cstdlib>
#include <mpi.h>

#include "render.hh"

using namespace std;

#define WIDTH 1000
#define HEIGHT 1000

int
mandelbrot(double x, double y) {
  int maxit = 511;
  double cx = x;
  double cy = y;
  double newx, newy;

  int it = 0;
  for (it = 0; it < maxit && (x*x + y*y) < 4; ++it) {
    newx = x*x - y*y + cx;
    newy = 2*x*y + cy;
    x = newx;
    y = newy;
  }
  return it;
}


int
main (int argc, char* argv[])
{
  /* Lucky you, you get to write MPI code */

  double minX = -2.1;
  double maxX = 0.7;
  double minY = -1.25;
  double maxY = 1.25;
  
  int ierr, num_procs, proc_id;


  int height, width;

  if (argc == 3) { //This sections determines the height and width of image based on user inputs
    height = atoi (argv[1]);
    width = atoi (argv[2]);
    assert (height > 0 && width > 0);
  } else {
    fprintf (stderr, "usage: %s <height> <width>\n", argv[0]);
    fprintf (stderr, "where <height> and <width> are the dimensions of the image.\n");
    return -1;
  }

  
  ierr = MPI_Init(NULL,  NULL); //initializes MPI processes, passing command line arguments through to each process

  ierr = MPI_Comm_rank(MPI_COMM_WORLD, &proc_id); //stores process id number, to be used to determine work load
  ierr = MPI_Comm_size(MPI_COMM_WORLD, &num_procs);//stores number of processes in num_proces
  double t2;
  double t1 = MPI_Wtime();

  double x, y, proc_offset;
  double it = (maxY - minY) / height; //Y-increment
  double jt = (maxX - minX) / width;  //X-increment
  int N = height/num_procs;//Determines how many rows of the image will be assinged per process

  float block_array[N*height]; //create array to contain mandelbrot data for on process
  int index = 0;
  for(int i = 0; i < N; ++i){
    proc_offset = proc_id* N + i;
    x = minX + proc_offset*jt;
    for( float j = minY; j < maxY; j+= it){
      block_array[index] = mandelbrot(x, j);
      index++;
    }

  } // fix this logic


  float* final_data;

  if( proc_id == 0){
    final_data = new float[width*height];
  }

  MPI_Gather(&block_array, N*height, MPI_INT, final_data, N*height, MPI_INT, 0, MPI_COMM_WORLD);
  t2 = MPI_Wtime();
  printf("Time elapsed is %1.2f\n", t2-t1);
  MPI_Finalize();
if(proc_id == 0){
  gil::rgb8_image_t img(height, width);
  auto img_view = gil::view(img);

  y = minY;
  x = 0;
  float value;
  int key = 0;
  for (int i = 0; i < height; ++i) {
    for (int j = 0; j < width; ++j) {
      value = final_data[key];
      img_view(i, j) = render(value/512.0);
      key++;
    }
  }
  gil::png_write_view("mandelbrot_joe.png", const_view(img));
}
}

/* eof */
