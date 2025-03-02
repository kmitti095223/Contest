#include <iostream>
#include <vector>
#include <algorithm>
#include <string_view>
#include <cstring>

constexpr int ALPHABET_SIZE = (26+1);

// �L�[��3��������ɃC���f�b�N�X���v�Z (1�`3�����Ή�)
constexpr int calculate_index(const char* key, size_t len) {
    int index = 0;
    if (len >= 1) index += (key[0] - 'a'+1) * ALPHABET_SIZE * ALPHABET_SIZE;
    if (len >= 2) index += (key[1] - 'a' + 1) * ALPHABET_SIZE;
    if (len >= 3) index += (key[2] - 'a' + 1);
    return index;
}

// FlatMap ��`
struct FlatMap {
    struct Record {
        std::string_view key;
        std::string_view value;

        // ��r���Z�q�̒�`
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

    // �͈͂��w�肵���_���v
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

    // �S�f�[�^���_���v
    void dump2(char*& output_ptr) const {
        for (const auto& record : records) {
            output_ptr = std::copy(record.key.begin(), record.key.end(), output_ptr);
            *output_ptr++ = ' ';
            output_ptr = std::copy(record.value.begin(), record.value.end(), output_ptr);
            *output_ptr++ = '\n';
        }
    }
};

// �萔
FlatMap flat_maps[ALPHABET_SIZE * ALPHABET_SIZE * ALPHABET_SIZE];
char input_buffer[16 * 1024 * 1024];
char output_buffer[16 * 1024 * 1024];

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::cin.read(input_buffer, sizeof(input_buffer));  // ���͂��ꊇ�擾
    char* ptr = input_buffer;

    int q = std::strtol(ptr, &ptr, 10);  // �ŏ��ɃN�G�������擾

    while (*ptr == '\n' || *ptr == ' ') ++ptr;  // ���s�R�[�h���X�L�b�v

    char* output_ptr = output_buffer;  // �o�̓o�b�t�@�̈ʒu

    for (int i = 0; i < q; ++i) {
        while (*ptr == ' ') ++ptr;  // �󔒂��X�L�b�v
        char op = *ptr++;  // �R�}���h���擾

        while (*ptr == ' ') ++ptr;  // �󔒂��X�L�b�v

        if (op == '0') {  // insert
            char* key_start = ptr;
            while (*ptr != ' ') ++ptr;  // �L�[�̏I���ʒu��T��
            *ptr++ = '\0';  // �L�[�I�[�� NULL ��}��

            char* value_start = ptr;
            while (*ptr != '\n') ++ptr;  // �l�̏I���ʒu��T��
            *ptr++ = '\0';  // �l�I�[�� NULL ��}��

            int index = calculate_index(key_start, std::strlen(key_start));  // �C���f�b�N�X���v�Z
            flat_maps[index].insert(std::string_view(key_start), std::string_view(value_start));

        }
        else if (op == '1') {  // get
            char* key_start = ptr;
            while (*ptr != '\n') ++ptr;  // �L�[�̏I���ʒu��T��
            *ptr++ = '\0';  // �L�[�I�[�� NULL ��}��

            int index = calculate_index(key_start, std::strlen(key_start));
            std::string_view result = flat_maps[index].get(std::string_view(key_start));

            // ���ʂ𒼐ڃo�b�t�@�ɃR�s�[
            output_ptr = std::copy(result.begin(), result.end(), output_ptr);
            *output_ptr++ = '\n';

        }
        else if (op == '2') {  // delete
            char* key_start = ptr;
            while (*ptr != '\n') ++ptr;  // �L�[�̏I���ʒu��T��
            *ptr++ = '\0';  // �L�[�I�[�� NULL ��}��

            int index = calculate_index(key_start, std::strlen(key_start));
            flat_maps[index].remove(std::string_view(key_start));

        }
        else if (op == '3') {  // dump
            char* L_start = ptr;
            while (*ptr != ' ') ++ptr;  // L�L�[�̏I����T��
            *ptr++ = '\0';  // L�I�[�� NULL ��}��

            char* R_start = ptr;
            while (*ptr != '\n') ++ptr;  // R�L�[�̏I����T��
            *ptr++ = '\0';  // R�I�[�� NULL ��}��

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

    // �o�̓o�b�t�@���ꊇ�ŏo��
    std::cout.write(output_buffer, output_ptr - output_buffer);
    return 0;
}

