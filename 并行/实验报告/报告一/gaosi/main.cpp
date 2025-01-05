#include <iostream>
#include <vector>
#include <iomanip>
#include <cstdlib> // for rand()
#include <ctime>   // for time()
#include <chrono>  // for timing
#include <immintrin.h> // For AVX and SSE

using namespace std;

// 生成随机矩阵
vector<vector<double>> generateRandomMatrix(int rows, int cols) {
    vector<vector<double>> matrix(rows, vector<double>(cols));
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            matrix[i][j] = static_cast<double>(rand()) / RAND_MAX * 10.0; // 生成0到10之间的随机数
        }
    }
    return matrix;
}

// 串行高斯消元法
vector<double> gaussianEliminationSerial(vector<vector<double>>& matrix) {
    int n = matrix.size();
    for (int i = 0; i < n; ++i) {
        double maxEl = abs(matrix[i][i]);
        int maxRow = i;
        for (int k = i + 1; k < n; ++k) {
            if (abs(matrix[k][i]) > maxEl) {
                maxEl = abs(matrix[k][i]);
                maxRow = k;
            }
        }
        swap(matrix[maxRow], matrix[i]);
        for (int k = i + 1; k < n; ++k) {
            double c = -matrix[k][i] / matrix[i][i];
            for (int j = i; j < n + 1; ++j) {
                if (i == j) {
                    matrix[k][j] = 0;
                } else {
                    matrix[k][j] += c * matrix[i][j];
                }
            }
        }
    }
    vector<double> solution(n);
    for (int i = n - 1; i >= 0; --i) {
        solution[i] = matrix[i][n] / matrix[i][i];
        for (int k = i - 1; k >= 0; --k) {
            matrix[k][n] -= matrix[k][i] * solution[i];
        }
    }
    return solution;
}

// 使用SSE的高斯消元法
void eliminateRowSSE(vector<vector<double>>& matrix, int i, int n) {
    __m128d coeffs, row, tmp;
    for (int k = i + 1; k < n; ++k) {
        double scale = -matrix[k][i] / matrix[i][i];
        for (int j = i; j < n + 1; j += 2) {
            coeffs = _mm_set1_pd(scale * matrix[i][j]);
            row = _mm_loadu_pd(&matrix[k][j]);
            tmp = _mm_add_pd(row, coeffs);
            _mm_storeu_pd(&matrix[k][j], tmp);
        }
    }
}

vector<double> gaussianEliminationSSE(vector<vector<double>>& matrix) {
    int n = matrix.size();
    for (int i = 0; i < n; ++i) {
        double maxEl = abs(matrix[i][i]);
        int maxRow = i;
        for (int k = i + 1; k < n; ++k) {
            if (abs(matrix[k][i]) > maxEl) {
                maxEl = abs(matrix[k][i]);
                maxRow = k;
            }
        }
        swap(matrix[maxRow], matrix[i]);
        eliminateRowSSE(matrix, i, n);
    }
    vector<double> solution(n);
    for (int i = n - 1; i >= 0; --i) {
        solution[i] = matrix[i][n] / matrix[i][i];
        for (int k = i - 1; k >= 0; --k) {
            matrix[k][n] -= matrix[k][i] * solution[i];
        }
    }
    return solution;
}

// 使用AVX的高斯消元法
void eliminateRowAVX(vector<vector<double>>& matrix, int i, int n) {
    __m256d coeffs, row, tmp;
    for (int k = i + 1; k < n; ++k) {
        double scale = -matrix[k][i] / matrix[i][i];
        for (int j = i; j < n + 1; j += 4) {
            coeffs = _mm256_set1_pd(scale * matrix[i][j]);
            row = _mm256_loadu_pd(&matrix[k][j]);
            tmp = _mm256_add_pd(row, coeffs);
            _mm256_storeu_pd(&matrix[k][j], tmp);
        }
    }
    vector<double> solution(n);
    for (int i = n - 1; i >= 0; --i) {
        solution[i] = matrix[i][n] / matrix[i][i];
        for (int k = i - 1; k >= 0; --k) {
            matrix[k][n] -= matrix[k][i] * solution[i];
        }
    }
}

vector<double> gaussianEliminationAVX(vector<vector<double>>& matrix) {
    int n = matrix.size();
    for (int i = 0; i < n; ++i) {
        double maxEl = abs(matrix[i][i]);
        int maxRow = i;
        for (int k = i + 1; k < n; ++k) {
            if (abs(matrix[k][i]) > maxEl) {
                maxEl = abs(matrix[k][i]);
                maxRow = k;
            }
        }
        swap(matrix[maxRow], matrix[i]);
        eliminateRowAVX(matrix, i, n);
    }
    vector<double> solution(n);
    for (int i = n - 1; i >= 0; --i) {
        solution[i] = matrix[i][n] / matrix[i][i];
        for (int k = i - 1; k >= 0; --k) {
            matrix[k][n] -= matrix[k][i] * solution[i];
        }
    }
    return solution;
}

int main() {
    srand(static_cast<unsigned int>(time(0))); // 初始化随机数种子

    int rows = 500; // 行数
    int cols = 500; // 列数

    // 生成随机增广矩阵
    vector<vector<double>> matrix(rows, vector<double>(cols + 1));
    matrix = generateRandomMatrix(rows, cols);

    // 串行方法
    auto startSerial = chrono::high_resolution_clock::now();
    gaussianEliminationSerial(matrix);
    auto endSerial = chrono::high_resolution_clock::now();
    chrono::duration<double> durationSerial = endSerial - startSerial;


    // SSE方法
    auto startSSE = chrono::high_resolution_clock::now();
    gaussianEliminationSSE(matrix);
    auto endSSE = chrono::high_resolution_clock::now();
    chrono::duration<double> durationSSE = endSSE - startSSE;


    // AVX方法
    auto startAVX = chrono::high_resolution_clock::now();
    gaussianEliminationAVX(matrix);
    auto endAVX = chrono::high_resolution_clock::now();
    chrono::duration<double> durationAVX = endAVX - startAVX;

    // 输出时间
    //cout << "串行方法运行时间: " << durationSerial.count() << " 秒" << endl;
    cout << "SSE方法运行时间: " << durationSSE.count() << " 秒" << endl;
    cout << "AVX方法运行时间: " << durationAVX.count() << " 秒" << endl;

    return 0;
}
