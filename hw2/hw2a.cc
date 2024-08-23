#include "complex.h"
#include <cassert>
#include <fstream>
#include <iostream>
#include <png.h>
#include <pthread.h>
#include <vector>

#include <stdlib.h>
#include <cstring>

void write_png(const char *filename, int iters, int width, int height, const int *buffer) {
    FILE *fp = fopen(filename, "wb");
    assert(fp);
    png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    assert(png_ptr);
    png_infop info_ptr = png_create_info_struct(png_ptr);
    assert(info_ptr);
    png_init_io(png_ptr, fp);
    png_set_IHDR(png_ptr, info_ptr, width, height, 8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
    png_set_filter(png_ptr, 0, PNG_NO_FILTERS);
    png_write_info(png_ptr, info_ptr);
    png_set_compression_level(png_ptr, 1);
    size_t row_size = 3 * width * sizeof(png_byte);
    png_bytep row = (png_bytep)malloc(row_size);
    for (int y = 0; y < height; ++y)
    {
        memset(row, 0, row_size);
        for (int x = 0; x < width; ++x)
        {
            int p = buffer[(height - 1 - y) * width + x];
            png_bytep color = row + x * 3;
            if (p != iters)
            {
                if (p & 16)
                {
                    color[0] = 240;
                    color[1] = color[2] = p % 16 * 16;
                }
                else
                {
                    color[0] = p % 16 * 16;
                }
            }
        }
        png_write_row(png_ptr, row);
    }
    free(row);
    png_write_end(png_ptr, NULL);
    png_destroy_write_struct(&png_ptr, &info_ptr);
    fclose(fp);
}

struct Argument {
    int i0, j0, i1, j1;

    Argument (int i0, int j0, int i1, int j1)
        : i0(i0), j0(j0), i1(i1), j1(j1) {}
};

namespace Glob {
    int *image, iter, width, height;
    double x0, y0, dx, dy;
}

void *simulation(void *arg) {
    auto [i0, j0, i1, j1] = *((Argument*) arg);

    for (int j = j0; j < j1; ++j) {
        double y = Glob::y0 + j * Glob::dy;

        for (int i = i0; i < i1; ++i) {
            double x = Glob::x0 + i * Glob::dx;
            Complex Z(0, 0), C(x, y);
            int result = 0;

            while (result < Glob::iter && Z.magnitudeSquared() < 4) {
                Z = Z.squared() + C;
                ++result;
            }

            Glob::image[j * Glob::width + i] = result;
        }
    }

    return nullptr;
}

int main (int argc, char **argv) {
    /* check the number of arguements */
    assert(argc == 9);

    /* argument parsing and initialization */
    Glob::iter = atoi(argv[2]);
    Glob::x0 = atof(argv[3]);
    Glob::y0 = atof(argv[5]);
    Glob::width = atoi(argv[7]);
    Glob::height = atoi(argv[8]);
    double x1 = atof(argv[4]);
    double y1 = atof(argv[6]);
    Glob::dx = (x1 - Glob::x0) / Glob::width;
    Glob::dy = (y1 - Glob::y0) / Glob::height;
    Glob::image = (int *) calloc(static_cast<int>(Glob::width) * static_cast<int>(Glob::height), sizeof(int));

    /* detect the nnumber of available CPUs */
    cpu_set_t cpu_set;
    sched_getaffinity(0, sizeof(cpu_set), &cpu_set);
    int thread_count = CPU_COUNT(&cpu_set);

    /* Mandelbrot Set */
    int w = Glob::width / thread_count + (Glob::width % thread_count != 0);
    int h = Glob::height / thread_count + (Glob::height % thread_count != 0);

    pthread_t thread_pool[thread_count * thread_count];
    for (int i0 = 0, idx = 0; i0 < Glob::width; i0 += w) {
        int i1 = std::min(Glob::width, i0 + w);
        for (int j0 = 0; j0 < Glob::height; j0 += h) {
            int j1 = std::min(Glob::height, j0 + h);
            Argument *argument = new Argument(i0, j0, i1, j1);
            pthread_create(&thread_pool[idx++], NULL, simulation, argument);
        }
    }

    for (auto thread : thread_pool) {
        pthread_join(thread, NULL);
    }

    /* draw and cleanup */
    char *output = argv[1];
    write_png(output, Glob::iter, Glob::width, Glob::height, Glob::image);
    free(Glob::image);
    return 0;
}