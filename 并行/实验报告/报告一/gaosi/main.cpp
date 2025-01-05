#include <iostream>
#include <vector>
#include <iomanip>
#include <cstdlib> // for rand()
#include <ctime>   // for time()
#include <chrono>  // for timing
#include <immintrin.h> // For AVX and SSE

using namespace std;

// �����������
vector<vector<double>> generateRandomMatrix(int rows, int cols) {
    vector<vector<double>> matrix(rows, vector<double>(cols));
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            matrix[i][j] = static_cast<double>(rand()) / RAND_MAX * 10.0; // ����0��10֮��������
        }
    }
    return matrix;
}

// ���и�˹��Ԫ��
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

// ʹ��SSE�ĸ�˹��Ԫ��
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

// ʹ��AVX�ĸ�˹��Ԫ��
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
    srand(static_cast<unsigned int>(time(0))); // ��ʼ�����������

    int rows = 500; // ����
    int cols = 500; // ����

    // ��������������
    vector<vector<double>> matrix(rows, vector<double>(cols + 1));
    matrix = generateRandomMatrix(rows, cols);

    // ���з���
    auto startSerial = chrono::high_resolution_clock::now();
    gaussianEliminationSerial(matrix);
    auto endSerial = chrono::high_resolution_clock::now();
    chrono::duration<double> durationSerial = endSerial - startSerial;


    // SSE����
    auto startSSE = chrono::high_resolution_clock::now();
    gaussianEliminationSSE(matrix);
    auto endSSE = chrono::high_resolution_clock::now();
    chrono::duration<double> durationSSE = endSSE - startSSE;


    // AVX����
    auto startAVX = chrono::high_resolution_clock::now();
    gaussianEliminationAVX(matrix);
    auto endAVX = chrono::high_resolution_clock::now();
    chrono::duration<double> durationAVX = endAVX - startAVX;

    // ���ʱ��
    //cout << "���з�������ʱ��: " << durationSerial.count() << " ��" << endl;
    cout << "SSE��������ʱ��: " << durationSSE.count() << " ��" << endl;
    cout << "AVX��������ʱ��: " << durationAVX.count() << " ��" << endl;

    return 0;
}
