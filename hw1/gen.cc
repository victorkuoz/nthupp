#include <algorithm>
#include <fstream>
#include <iostream>
#include <limits>
#include <random>
#include <vector>

int main (int argc, char **argv) {
    if (argc != 4) {
        fprintf(stderr, "wrong argument number\n");
        return 0;
    }

    int n = atoi(argv[1]);

    // create a random engine, seeding it with std::random_device for more randomness
    std::random_device ranDev;
    std::default_random_engine ranEng(ranDev());

    // set up the distribution
    std::uniform_real_distribution<float> dist(-1e30f, 1e30f);

    // open output file
    std::ofstream output(argv[2], std::ios::binary);
    if (!output) {
        fprintf(stderr, "fail to open output file\n");
        return 0;
    }

    std::vector<float> vec(n);
    for (int i = 0; i < n; ++i) {
        while (true) {
            float ranNum = dist(ranEng);

            if (std::isinf(ranNum) == false) {
                output.write(reinterpret_cast<const char*>(&ranNum), sizeof(float));
                vec[i] = ranNum;
                break;
            }
        }
    }

    std::sort(vec.begin(), vec.end());

    // open solution file
    std::ofstream solution(argv[3], std::ios::binary);
    if (!solution) {
        fprintf(stderr, "fail to open solution file\n");
        return 0;
    }
    solution.write(reinterpret_cast<const char*>(vec.data()), n * sizeof(float));

    return 0;
}