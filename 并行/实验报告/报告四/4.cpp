#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <emmintrin.h> // 包含 SSE 指令头文件

#define N X // 矩阵大小

void print_matrix(double matrix[N][N + 1]) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N + 1; j++) {
            printf("%10.4f ", matrix[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

void forward_elimination(double matrix[N][N + 1], int rank, int size) {
    for (int k = 0; k < N; k++) {
        // 广播主行到所有进程
        if (rank == 0) {
            for (int j = k; j < N + 1; j++) {
                matrix[k][j] /= matrix[k][k]; // 标准化主行
            }
        }
        MPI_Bcast(&matrix[k], N + 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

        // 每个进程处理自己的任务行
        for (int i = k + 1 + rank; i < N; i += size) {
            __m128d factor = _mm_set1_pd(matrix[i][k]); // 加载消元系数
            for (int j = k; j < N + 1; j += 2) {
                __m128d row = _mm_loadu_pd(&matrix[i][j]);   // 加载当前行
                __m128d pivot = _mm_loadu_pd(&matrix[k][j]); // 加载主行
                __m128d result = _mm_sub_pd(row, _mm_mul_pd(factor, pivot)); // row[j] -= factor * pivot[j]
                _mm_storeu_pd(&matrix[i][j], result);        // 存回结果
            }
        }

        // 将每个进程的更新行同步到主进程
        for (int i = k + 1 + rank; i < N; i += size) {
            MPI_Send(&matrix[i], N + 1, MPI_DOUBLE, 0, i, MPI_COMM_WORLD);
        }

        if (rank == 0) {
            for (int i = k + 1; i < N; i++) {
                MPI_Recv(&matrix[i], N + 1, MPI_DOUBLE, MPI_ANY_SOURCE, i, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }
        }
    }
}

void back_substitution(double matrix[N][N + 1], double* x) {
    for (int i = N - 1; i >= 0; i--) {
        x[i] = matrix[i][N];
        for (int j = i + 1; j < N; j++) {
            x[i] -= matrix[i][j] * x[j];
        }
    }
}

int main(int argc, char** argv) {
    int rank, size;
    double matrix[N][N]
    double x[N];

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0) {
        printf("初始矩阵:\n");
        print_matrix(matrix);
    }

    forward_elimination(matrix, rank, size);

    if (rank == 0) {
        printf("消元后的矩阵:\n");
        print_matrix(matrix);

        back_substitution(matrix, x);

        printf("解向量:\n");
        for (int i = 0; i < N; i++) {
            printf("x[%d] = %.4f\n", i, x[i]);
        }
    }

    MPI_Finalize();
    return 0;
}