#! /bin/bash

# compile gen.cc
rm -f gen
make gen
if [ $? -ne 0 ]; then
    exit 1
fi

# compile trans.cc
rm -f trans
make trans
if [ $? -ne 0 ]; then
    exit 1
fi

# compile main.cc
rm -f main
make
if [ $? -ne 0 ]; then
    exit 1
fi

# testing
process_number=10
for (( i=0; i<10; ++i ))
do
    rm -f *.bin *.txt
    echo "########## Testcase: $i ##########"
    random_number=$(awk 'BEGIN { srand(); print int(1 + rand() * 1000) }')

    ./gen $random_number in.bin sol.bin

    mpirun -np $process_number ./main $random_number in.bin out.bin

    ./trans in.bin in.txt

    ./trans out.bin out.txt

    ./trans sol.bin sol.txt

    if diff -q "out.bin" "sol.bin" > /dev/null; then
        echo "AC"
    else
        echo "WA"
    fi
done

rm -f *.bin *.txt
make clean