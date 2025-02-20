#include <cstring>
#include <iostream>
#include <set>
#include <cstdio>

#ifdef WINDOWS_DEBUG
#include <windows.h>
LARGE_INTEGER m_Freq, m_Start, m_End;
int nCount1 = 0, nCount2 = 0;
#endif

using namespace std;

struct cmp_str {
    bool operator()(const char* a, const char* b) const {
        return strcmp(a, b) < 0;
    }
};

const int MAX_BUF = 1 << 24;
char input_buffer[MAX_BUF];
int buf_index = 0;
char output_buffer[MAX_BUF];
int output_index = 0;

set<char*, cmp_str> M[26 * 26 * 26]; // 1次元配列に変更
int Num[26 * 26 * 26] = { 0 }; // 1次元配列に変更 // ★new に変えても変わらない

inline int read_int() {
    int x = 0;
    while (input_buffer[buf_index] < '0' || input_buffer[buf_index] > '9') {
        ++buf_index;
    }
    while (input_buffer[buf_index] >= '0' && input_buffer[buf_index] <= '9') {
        x = x * 10 + (input_buffer[buf_index] - '0');
        ++buf_index;
    }
    return x;
}

inline char* read_string() {
    while (input_buffer[buf_index] == ' ' || input_buffer[buf_index] == '\n') ++buf_index;
    char* start = &input_buffer[buf_index];
    while (input_buffer[buf_index] > ' ') ++buf_index;
    input_buffer[buf_index] = '\0';
    ++buf_index;
    return start;
}

inline void write_string(const char* str, char c = '\n') {
    int len = strlen(str);
    if (output_index + len + 1 > MAX_BUF) {
        fwrite(output_buffer, 1, output_index, stdout);
        output_index = 0;
    }
    memcpy(output_buffer + output_index, str, len);
    output_index += len;
    output_buffer[output_index++] = c;
}

inline void write_string2(const char* str) {

    int len = strlen(str);
    int len2 = len + strlen(str + len + 1) + 1;

    memcpy(output_buffer + output_index, str, len2);
    output_buffer[output_index + len] = ' ';
    output_index += len2;
    output_buffer[output_index++] = '\n';
}

inline void write_string3(const char* str) {

    str = str + (strlen(str) + 1);
    int len = strlen(str);

    memcpy(output_buffer + output_index, str, len);
    output_index += len;
    output_buffer[output_index++] = '\n';
}

int main() {
#ifdef WINDOWS_DEBUG
    QueryPerformanceFrequency(&m_Freq);
    QueryPerformanceCounter(&m_Start);
#endif

    int input_size = fread(input_buffer, 1, MAX_BUF - 1, stdin);
    if (input_size <= 0) return 1;
    input_buffer[input_size] = '\0';

    int map_max = 0;

    int q = read_int();
    while (q--) {
        char* op = read_string();
        char* key = read_string();
        int len1 = strlen(key);
        int index = (key[0] - 'a') * 676 + (len1 > 1 ? (key[1] - 'a') * 26 : 0) + (len1 > 2 ? key[2] - 'a' : 0);

        if (*op == '0') {
            char* value = read_string();

            auto it = M[index].find(key);// 60 ms
            if (it != M[index].end())
                M[index].erase(key);

            M[index].insert(key);
            Num[index] = 1;
        }
        else if (*op == '1') {

            if (Num[index]) {
                auto it = M[index].find(key);// 60 ms
                if (it == M[index].end())
                    write_string("0");
                else
                    write_string3(*it);
                // write_string(it != M[index].end() ? it->second : "0");// 0 ms
            }
            else
            {
                write_string("0");
            }
        }
        else if (*op == '2') {
            // 0 ms
            M[index].erase(key);
        }
        else if (*op == '3') {
            // 120 ms
            char* R = read_string();
            int len2 = strlen(R);
            int end_index = (R[0] - 'a') * 676 + (len2 > 1 ? (R[1] - 'a') * 26 : 0) + (len2 > 2 ? R[2] - 'a' : 0);
            for (int i = index; i <= end_index; ++i) {
                if (Num[i]) {
                    if ((i == index) || i == end_index)
                        for (auto it = M[i].lower_bound(key); it != M[i].end() && strcmp(*it, R) <= 0; ++it) {
                            write_string2(*it);
                        }
                    else
                        for (auto it = M[i].begin(); it != M[i].end(); ++it) {
                            write_string2(*it);
                        }
                }
            }
        }
    }

    if (output_index > 0) {
#ifndef WINDOWS_DEBUG
        fwrite(output_buffer, 1, output_index, stdout);
#endif
    }

#ifdef WINDOWS_DEBUG
    QueryPerformanceCounter(&m_End);
    printf("実行時間:%dms\n", (int)((double)(m_End.QuadPart - m_Start.QuadPart) * 1000.0 / m_Freq.QuadPart));
    printf("nCount1=%d  nCount2=%d\n", nCount1, nCount2);

    while (1) getchar();
#endif

    return 0;
}
