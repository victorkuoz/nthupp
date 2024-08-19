#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <limits>
#include <random>

int main (int argc, char **argv) {
    if (argc != 3) {
        std::cerr << "argument error\n";
        return 0;
    }

    int n = atoi(argv[1]);

    // create a random engine, seeding it with std::random_device for more randomness
    std::random_device ranDev;
    std::default_random_engine ranEng(ranDev());

    // set up the distribution
    std::uniform_real_distribution<float> dist(-1e30f, 1e30f);

    // open file for output
    std::ofstream output(argv[2], std::ios::binary);
    if (!output) {
        std::cerr << "output file error" << std::endl;
        return 0;
    }

    for (int i = 0; i < n; ++i) {
        while (true) {
            float ranNum = dist(ranEng);

            if (std::isinf(ranNum) == false) {
                output.write(reinterpret_cast<const char*>(&ranNum), sizeof(float));
                // std::cout << ranNum << std::endl;
                break;
            }
        }
    }

    return 0;
}