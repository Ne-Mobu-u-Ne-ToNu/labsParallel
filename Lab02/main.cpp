#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <omp.h>

class LabTwoThreads {
private:
    int id;
    static const int SIZE = 50;
    static const int matrixCount = 1000;
    static const int MIN = -1000;
    static const int MAX = 1000;
    int from;
    int to;
    static std::vector<std::vector<std::vector<int>>> matrices;

public:
    LabTwoThreads(int id, int from, int to) : id(id), from(from), to(to) {}

    static void initMatrices() {
        std::srand(static_cast<unsigned int>(std::time(nullptr)));

#pragma omp parallel for
        for (int k = 0; k < matrixCount; k++) {
            std::vector<std::vector<int>> matrix(SIZE, std::vector<int>(SIZE));

            for (int i = 0; i < SIZE; i++) {
                for (int j = 0; j < SIZE; j++) {
                    matrix[i][j] = rnd(MIN, MAX);
                }
            }

#pragma omp critical
            matrices.push_back(matrix);
        }
    }

    static int rnd(int min, int max) {
        max -= min;
        return std::rand() % (max + 1) + min;
    }

    int getTrace(const std::vector<std::vector<int>>& matrix) {
        int result = 0;
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                if (i == j) result += matrix[i][j];
            }
        }
        return result;
    }

    void operator()() {
#pragma omp parallel for
        for (int i = from; i < to; i++) {
            int trace = getTrace(matrices[i]);
#pragma omp critical
            {
                std::cout << "Поток #" << id << " матрица #" << i << " след: " << trace << std::endl;
            }
        }
    }
};

std::vector<std::vector<std::vector<int>>> LabTwoThreads::matrices;

void threadTest(int threads) {
    int from = 0;
    int inc = 1000 / threads;
    int to = inc;
    std::vector<LabTwoThreads> threadObjects;

    for (int i = 0; i < threads; i++) {
        threadObjects.emplace_back(i + 1, from, to);
        from = to;
        to += inc;
    }

#pragma omp parallel for
    for (int i = 0; i < threads; i++) {
        threadObjects[i]();
    }
}

int main() {
    setlocale(LC_ALL, "Ru");
    LabTwoThreads::initMatrices();
    std::vector<long long> time(4, 0);
    int threads = 1;

    // Execute for accurate timing measurement
    threadTest(1);

    for (int i = 0; i < 4; i++) {
        double start = omp_get_wtime();
        threadTest(threads);
        double end = omp_get_wtime();
        time[i] = static_cast<long long>((end - start) * 1e6); // Convert to microseconds
        threads *= 2;
    }

    threads = 1;
    for (int i = 0; i < 4; i++) {
        std::cout << "Время выполнения микросекунд для " << threads << " потоков: " << time[i] << std::endl;
        threads *= 2;
    }

    return 0;
}
