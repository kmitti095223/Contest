#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// �K�v�ȃo�b�t�@�T�C�Y���m��
static char input_buffer[12000000];  // ���̓f�[�^���ꊇ�i�[�i�\���ȃT�C�Y���m�ہj
static const char* suffix_array[1000000]; // �T�t�B�b�N�X�z��i�ő�T�̒����j
static const char* P[10000];          // �N�G��������̃|�C���^�z��i�ő�10000�̃N�G���j
static char output_buffer[100000];   // �o�̓o�b�t�@�i���ʂ��ꊇ�o�́j

// �񕪒T���p��r�֐��iP�̒����Ŕ�r�j
int compare_query(const void* key, const void* suffix) {
    const char* query = *(const char**)key;
    const char* suffix_str = *(const char**)suffix;
    size_t query_len = strlen(query);  // �N�G��������̒������v�Z
    return strncmp(query, suffix_str, query_len);
}

// ��r�֐��i�T�t�B�b�N�X�z��p�A�ő�1000�����܂Ŕ�r�j
int compare_suffixes(const void* a, const void* b) {
    const char* suffix_a = *(const char**)a;
    const char* suffix_b = *(const char**)b;
    return strncmp(suffix_a, suffix_b, 1000);  // �ő�1000������r
}

int main() {
    // ���̓f�[�^���ꊇ�ǂݍ���
    fread(input_buffer, 1, sizeof(input_buffer), stdin);

    // �|�C���^ `ptr` ����͗p�ɐݒ�
    char* ptr = input_buffer;

    // 1�s�ځF������ T ���擾
    char* T = ptr;
    while (*ptr != '\n') ptr++;
    *ptr++ = '\0';  // ���s�� '\0' �ɕϊ�
    int len_T = strlen(T);  // T �̒������擾

    // 2�s�ځF�N�G���� Q ���擾
    int Q = atoi(ptr);
    while (*ptr != '\n') ptr++;
    ptr++;  // ���s���X�L�b�v

    // �T�t�B�b�N�X�z����\�z�iT �̊e�����ւ̃|�C���^���i�[�j
    for (int i = 0; i < len_T; i++) {
        suffix_array[i] = &T[i];
    }

    // �T�t�B�b�N�X�z����������Ƀ\�[�g�i�ő�1000������r�j
    qsort(suffix_array, len_T, sizeof(const char*), compare_suffixes);

    // 1�ڂ̃��[�v�F�N�G������������ׂĎ擾�i`P` �̊e�v�f�ɃA�h���X���Z�b�g�j
    for (int i = 0; i < Q; i++) {
        P[i] = ptr;  // �N�G��������̊J�n�ʒu���Z�b�g
        while (*ptr != '\n' && *ptr != '\0') ptr++;
        if (*ptr == '\n') {
            *ptr++ = '\0';  // ���s�� '\0' �ɕϊ�
        }
    }

    // �o�̓o�b�t�@�p�̃|�C���^������
    char* out_ptr = output_buffer;

    // 2�ڂ̃��[�v�F��������
    for (int i = 0; i < Q; i++) {
        // bsearch ���g���ĕ��������� (P[i]) ���T�t�B�b�N�X�z����Ō���
        const char** result = (const char**)bsearch(
            &P[i],            // �T���L�[�i�|�C���^�̃A�h���X��n���j
            suffix_array,     // �T���Ώہi�T�t�B�b�N�X�z��j
            len_T,            // �v�f��
            sizeof(const char*), // �e�v�f�̃T�C�Y
            compare_query     // ��r�֐�
        );

        // �������ʂ��o�̓o�b�t�@�Ɋi�[
        *out_ptr++ = (result != NULL ? '1' : '0');
        *out_ptr++ = '\n';  // ���s��ǉ�
    }

    // �o�̓o�b�t�@��W���o�͂ւ܂Ƃ߂ď�������
    fwrite(output_buffer, 1, out_ptr - output_buffer, stdout);

    return 0;
}