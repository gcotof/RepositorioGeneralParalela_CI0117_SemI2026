#!/bin/bash
#SBATCH --job-name=heat_scaling
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=16
#SBATCH --time=04:00:00
#SBATCH --partition=nukwa
#SBATCH --output=heat_%j.out

cd ~/heat

echo "===================================="
echo "COMPILING"
echo "===================================="

make clean
make

echo ""
echo "===================================="
echo "STRONG SCALING"
echo "===================================="

echo "N,Threads,AvgSeq,AvgPar,Speedup,Efficiency" > strong_scaling.csv

for N in 100 200 300; do
    for T in 1 2 4 8 16; do

        echo ""
        echo "===================================="
        echo "N=$N THREADS=$T"
        echo "===================================="

        export OMP_NUM_THREADS=$T

        seq_sum=0
        par_sum=0

        for run in 1 2 3; do

            echo "--- Run $run ---"

            output=$(./heat $N 1000 $T)
            echo "$output"

            seq=$(echo "$output" | grep "sequential execution time" | awk '{print $6}' | sed 's/s//')
            par=$(echo "$output" | grep "parallel execution time" | awk '{print $6}' | sed 's/s//')

            seq_sum=$(awk "BEGIN {print $seq_sum + $seq}")
            par_sum=$(awk "BEGIN {print $par_sum + $par}")

        done

        avg_seq=$(awk "BEGIN {print $seq_sum / 3}")
        avg_par=$(awk "BEGIN {print $par_sum / 3}")

        speedup=$(awk "BEGIN {print $avg_seq / $avg_par}")
        efficiency=$(awk "BEGIN {print 100 * $speedup / $T}")

        echo ""
        echo "AVERAGES:"
        echo "Seq = $avg_seq"
        echo "Par = $avg_par"
        echo "Speedup = $speedup"
        echo "Efficiency = $efficiency"

        echo "$N,$T,$avg_seq,$avg_par,$speedup,$efficiency" >> strong_scaling.csv

    done
done


echo ""
echo "===================================="
echo "WEAK SCALING"
echo "===================================="

echo "Threads,N,AvgPar,WeakEfficiency" > weak_scaling.csv

baseline=0

for T in 1 2 4 8 16; do

    N=$(python3 -c "import math; print(int(100 * (${T} ** (1/3))))")

    echo ""
    echo "===================================="
    echo "THREADS=$T N=$N"
    echo "===================================="

    export OMP_NUM_THREADS=$T

    par_sum=0

    for run in 1 2 3; do

        echo "--- Run $run ---"

        output=$(./heat $N 1000 $T)
        echo "$output"

        par=$(echo "$output" | grep "parallel execution time" | awk '{print $6}' | sed 's/s//')

        par_sum=$(awk "BEGIN {print $par_sum + $par}")

    done

    avg_par=$(awk "BEGIN {print $par_sum / 3}")

    if [ "$T" -eq 1 ]; then
        baseline=$avg_par
    fi

    weak_eff=$(awk "BEGIN {print 100 * $baseline / $avg_par}")

    echo ""
    echo "Avg Par = $avg_par"
    echo "Weak Eff = $weak_eff"

    echo "$T,$N,$avg_par,$weak_eff" >> weak_scaling.csv

done


echo ""
echo "===================================="
echo "DONE"
echo "===================================="
