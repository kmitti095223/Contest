#include <iostream>
#include <vector>
#include <algorithm>
#include <string_view>
#include <cstring>

constexpr int ALPHABET_SIZE = (26+1);

// キーの3文字を基にインデックスを計算 (1～3文字対応)
constexpr int calculate_index(const char* key, size_t len) {
    int index = 0;
    if (len >= 1) index += (key[0] - 'a'+1) * ALPHABET_SIZE * ALPHABET_SIZE;
    if (len >= 2) index += (key[1] - 'a' + 1) * ALPHABET_SIZE;
    if (len >= 3) index += (key[2] - 'a' + 1);
    return index;
}

// FlatMap 定義
struct FlatMap {
    struct Record {
        std::string_view key;
        std::string_view value;

        // 比較演算子の定義
        bool operator<(const Record& other) const {
            return key < other.key;
        }
        bool operator<(const std::string_view& sv) const {
            return key < sv;
        }
    };

    std::vector<Record> records;

    void insert(std::string_view key, std::string_view value) {
        auto it = std::lower_bound(records.begin(), records.end(), key,
            [](const Record& rec, const std::string_view& str) { return rec.key < str; });
        if (it != records.end() && it->key == key) {
            it->value = value;
        }
        else {
            records.insert(it, { key, value });
        }
    }

    std::string_view get(std::string_view key) const {
        auto it = std::lower_bound(records.begin(), records.end(), key,
            [](const Record& rec, const std::string_view& str) { return rec.key < str; });
        if (it != records.end() && it->key == key) {
            return it->value;
        }
        return "0";
    }

    void remove(std::string_view key) {
        auto it = std::lower_bound(records.begin(), records.end(), key,
            [](const Record& rec, const std::string_view& str) { return rec.key < str; });
        if (it != records.end() && it->key == key) {
            records.erase(it);
        }
    }

    // 範囲を指定したダンプ
    void dump(std::string_view L, std::string_view R, char*& output_ptr) const {
        auto start = std::lower_bound(records.begin(), records.end(), L,
            [](const Record& rec, const std::string_view& str) { return rec.key < str; });
        auto last = std::upper_bound(records.begin(), records.end(), R,
            [](const std::string_view& str, const Record& rec) { return str < rec.key; });
        for (auto it = start; it != last; ++it) {
            output_ptr = std::copy(it->key.begin(), it->key.end(), output_ptr);
            *output_ptr++ = ' ';
            output_ptr = std::copy(it->value.begin(), it->value.end(), output_ptr);
            *output_ptr++ = '\n';
        }
    }

    // 全データをダンプ
    void dump2(char*& output_ptr) const {
        for (const auto& record : records) {
            output_ptr = std::copy(record.key.begin(), record.key.end(), output_ptr);
            *output_ptr++ = ' ';
            output_ptr = std::copy(record.value.begin(), record.value.end(), output_ptr);
            *output_ptr++ = '\n';
        }
    }
};

// 定数
FlatMap flat_maps[ALPHABET_SIZE * ALPHABET_SIZE * ALPHABET_SIZE];
char input_buffer[16 * 1024 * 1024];
char output_buffer[16 * 1024 * 1024];

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::cin.read(input_buffer, sizeof(input_buffer));  // 入力を一括取得
    char* ptr = input_buffer;

    int q = std::strtol(ptr, &ptr, 10);  // 最初にクエリ数を取得

    while (*ptr == '\n' || *ptr == ' ') ++ptr;  // 改行コードをスキップ

    char* output_ptr = output_buffer;  // 出力バッファの位置

    for (int i = 0; i < q; ++i) {
        while (*ptr == ' ') ++ptr;  // 空白をスキップ
        char op = *ptr++;  // コマンドを取得

        while (*ptr == ' ') ++ptr;  // 空白をスキップ

        if (op == '0') {  // insert
            char* key_start = ptr;
            while (*ptr != ' ') ++ptr;  // キーの終わり位置を探す
            *ptr++ = '\0';  // キー終端に NULL を挿入

            char* value_start = ptr;
            while (*ptr != '\n') ++ptr;  // 値の終わり位置を探す
            *ptr++ = '\0';  // 値終端に NULL を挿入

            int index = calculate_index(key_start, std::strlen(key_start));  // インデックスを計算
            flat_maps[index].insert(std::string_view(key_start), std::string_view(value_start));

        }
        else if (op == '1') {  // get
            char* key_start = ptr;
            while (*ptr != '\n') ++ptr;  // キーの終わり位置を探す
            *ptr++ = '\0';  // キー終端に NULL を挿入

            int index = calculate_index(key_start, std::strlen(key_start));
            std::string_view result = flat_maps[index].get(std::string_view(key_start));

            // 結果を直接バッファにコピー
            output_ptr = std::copy(result.begin(), result.end(), output_ptr);
            *output_ptr++ = '\n';

        }
        else if (op == '2') {  // delete
            char* key_start = ptr;
            while (*ptr != '\n') ++ptr;  // キーの終わり位置を探す
            *ptr++ = '\0';  // キー終端に NULL を挿入

            int index = calculate_index(key_start, std::strlen(key_start));
            flat_maps[index].remove(std::string_view(key_start));

        }
        else if (op == '3') {  // dump
            char* L_start = ptr;
            while (*ptr != ' ') ++ptr;  // Lキーの終わりを探す
            *ptr++ = '\0';  // L終端に NULL を挿入

            char* R_start = ptr;
            while (*ptr != '\n') ++ptr;  // Rキーの終わりを探す
            *ptr++ = '\0';  // R終端に NULL を挿入

            int start_index = calculate_index(L_start, std::strlen(L_start));
            int end_index = calculate_index(R_start, std::strlen(R_start));

            for (int idx = start_index; idx <= end_index; ++idx) {
                if (idx == start_index || idx == end_index) {
                    flat_maps[idx].dump(std::string_view(L_start), std::string_view(R_start), output_ptr);
                }
                else {
                    flat_maps[idx].dump2(output_ptr);
                }
            }
        }
    }

    // 出力バッファを一括で出力
    std::cout.write(output_buffer, output_ptr - output_buffer);
    return 0;
}

