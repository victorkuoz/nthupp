#include "mpi.h"

#include <fstream>
#include <iostream>
#include <vector>

int main (int argc, char **argv) {
    if (argc != 4) {
        fprintf(stderr, "wrong argument number\n");
        return 0;
    }

    int n = atoi(argv[1]), ec;

    // mpi initialization
    ec = MPI_Init(&argc, &argv);

    int procNum;
    ec = MPI_Comm_size(MPI_COMM_WORLD, &procNum);

    int curRank;
    ec = MPI_Comm_rank(MPI_COMM_WORLD, &curRank);

    // require at most n >> 1 processes
    procNum = std::min(procNum, n >> 1);

    if (curRank < procNum) {
        // open input file
        MPI_File input;
        ec = MPI_File_open(MPI_COMM_WORLD, argv[2], MPI_MODE_RDONLY, MPI_INFO_NULL, &input);

        if (ec != MPI_SUCCESS) {
            fprintf(stderr, "process: %d, fail to open the input file\n", curRank);
            MPI_Abort(MPI_COMM_WORLD, ec);
        }

        // calculate the sorting size and the file descriptor offset
        int sortSize = n / procNum, remainSize = n % procNum, offset = sizeof(float);

        if (curRank < remainSize) {
            ++sortSize;
            offset *= curRank * sortSize;
        } else {
            offset *= remainSize * (sortSize + 1) + (curRank - remainSize) * sortSize;
        }

        ec = MPI_File_seek(input, offset, MPI_SEEK_SET);

        // read from input file
        float *buffer = (float*) malloc(sortSize * sizeof(float));
        MPI_Status status;

        ec = MPI_File_read(input, buffer, sortSize, MPI_FLOAT, &status);
        ec = MPI_File_close(&input);

        // for (int i = 0; i < sortSize; ++i) {
        //     printf("%f\n", buffer[i]);
        // }

        int startIdx = offset / sizeof(float);  // the start index in the original array
        for (int phase = 0; phase < n; ++phase) {
            // printf("%d %d %d\n", curRank, startIdx, phase);
            if ((startIdx & 1) ^ (phase & 1)) {   // the startIdx does not match the phase
                for (int i = 1; i + 1 < sortSize; i += 2) {
                    if (buffer[i] > buffer[i + 1]) {
                        std::swap(buffer[i], buffer[i + 1]);
                    }
                }

                // non-blocking send
                MPI_Request reqL, reqR;

                if (curRank != 0) {
                    MPI_Isend(buffer, 1, MPI_FLOAT, curRank - 1, 0, MPI_COMM_WORLD, &reqL);
                }

                if (curRank != procNum - 1 && (sortSize & 1) == 0) {
                    MPI_Isend(buffer + sortSize - 1, 1, MPI_FLOAT, curRank + 1, 0, MPI_COMM_WORLD, &reqR);
                }

                // blocking recv
                float recvValue = -1;
                MPI_Status status;

                if (curRank != 0) {
                    MPI_Recv(&recvValue, 1, MPI_FLOAT, curRank - 1, 0, MPI_COMM_WORLD, &status);

                    if (recvValue > buffer[0]) {
                        std::swap(recvValue, buffer[0]);
                    }
                }

                if (curRank != procNum - 1 && (sortSize & 1) == 0) {
                    MPI_Recv(&recvValue, 1, MPI_FLOAT, curRank + 1, 0, MPI_COMM_WORLD, &status);

                    if (recvValue < buffer[sortSize - 1]) {
                        std::swap(recvValue, buffer[sortSize - 1]);
                    }
                }

            } else {    // the startIdx matches the phase
                for (int i = 0; i + 1 < sortSize; i += 2) {
                    if (buffer[i] > buffer[i + 1]) {
                        std::swap(buffer[i], buffer[i + 1]);
                    }
                }

                // non-blocking send
                MPI_Request reqR;

                if (curRank != procNum - 1 && (sortSize & 1) != 0) {
                    MPI_Isend(buffer + sortSize - 1, 1, MPI_FLOAT, curRank + 1, 0, MPI_COMM_WORLD, &reqR);
                }

                // blocking recv
                float recvValue = -1;
                MPI_Status status;

                if (curRank != procNum - 1 && (sortSize & 1) != 0) {
                    MPI_Recv(&recvValue, 1, MPI_FLOAT, curRank + 1, 0, MPI_COMM_WORLD, &status);

                    if (recvValue < buffer[sortSize - 1]) {
                        std::swap(recvValue, buffer[sortSize - 1]);
                    }
                }
            }
            MPI_Barrier(MPI_COMM_WORLD);
        }

        // open output file
        MPI_File output;
        ec = MPI_File_open(MPI_COMM_WORLD, argv[3], MPI_MODE_WRONLY | MPI_MODE_CREATE, MPI_INFO_NULL, &output);

        if (ec != MPI_SUCCESS) {
            fprintf(stderr, "process: %d, fail to open the output file\n", curRank);
            MPI_Abort(MPI_COMM_WORLD, ec);
        }

        ec = MPI_File_seek(output, offset, MPI_SEEK_SET);

        // write to output file
        ec = MPI_File_write(output, buffer, sortSize, MPI_FLOAT, &status);
        ec = MPI_File_close(&output);

        free(buffer);
    }

    // MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();
    return 0;
}