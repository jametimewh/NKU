#include<iostream>
#include <stdio.h>
#include <pmmintrin.h>
#include<immintrin.h>
#include<xmmintrin.h>
#include <stdlib.h>
#include <algorithm>
#include <windows.h>
#include<cstdlib>
#include <chrono>
using namespace std;

const int maxN = 512; // magnitude of matrix
const int T = 64; // tile size
int n;
float a[maxN][maxN];
float b[maxN][maxN];
float c[maxN][maxN];
long long head, tail, freq; //timers
void mul(int n, float** a, float** b, float** c) {
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            c[i][j] = 0.0;
            for (int k = 0; k < n; ++k) {
                c[i][j] += a[i][k] * b[k][j];
            }
        }
    }
}

void trans_mul(int n, float** a, float** b, float** c) {
    // transposition
    for (int i = 0; i < n; ++i) for (int j = 0; j < i; ++j) swap(b[i][j], b[j][i]);
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            c[i][j] = 0.0;
            for (int k = 0; k < n; ++k) {
                c[i][j] += a[i][k] * b[j][k];
            }
        }
    }
    // transposition
    for (int i = 0; i < n; ++i) for (int j = 0; j < i; ++j) swap(b[i][j], b[j][i]);
}

void sse_tile(int n, float** a, float** b, float** c)
{
    __m128 t1, t2, sum;
    float t;
    for (int i = 0; i < n; ++i) for (int j = 0; j < i; ++j) swap(b[i][j], b[j][i]);
    for (int r = 0; r < n / T; ++r) for (int q = 0; q < n / T; ++q) {
        for (int i = 0; i < T; ++i) for (int j = 0; j < T; ++j) c[r * T + i][q * T +
            j] = 0.0;
        for (int p = 0; p < n / T; ++p) {
            for (int i = 0; i < T; ++i) for (int j = 0; j < T; ++j) {
                sum = _mm_setzero_ps();
                for (int k = 0; k < T; k += 4) { //sum every 4th elements
                    t1 = _mm_loadu_ps(a[r * T + i] + p * T + k);
                    t2 = _mm_loadu_ps(b[q * T + j] + p * T + k);
                    t1 = _mm_mul_ps(t1, t2);
                    sum = _mm_add_ps(sum, t1);
                }
                sum = _mm_hadd_ps(sum, sum);
                sum = _mm_hadd_ps(sum, sum);
                _mm_store_ss(&t, sum);
                c[r * T + i][q * T + j] += t;
            }
        }
    }
    for (int i = 0; i < n; ++i) for (int j = 0; j < i; ++j) swap(b[i][j], b[j][i]);
}

void sse_mul(int n, float** a, float** b, float** c) {
    __m128 t1, t2, sum;
    for (int i = 0; i < n; ++i) for (int j = 0; j < i; ++j) swap(b[i][j], b[j][i]);
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            c[i][j] = 0.0;
            sum = _mm_setzero_ps();
            for (int k = n - 4; k >= 0; k -= 4) { // sum every 4 elements
                t1 = _mm_loadu_ps(a[i] + k);
                t2 = _mm_loadu_ps(b[j] + k);
                t1 = _mm_mul_ps(t1, t2);
                sum = _mm_add_ps(sum, t1);
            }
            sum = _mm_hadd_ps(sum, sum);
            sum = _mm_hadd_ps(sum, sum);
            _mm_store_ss(c[i] + j, sum);
            for (int k = (n % 4) - 1; k >= 0; --k) { // handle the last n%4 elements
                c[i][j] += a[i][k] * b[j][k];
            }
        }
    }
    for (int i = 0; i < n; ++i) for (int j = 0; j < i; ++j) swap(b[i][j], b[j][i]);
}

int main() {
    float** mar = new float* [maxN];
    float** mar2 = new float* [maxN];
    float** result = new float* [maxN];
    for (int i = 0; i < maxN; i++) {
        mar[i] = new float[maxN];
        mar2[i] = new float[maxN];
        result[i] = new float[maxN];
    }
    for (int i = 0; i < maxN; i++) {
        for (int j = 0; j < maxN; j++) {
            mar[i][j] = rand() % 100;
        }
    }
    for (int i = 0; i < maxN; i++) {
        for (int j = 0; j < maxN; j++) {
            mar2[i][j] = rand() % 100;  // 随机生成 0-99 之间的数
        }
    }
    auto start = chrono::high_resolution_clock::now();
    mul(maxN, mar, mar2, result);
    //在这里替换
    auto end = chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration = end - start;

    // 输出持续时间
    std::cout << "执行时间: " << duration.count() << " 毫秒" << std::flush<< std::endl;
    start = chrono::high_resolution_clock::now();
    trans_mul(maxN, mar, mar2, result);
    //在这里替换
    end = chrono::high_resolution_clock::now();
    duration = end - start;

    // 输出持续时间
    std::cout << "执行时间: " << duration.count() << " 毫秒" << std::flush<< std::endl;
    start = chrono::high_resolution_clock::now();
    sse_mul(maxN, mar, mar2, result);
    //在这里替换
    end = chrono::high_resolution_clock::now();
    duration = end - start;

    // 输出持续时间
    std::cout << "执行时间: " << duration.count() << " 毫秒" << std::flush<< std::endl;
    start = chrono::high_resolution_clock::now();
    sse_tile(maxN, mar, mar2, result);
    //在这里替换
    end = chrono::high_resolution_clock::now();
    duration = end - start;

    // 输出持续时间
    std::cout << "执行时间: " << duration.count() << " 毫秒" << std::flush<< std::endl;

}

