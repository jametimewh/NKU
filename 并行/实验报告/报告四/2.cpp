#include <mpi.h>
#include <stdio.h>
#include <math.h>

// 定义被积函数
double f(double x) {
    return x * x; 
}

// 梯形积分法计算单个区间的积分
double trapezoidal_single(double a, double b) {
    return (f(a) + f(b)) * (b - a) / 2.0;
}

int main(int argc, char** argv) {
    int rank, size;
    double global_integral = 0.0; // 全局积分值
    double a = 0.0, b = 1.0; // 积分区间 [a, b]
    int n = 12000; // 总的小区间数
    double h = (b - a) / n; // 每个小区间的宽度

    // 初始化 MPI 环境
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); // 获取进程号
    MPI_Comm_size(MPI_COMM_WORLD, &size); // 获取总进程数

    // 动态分配任务：每个进程负责一部分区间
    double local_integral = 0.0;
    for (int i = rank; i < n; i += size) {
        double x_start = a + i * h;
        double x_end = x_start + h;
        local_integral += trapezoidal_single(x_start, x_end);
    }

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