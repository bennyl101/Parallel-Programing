/**
 *  \file mandelbrot_ms.cc
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
#define return_data_tag 2002
#define send_data_tag 2001

int mandelbrot(double x, double y)
{
  int maxit = 511;
  double cx = x;
  double cy = y;
  double newx, newy;

  int it = 0;
  for (it = 0; it < maxit && (x * x + y * y) < 4; ++it)
  {
    newx = x * x - y * y + cx;
    newy = 2 * x * y + cy;
    x = newx;
    y = newy;
  }
  return it;
}
int main(int argc, char *argv[])
{
  /* Lucky you, you get to write MPI code */
  double minX = -2.1;
  double maxX = 0.7;
  double minY = -1.25;
  double maxY = 1.25;

  int height, width;

  if (argc == 3)
  { //This sections determines the height and width of image based on user inputs
    height = atoi(argv[1]);
    width = atoi(argv[2]);
    assert(height > 0 && width > 0);
  }
  else
  {
    fprintf(stderr, "usage: %s <height> <width>\n", argv[0]);
    fprintf(stderr, "where <height> and <width> are the dimensions of the image.\n");
    return -1;
  }

  int my_id, root_process, ierr, num_procs, an_id, index_return;
  int x_coordinate = 0;
  int task_finish = 0;
  int task_finish_check = 0;

  float temp_array[height];

  root_process = 0;

  ierr = MPI_Init(&argc, &argv);
  MPI_Status status;
  ierr = MPI_Comm_rank(MPI_COMM_WORLD, &my_id);
  ierr = MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

  double it = (maxY - minY) / height; //Y-increment
  double jt = (maxX - minX) / width;  //X-increment

  double x, y, proc_offset;
  y = minY;
  x = minX;
  float* final_data;

  int index = 0;
  if (my_id == 0)
  {
    final_data = new float[width*height];
    gil::rgb8_image_t img(height, width);
    auto img_view = gil::view(img);
    while (x < maxX)
    {
      for (an_id = 1; an_id < num_procs; an_id++)
      {
        x_coordinate = x;
        ierr = MPI_Send(&task_finish, 1, MPI_INT, an_id, send_data_tag, MPI_COMM_WORLD);
        ierr = MPI_Send(&x_coordinate, 1, MPI_INT, an_id, send_data_tag, MPI_COMM_WORLD);
        ierr = MPI_Send(&final_data[index * height], height, MPI_INT, an_id, send_data_tag, MPI_COMM_WORLD);
        ierr = MPI_Send(&index, 1, MPI_INT, an_id, send_data_tag, MPI_COMM_WORLD);
        x += jt;
        index++;
        ierr = MPI_Recv(&index_return, 1, MPI_INT, root_process, return_data_tag, MPI_COMM_WORLD, &status);
        ierr = MPI_Recv(&final_data[index_return * height], height, MPI_INT, root_process, return_data_tag, MPI_COMM_WORLD, &status);
        if (x > maxX)
        {
          break;
        }
      }
    }
    task_finish = 1;
    for (an_id = 1; an_id < num_procs; an_id++)
    {
      ierr = MPI_Send(&task_finish, 1, MPI_INT, an_id, send_data_tag, MPI_COMM_WORLD);
    }
    x = minX;
    int x_val = 0;

    y = minY;
    x = minX;
    float value;
    int key = 0;
    for (int i = 0; i < height; ++i)
    {
      for (int j = 0; j < width; ++j)
      {
        value = final_data[key];
        img_view(i, j) = render(value / 512.0);
        key++;
      }
    }
    gil::png_write_view("mandelbrot_ms.png", const_view(img));
  }
  else
  {
    int key = 0;
    while (task_finish == 0)
    {
      ierr = MPI_Recv(&task_finish, 1, MPI_INT, root_process, send_data_tag, MPI_COMM_WORLD, &status);
      task_finish_check = task_finish;
      if (task_finish_check == 1)
      {
        break;
      }
      ierr = MPI_Recv(&x_coordinate, 1, MPI_INT, root_process, send_data_tag, MPI_COMM_WORLD, &status);
      ierr = MPI_Recv(&temp_array, height, MPI_INT, root_process, send_data_tag, MPI_COMM_WORLD, &status);
      ierr = MPI_Recv(&index_return, 1, MPI_INT, root_process, send_data_tag, MPI_COMM_WORLD, &status);

      x = x_coordinate;
      for (float j = minY; j < maxY; j += it)
      {
        temp_array[key] = mandelbrot(x, j);
        key++;
      }
      ierr = MPI_Send(&index_return, 1, MPI_INT, root_process, return_data_tag, MPI_COMM_WORLD);
      ierr = MPI_Send(&temp_array, height, MPI_INT, root_process, return_data_tag, MPI_COMM_WORLD);
    }
    ierr = MPI_Finalize();
  }
}

/* eof */
