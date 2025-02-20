#include <cstdio>
#include <cstring>
#include <string_view>
#include <vector>
#include <algorithm>

using namespace std;

const int MAX_BUF = 1 << 24; // 16MB
char input_buffer[MAX_BUF];
int buf_index = 0;
char output_buffer[MAX_BUF];
int output_index = 0;

// 高速入力：整数
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

// 高速入力：string_view（動的確保を避ける）
inline std::string_view read_sv() {
    while (input_buffer[buf_index] == ' ' || input_buffer[buf_index] == '\n')
        ++buf_index;
    int start = buf_index;
    while (input_buffer[buf_index] > ' ')
        ++buf_index;
    return { input_buffer + start, size_t(buf_index - start) };
}

// 高速出力：string_view
inline void write_string(std::string_view sv) {
    int len = int(sv.size());
    if (output_index + len + 1 > MAX_BUF) {
        fwrite(output_buffer, 1, output_index, stdout);
        output_index = 0;
    }
    memcpy(output_buffer + output_index, sv.data(), len);
    output_index += len;
}

// 高速出力：スペース追加
inline void write_space() {
    if (output_index + 1 > MAX_BUF) {
        fwrite(output_buffer, 1, output_index, stdout);
        output_index = 0;
    }
    output_buffer[output_index++] = ' ';
}

// 高速出力：改行
inline void write_newline() {
    if (output_index + 1 > MAX_BUF) {
        fwrite(output_buffer, 1, output_index, stdout);
        output_index = 0;
    }
    output_buffer[output_index++] = '\n';
}

// FlatMap : キーは string_view、値も string_view とする
class FlatMap {
public:
    using value_type = std::pair<std::string_view, std::string_view>;

    void emplace(std::string_view key, std::string_view value) {
        auto it = std::lower_bound(data.begin(), data.end(), key,
            [](const value_type& a, std::string_view b) { return a.first < b; });
        if (it != data.end() && it->first == key) {
            it->second = value;
        }
        else {
            data.insert(it, { key, value });
        }
    }

    std::string_view* find(std::string_view key) {
        auto it = std::lower_bound(data.begin(), data.end(), key,
            [](const value_type& a, std::string_view b) { return a.first < b; });
        return (it != data.end() && it->first == key) ? &it->second : nullptr;
    }

    void erase(std::string_view key) {
        auto it = std::lower_bound(data.begin(), data.end(), key,
            [](const value_type& a, std::string_view b) { return a.first < b; });
        if (it != data.end() && it->first == key) {
            data.erase(it);
        }
    }

    auto begin() { return data.begin(); }
    auto end() { return data.end(); }

private:
    std::vector<value_type> data;
};

FlatMap M_flat[26 * 26 * 26];

int main() {
    int input_size = (int)fread(input_buffer, 1, MAX_BUF - 1, stdin);
    if (input_size <= 0)
        return 1;
    input_buffer[input_size] = '\0';

    int q = read_int();
    while (q--) {
        int op = read_int();
        std::string_view key = read_sv();

        int i1 = key[0] - 'a';
        int i2 = key.size() > 1 ? key[1] - 'a' : 0;
        int i3 = key.size() > 2 ? key[2] - 'a' : 0;

        int index = (i1 * 26 * 26) + (i2 * 26) + i3;

        if (op == 0) {
            std::string_view value = read_sv();
            M_flat[index].emplace(key, value);
        }
        else if (op == 1) {
            std::string_view* val = M_flat[index].find(key);
            write_string(val ? *val : "0");
            write_newline();
        }
        else if (op == 2) {
            M_flat[index].erase(key);
        }
        else if (op == 3) {
            std::string_view R = read_sv();
            int j1 = R[0] - 'a';
            int j2 = R.size() > 1 ? R[1] - 'a' : 0;
            int j3 = R.size() > 2 ? R[2] - 'a' : 0;

            int j_index = (j1 * 26 * 26) + (j2 * 26) + j3;

            for (int i = index; i <= j_index; ++i) {
                auto& mp = M_flat[i];

                auto it = (i == index || i == j_index) ? 
                    std::lower_bound(mp.begin(), mp.end(), key, 
                        [](const FlatMap::value_type& p, std::string_view key) { return p.first < key; }) : mp.begin();

                while (it != mp.end() && it->first <= R) {
                    write_string(it->first);
                    write_space();
                    write_string(it->second);
                    write_newline();
                    ++it;
                }
            }
        }
    }

    if (output_index > 0)
        fwrite(output_buffer, 1, output_index, stdout);

    return 0;
}
