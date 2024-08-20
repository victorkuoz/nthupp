#include <fstream>
#include <iomanip>
#include <iostream>
#include <vector>

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "wrong argument number\n");
        return 1;
    }

    std::ifstream input(argv[1], std::ios::binary);
    if (!input) {
        fprintf(stderr, "fail to open input file\n");
        return 1;
    }

    float value;
    std::vector<float> values;
    while (input.read(reinterpret_cast<char*>(&value), sizeof(float))) {
        values.push_back(value);
    }

    input.close();
    
    FILE *output = fopen(argv[2], "w");
    if (output == nullptr) {
        fprintf(stderr, "fail to open input file\n");
        return 1;
    }

    for (const float& value : values) {
        fprintf(output, "%.f\n", value);
    }

    fclose(output);

    return 0;
}