#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// 简单的冒泡排序算法
void bubble_sort(int* arr, int n) {
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (arr[j] > arr[j + 1]) {
                int temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }
}

int main(int argc, char** argv) {
    int rank, size;
    int num_arrays = 8;    // 总共的数组数量
    int array_size = 10;   // 每个数组的大小
    int* all_data = NULL;  // 存储所有数组的扁平化数据
    int* local_data = NULL; // 每个进程的本地数据
    int local_num_arrays;  // 每个进程负责的数组数量

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    local_num_arrays = num_arrays / size; // 静态分配，均分数组
    int remainder = num_arrays % size;   // 剩余数组用于补偿分配
    if (rank < remainder) {
        local_num_arrays++;
    }

    int local_data_size = local_num_arrays * array_size;

    if (rank == 0) {
        // 主进程初始化所有数组并扁平化存储
        all_data = (int*)malloc(num_arrays * array_size * sizeof(int));
        srand(time(NULL));
        for (int i = 0; i < num_arrays * array_size; i++) {
            all_data[i] = rand() % 100;
        }

        // 打印初始的数组
        printf("初始数组:\n");
        for (int i = 0; i < num_arrays; i++) {
            printf("数组 %d: ", i);
            for (int j = 0; j < array_size; j++) {
                printf("%d ", all_data[i * array_size + j]);
            }
            printf("\n");
        }
    }

    // 为本地数组分配内存
    local_data = (int*)malloc(local_data_size * sizeof(int));

    // 使用 MPI_Scatterv 将任务静态分配给每个进程
    int* sendcounts = (int*)malloc(size * sizeof(int));
    int* displs = (int*)malloc(size * sizeof(int));
    int offset = 0;
    for (int i = 0; i < size; i++) {
        sendcounts[i] = (num_arrays / size + (i < remainder ? 1 : 0)) * array_size;
        displs[i] = offset;
        offset += sendcounts[i];
    }

    MPI_Scatterv(all_data, sendcounts, displs, MPI_INT, local_data, local_data_size, MPI_INT, 0, MPI_COMM_WORLD);

    // 每个进程对其负责的数组进行排序
    for (int i = 0; i < local_num_arrays; i++) {
        bubble_sort(local_data + i * array_size, array_size);
    }

    // 将排序好的数据收集到主进程
    MPI_Gatherv(local_data, local_data_size, MPI_INT, all_data, sendcounts, displs, MPI_INT, 0, MPI_COMM_WORLD);

    // 主进程输出排序后的数组
    if (rank == 0) {
        printf("\n排序后的数组:\n");
        for (int i = 0; i < num_arrays; i++) {
            printf("数组 %d: ", i);
            for (int j = 0; j < array_size; j++) {
                printf("%d ", all_data[i * array_size + j]);
            }
            printf("\n");
        }
        free(all_data);
    }

    // 释放内存
    free(local_data);
    free(sendcounts);
    free(displs);

    MPI_Finalize();
    return 0;
}
