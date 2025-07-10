#include <iostream>
#include "lodepng.h"
#include <vector>
using namespace std;

int main() {
    int width = 15000;
    int height = 10000;
    int maxIterations = 10000;

    std::vector<unsigned char> image(width * height * 4); // image buffer


    // loop trough pixels
    #pragma omp parallel for schedule(dynamic)
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {

            // map pixel to complex plane
            double a = (x / (double)width) * 3.0 - 2.0; // real part
            double b = (y / (double)height) * 2.0 - 1.0; // complex part
            
            // c = a + bi
            double ca = a;
            double cb = b;

            // z = 0 + 0i
            double zx = 0;
            double zy = 0;

            int iteration = 0;

            while (zx*zx + zy*zy < 4 && iteration < maxIterations) {
                double xtemp = zx*zx - zy*zy + ca;
                zy = 2*zx*zy + cb;
                zx = xtemp;
                iteration++;
            }

            int rval, gval, bval;

            if (iteration == maxIterations) {
                rval = 0;
                gval = 0;
                bval = 0;
            } else {
                rval = iteration % 256;
                gval = (iteration * 5) % 256;
                bval = (iteration * 13) % 256;
            }

            int index = 4 * (y * width +x);
            image[index + 0] = rval;
            image[index + 1] = gval;
            image[index + 2] = bval;
            image[index + 3] = 255;

            
        }

        #pragma omp critical
        {
            static int lastProgress = -1;  // shared among threads
            int progress = (100 * y) / height;

            if (progress % 2 == 0 && progress != lastProgress) {
                lastProgress = progress;

            // Draw progress bar
            cout << "\rRendering: " << progress << "% [";

            for (int i = 0; i < progress / 2; i++) {
                cout << "#";
            }

            for (int i = progress / 2; i < 50; i++) {
                cout << "_";
            }

            cout << "]" << flush;
            }
        }
    }


    unsigned error = lodepng::encode("mandelbrot.png", image, width, height);

    if (error) {
        std::cout << "Encode error " << error << ": " <<lodepng_error_text(error) << std::endl;
    } else {
        std::cout << "Saved image to mandelbrot.png" << std::endl;
    }
}