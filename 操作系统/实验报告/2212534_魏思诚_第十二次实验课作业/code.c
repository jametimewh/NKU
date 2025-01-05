#include <iostream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <algorithm>
#include <random>
#include <climits> // For INT_MAX
#include <fstream> // For file output

using namespace std;

const int TOTAL_INSTRUCTIONS = 320; // 总指令数
const int PAGE_SIZE = 10;           // 每页大小
const int TOTAL_PAGES = 32;         // 总页数

// 生成指令序列
vector<int> generateInstructions() {
    vector<int> instructions;
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dist(0, TOTAL_INSTRUCTIONS - 1);

    int m = dist(gen); // 随机选取起始指令
    while (instructions.size() < TOTAL_INSTRUCTIONS) {
        // 顺序执行
        instructions.push_back(m);
        if (instructions.size() >= TOTAL_INSTRUCTIONS) break;

        // 跳转到前地址部分
        if (m > 0) {
            uniform_int_distribution<> dist_front(0, m - 1);
            int m1 = dist_front(gen);
            instructions.push_back(m1);
            if (instructions.size() >= TOTAL_INSTRUCTIONS) break;

            // 跳转到后地址部分
            if (m1 + 2 < TOTAL_INSTRUCTIONS) {
                uniform_int_distribution<> dist_back(m1 + 2, TOTAL_INSTRUCTIONS - 1);
                int m2 = dist_back(gen);
                instructions.push_back(m2);
                m = m2 + 1; // 更新下次顺序执行起始点
            }
        }
    }
    return instructions;
}

// 输出物理地址及内存块
void printPhysicalAddressAndBlock(int instruction, int page, vector<int>& memory, ofstream& outputFile) {
    int offset = instruction % PAGE_SIZE;
    int physical_address = page * PAGE_SIZE + offset;
    outputFile << "指令 " << instruction << " 的物理地址为 " << physical_address 
               << "，该指令存放在内存块 " << find(memory.begin(), memory.end(), page) - memory.begin() + 1 << endl;
}

// FIFO 置换算法
int simulateFIFO(const vector<int>& instructions, int memory_blocks, ofstream& outputFile) {
    queue<int> memory;
    unordered_map<int, bool> page_map;
    int page_faults = 0;

    for (int instruction : instructions) {
        int page = instruction / PAGE_SIZE;
        if (page_map[page]) { // 已在内存
            vector<int> memory_vector;
            queue<int> temp_queue = memory;
            while (!temp_queue.empty()) {
                memory_vector.push_back(temp_queue.front());
                temp_queue.pop();
            }
            printPhysicalAddressAndBlock(instruction, page, memory_vector, outputFile); // 输出物理地址和内存块
            continue; // 跳过置换
        }

        page_faults++;
        if (memory.size() >= memory_blocks) {
            int removed_page = memory.front();
            memory.pop();
            page_map[removed_page] = false;
        }
        memory.push(page);
        page_map[page] = true;

        // 将 queue 转换为 vector，方便调用 printPhysicalAddressAndBlock
        vector<int> memory_vector;
        queue<int> temp_queue = memory;
        while (!temp_queue.empty()) {
            memory_vector.push_back(temp_queue.front());
            temp_queue.pop();
        }

        printPhysicalAddressAndBlock(instruction, page, memory_vector, outputFile); // 输出物理地址和内存块
    }
    return page_faults;
}

// LRU 置换算法
int simulateLRU(const vector<int>& instructions, int memory_blocks, ofstream& outputFile) {
    vector<int> memory;
    unordered_map<int, int> last_used;
    int page_faults = 0;

    for (int i = 0; i < instructions.size(); i++) {
        int page = instructions[i] / PAGE_SIZE;
        auto it = find(memory.begin(), memory.end(), page);

        if (it != memory.end()) { // 已在内存
            printPhysicalAddressAndBlock(instructions[i], page, memory, outputFile); // 输出物理地址和内存块
            continue; // 跳过置换
        }

        page_faults++;
        if (memory.size() < memory_blocks) {
            memory.push_back(page);
        } else {
            int lru_page = memory[0];
            int lru_time = last_used[lru_page];
            for (int p : memory) {
                if (last_used[p] < lru_time) {
                    lru_page = p;
                    lru_time = last_used[p];
                }
            }
            replace(memory.begin(), memory.end(), lru_page, page);
        }
        last_used[page] = i;
        printPhysicalAddressAndBlock(instructions[i], page, memory, outputFile); // 输出物理地址和内存块
    }
    return page_faults;
}

// OPT 置换算法
int simulateOPT(const vector<int>& instructions, int memory_blocks, ofstream& outputFile) {
    vector<int> memory;
    int page_faults = 0;

    for (int i = 0; i < instructions.size(); i++) {
        int page = instructions[i] / PAGE_SIZE;
        auto it = find(memory.begin(), memory.end(), page);

        if (it != memory.end()) { // 已在内存
            printPhysicalAddressAndBlock(instructions[i], page, memory, outputFile); // 输出物理地址和内存块
            continue; // 跳过置换
        }

        page_faults++;
        if (memory.size() < memory_blocks) {
            memory.push_back(page);
        } else {
            unordered_map<int, int> future_use;
            for (int j : memory) future_use[j] = INT_MAX;
            for (int j = i + 1; j < instructions.size(); j++) {
                int future_page = instructions[j] / PAGE_SIZE;
                if (future_use.find(future_page) != future_use.end() && future_use[future_page] == INT_MAX) {
                    future_use[future_page] = j;
                }
            }
            int to_replace = memory[0];
            int max_dist = -1;
            for (int j : memory) {
                if (future_use[j] > max_dist) {
                    max_dist = future_use[j];
                    to_replace = j;
                }
            }
            replace(memory.begin(), memory.end(), to_replace, page);
        }
        printPhysicalAddressAndBlock(instructions[i], page, memory, outputFile); // 输出物理地址和内存块
    }
    return page_faults;
}

int main() {
    vector<int> instructions = generateInstructions();

    // 打开文件输出流
    ofstream outputFile("output.txt");
    if (!outputFile.is_open()) {
        cerr << "无法打开文件进行写入！" << endl;
        return 1;
    }

    outputFile << "Memory Blocks\tOPT (Hit Rate)\tFIFO (Hit Rate)\tLRU (Hit Rate)\n";
    for (int memory_blocks = 2; memory_blocks <= 10; memory_blocks++) {
        int opt_faults = simulateOPT(instructions, memory_blocks, outputFile);
        int fifo_faults = simulateFIFO(instructions, memory_blocks, outputFile);
        int lru_faults = simulateLRU(instructions, memory_blocks, outputFile);

        double opt_hit_rate = 1 - (double)opt_faults / TOTAL_INSTRUCTIONS;
        double fifo_hit_rate = 1 - (double)fifo_faults / TOTAL_INSTRUCTIONS;
        double lru_hit_rate = 1 - (double)lru_faults / TOTAL_INSTRUCTIONS;

        outputFile << memory_blocks << "\t\t" 
                   << opt_hit_rate << "\t\t" 
                   << fifo_hit_rate << "\t\t" 
                   << lru_hit_rate << "\n";
    }

    // 关闭文件输出流
    outputFile.close();

    cout << "输出已保存到 output.txt 文件中！" << endl;
    return 0;
}
