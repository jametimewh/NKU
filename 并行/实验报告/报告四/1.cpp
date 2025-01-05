#include <mpi.h>
#include <stdio.h>
#include <math.h>

// 定义被积函数
double f(double x) {
    return x * x; 
}

// 梯形积分法实现
double trapezoidal_rule(double a, double b, int n, int rank, int size) {
    double h = (b - a) / n; // 每个小区间的宽度
    int local_n = n / size; // 每个进程负责的小区间数
    double local_a = a + rank * local_n * h; // 当前进程的起点
    double local_b = local_a + local_n * h; // 当前进程的终点

    // 计算本进程的局部积分
    double local_integral = (f(local_a) + f(local_b)) / 2.0;
    for (int i = 1; i < local_n; i++) {
        local_integral += f(local_a + i * h);
    }

    return local_integral * h;
}

int main(int argc, char** argv) {
    int rank, size;
    double global_integral = 0.0; // 全局积分值
    double a = 0.0, b = 1.0; // 积分区间 [a, b]
    int n = 12000; // 总的小区间数

    // 初始化 MPI 环境
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); // 获取进程号
    MPI_Comm_size(MPI_COMM_WORLD, &size); // 获取总进程数

    // 各进程计算其局部积分
    double local_integral = trapezoidal_rule(a, b, n, rank, size);

    // 主进程汇总所有进程的结果
    MPI_Reduce(&local_integral, &global_integral, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    // 主进程输出结果
    if (rank == 0) {
        printf("积分结果为: %.6lf\n", global_integral);
    }

    // 结束 MPI 环境
    MPI_Finalize();
    return 0;
}