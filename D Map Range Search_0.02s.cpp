#include <cstring>
#include <iostream>
#include <map>
#include <string>
#include <cstdio>
using namespace std;

const int MAX_BUF = 1 << 24; // バッファサイズ16MB
char input_buffer[MAX_BUF];  // 入力バッファ
int buf_index = 0;           // 入力バッファの現在位置
char output_buffer[MAX_BUF]; // 出力バッファ
int output_index = 0;        // 出力バッファの現在位置

map<string, int> M[26][26][26]; // 先頭3文字ごとに分割した26×26×26個のmap

inline int read_int() {
    int x = 0, neg = 1;
    while (input_buffer[buf_index] < '0' || input_buffer[buf_index] > '9') {
        if (input_buffer[buf_index] == '-') neg = -1;
        ++buf_index;
    }
    while (input_buffer[buf_index] >= '0' && input_buffer[buf_index] <= '9') {
        x = x * 10 + (input_buffer[buf_index] - '0');
        ++buf_index;
    }
    return x * neg;
}

inline string read_string() {
    while (input_buffer[buf_index] == ' ' || input_buffer[buf_index] == '\n') ++buf_index;
    int start = buf_index;
    while (input_buffer[buf_index] > ' ') ++buf_index;
    return string(&input_buffer[start], &input_buffer[buf_index]);
}

inline void write_string(const string& str) {
    int len = str.size();
    if (output_index + len + 1 > MAX_BUF) {
        fwrite(output_buffer, 1, output_index, stdout);
        output_index = 0;
    }
    memcpy(output_buffer + output_index, str.c_str(), len);
    output_index += len;
    output_buffer[output_index++] = ' ';
}

inline void write_int(int x) {
    char buf[12];
    int index = 0;
    if (x == 0) {
        output_buffer[output_index++] = '0';
        output_buffer[output_index++] = '\n';
        return;
    }
    while (x) {
        buf[index++] = '0' + (x % 10);
        x /= 10;
    }
    while (index) output_buffer[output_index++] = buf[--index];
    output_buffer[output_index++] = '\n';
}

int main() {
    int input_size = fread(input_buffer, 1, MAX_BUF - 1, stdin);
    if (input_size <= 0) return 1;
    input_buffer[input_size] = '\0';

    int q = read_int();
    while (q--) {
        int op = read_int();
        string key = read_string();
        int i1 = key[0] - 'a';
        int i2 = key.size() > 1 ? key[1] - 'a' : 0;
        int i3 = key.size() > 2 ? key[2] - 'a' : 0;
        
        if (op == 0) {
            int value = read_int();
            M[i1][i2][i3][key] = value;
        } else if (op == 1) {
            auto it = M[i1][i2][i3].find(key);
            write_int(it != M[i1][i2][i3].end() ? it->second : 0);
        } else if (op == 2) {
            M[i1][i2][i3].erase(key);
        } else if (op == 3) {
            string R = read_string();
            int j1 = R[0] - 'a';
            int j2 = R.size() > 1 ? R[1] - 'a' : 25;
            int j3 = R.size() > 2 ? R[2] - 'a' : 25;
            for (int i = i1; i <= j1; ++i) {
                for (int j = (i == i1 ? i2 : 0); j <= (i == j1 ? j2 : 25); ++j) {
                    for (int k = (i == i1 && j == i2 ? i3 : 0); k <= (i == j1 && j == j2 ? j3 : 25); ++k) {
                        for (auto it = M[i][j][k].lower_bound((i == i1 && j == i2 && k == i3) ? key : ""); it != M[i][j][k].end() && it->first <= R; ++it) {
                            write_string(it->first);
                            write_int(it->second);
                        }
                    }
                }
            }
        }
    }
    
    // バッファに残っているデータを出力
    if (output_index > 0) {
        fwrite(output_buffer, 1, output_index, stdout);
    }
    
    return 0;
}
