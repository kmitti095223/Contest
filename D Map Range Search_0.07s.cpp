#include <iostream>
#include <map>
#include <string>
#include <cstdio>
#include <cstring>
using namespace std;

const int MAX_BUF = 1 << 24; // バッファサイズ16MB
char input_buffer[MAX_BUF];  // 入力バッファ
char output_buffer[MAX_BUF]; // 出力バッファ
int buf_index = 0;           // 入力バッファの現在位置
int out_index = 0;           // 出力バッファの現在位置

// 安全な整数読み込み
inline int read_int() {
    int x = 0, neg = 1;
    while (input_buffer[buf_index] < '0' || input_buffer[buf_index] > '9') {
        if (input_buffer[buf_index] == '-') neg = -1; // 負数対応
        ++buf_index;
    }
    while (input_buffer[buf_index] >= '0' && input_buffer[buf_index] <= '9') {
        x = x * 10 + (input_buffer[buf_index] - '0');
        ++buf_index;
    }
    return x * neg;
}

// 安全な文字列読み込み
inline void read_string(string& s) {
    // 空白や改行をスキップ
    while (input_buffer[buf_index] == ' ' || input_buffer[buf_index] == '\n') ++buf_index;

    s.clear();
    while (input_buffer[buf_index] != ' ' && input_buffer[buf_index] != '\n' && input_buffer[buf_index] != '\0') {
        s += input_buffer[buf_index];
        ++buf_index;
    }
}

// 高速な文字列出力
inline void write_string(const char* str) {
    while (*str != '\0') {
        output_buffer[out_index++] = *str++;
    }
    output_buffer[out_index++] = ' ';
}

// 高速な整数出力
inline void write_int(int x) {
    static char temp_buf[12];
    int pos = 0;
    if (x < 0) {
        output_buffer[out_index++] = '-';
        x = -x;
    }
    do {
        temp_buf[pos++] = '0' + (x % 10);
        x /= 10;
    } while (x > 0);
    while (pos > 0) {
        output_buffer[out_index++] = temp_buf[--pos];
    }
    output_buffer[out_index++] = '\n';
}

// 入出力処理のフラッシュ
inline void flush_output() {
    fwrite(output_buffer, 1, out_index, stdout);
    out_index = 0;
}

int main() {
    // 標準入力を一括で読み込む
    int input_size = fread(input_buffer, 1, MAX_BUF - 1, stdin);
    if (input_size == 0 || input_size >= MAX_BUF - 1) {
        cerr << "Input error or buffer overflow!" << endl;
        return 1; // 入力エラー
    }
    input_buffer[input_size] = '\0'; // 入力バッファ終端を設定

    int q = read_int(); // クエリ数の読み込み
    map<string, int> M; // 辞書データ構造

    while (q--) {
        int operation = read_int(); // 操作タイプ取得

        if (operation == 0) { // insert
            string key;
            int value;
            read_string(key);  // キーの取得
            value = read_int(); // 値の取得
            M[key] = value;    // マップに挿入または更新
        } else if (operation == 1) { // get
            string key;
            read_string(key);  // キーの取得
            auto it = M.find(key); // マップ検索
            if (it != M.end()) {
                write_int(it->second);
            } else {
                write_int(0);
            }
        } else if (operation == 2) { // delete
            string key;
            read_string(key);  // キーの取得
            M.erase(key);      // マップから削除
        } else if (operation == 3) { // dump
            string L, R;
            read_string(L);    // 範囲キー (開始)
            read_string(R);    // 範囲キー (終了)

            auto start_it = M.lower_bound(L); // 範囲の開始位置
            auto end_it = M.upper_bound(R);   // 範囲の終了位置
            for (auto it = start_it; it != end_it; ++it) {
                write_string(it->first.c_str());
                write_int(it->second);
            }
        }
    }

    flush_output(); // 出力をフラッシュ
    return 0;
}