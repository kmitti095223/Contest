#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INPUT_BUFFER_SIZE 12000000  // ���̓o�b�t�@�̃T�C�Y�i12MB�j
#define OUTPUT_BUFFER_SIZE 1200000  // �o�̓o�b�t�@�̃T�C�Y�i1.2MB�j
#define MAX_T_LEN 1000000           // T�̍ő咷
#define MAX_P_LEN 1000              // P�̍ő咷�i��r����ő啶�����j
#define MAX_Q 10000                 // �N�G���̍ő吔
#define MAX_INDEX_SIZE 16777216     // �z��̍ő�C���f�b�N�X�T�C�Y�i256�~256�~256�j

// �T�t�B�b�N�X�z��p�̔�r�֐��i�ő�MAX_P_LEN�܂Ŕ�r�j
int compareSuffix(const void *a, const void *b) {
    const char *suffixA = *(const char **)a;
    const char *suffixB = *(const char **)b;
    return strncmp(suffixA, suffixB, MAX_P_LEN);  // �ő�MAX_P_LEN�����܂Ŕ�r
}

// bsearch�p�̃N�G����r�֐��i�N�G��������ƃT�t�B�b�N�X���r�A�K�v������������r�j
int compareQuery(const void *a, const void *b) {
    const char *query = (const char *)a;
    const char *suffix = *(const char **)b;
    return strncmp(query, suffix, strlen(query));  // �N�G��������̒���������r
}

// ������Ɛ擪nPrefixLength�̕���������ɃC���f�b�N�X���v�Z����֐�
int calculateIndex(const char *str, int nPrefixLength, int *charPresence, int baseSize) {
    int index = 0;  // �C���f�b�N�X�̏����l
    int base = 1;   // ���݂̌��̊�i1, baseSize, ...�j

    // nPrefixLength������������
    for (int i = 0; i < nPrefixLength; ++i) {
        if (str[i] == '\0') {
            break;  // �����񂪏I�[�ɓ��B�����ꍇ
        }

        // ���݂̕����̒l�icharPresence�Ŏ擾�j
        index += charPresence[str[i]] * base;
        base *= baseSize;  // ����g��
    }

    return index;
}

int main() {
    static char inputBuffer[INPUT_BUFFER_SIZE];   // ���͑S�̂��i�[����Œ蒷�o�b�t�@
    static char outputBuffer[OUTPUT_BUFFER_SIZE]; // �o�͓��e���i�[����Œ蒷�o�b�t�@
    static char *P[MAX_Q];                        // �N�G��������̎Q�Ɣz��i�ÓI�ϐ��j
    static int charPresence['z' + 1] = {0};       // �e�����̏o���t���O�z��
    char *T;                                      // �啶����T
    char *ptr = inputBuffer;                      // ���̓o�b�t�@����p�|�C���^
    char **suffixArray;                           // �T�t�B�b�N�X�z��
    int Q, n, outputIndex = 0;                    // �N�G�����AT�̒����A�o�̓o�b�t�@�̈ʒu
    int usedCharCount = 0;                        // �g�p����Ă��镶���̐�
    int nPrefixLength = 1;                        // �g�p����擪�������i�ŏI���ʁj
    int minPLength = MAX_P_LEN;                   // �N�G���̍ŒZ���̏����l
    int *indexArray;                              // �C���f�b�N�X���Ƃ̒l�����z��
    char *flagArray;                              // �C���f�b�N�X�����݂��邩���L�^����z��

    // �W�����͂��炷�ׂĂ�ǂݍ���
    size_t bytesRead = fread(inputBuffer, 1, INPUT_BUFFER_SIZE, stdin);
    inputBuffer[bytesRead] = '\n';  // �I�[�ɉ��s��ǉ�

    // T���擾�i�ŏ��̍s�j
    T = ptr;
    while (*ptr != '\n') ++ptr;
    *ptr++ = '\0';  // ���s��NULL�ɒu�������ɐi��

    // Q���擾�i2�s�ڂ̐����A�N�G�����j
    Q = atoi(ptr);
    while (*ptr != '\n') ++ptr;
    *ptr++ = '\0';  // ���s��NULL�ɒu�������ɐi��

    // �N�G�����擾���ĎQ�Ɣz��P�Ɋi�[
    for (int i = 0; i < Q; ++i) {
        P[i] = ptr;  // �N�G���̈ʒu��P�Ɋi�[
        int length = 0;
        while (*ptr != '\n') {
            ++ptr;
            ++length;  // �N�G���̒������v��
        }
        if (length < minPLength) {
            minPLength = length;  // �ŒZ�N�G�������X�V
        }
        *ptr++ = '\0';  // ���s��NULL�ɒu�������ɐi��
    }

    // T��P�Ɋ܂܂�镶�����`�F�b�N���AcharPresence�z��ɃZ�b�g
    for (int i = 0; T[i] != '\0'; ++i) {
        charPresence[T[i]] = 1;  // T�Ɋ܂܂�镶�����Z�b�g
    }
    for (int i = 0; i < Q; ++i) {
        for (int j = 0; P[i][j] != '\0'; ++j) {
            charPresence[P[i][j]] = 1;  // P�Ɋ܂܂�镶�����Z�b�g
        }
    }

    // T��P�Ɋ܂܂�镶���𐔒l�ɕϊ�����e�[�u�����\�z
    int value = 1;  // ���l���J�n�l�i= �g�p�ςݕ����̃J�E���g + 1�j
    for (int i = 0; i <= 'z'; ++i) {  // 'z'�̕����R�[�h�܂Ń��[�v
        if (charPresence[i] == 1) {
            charPresence[i] = value++;  // 1���Z�b�g����Ă���v�f�ɔԍ�������U��
            ++usedCharCount;  // �g�p���ꂽ�������J�E���g
        }
    }

    // ����v�Z
    int baseSize = usedCharCount + 1;

    // �擪n�������������ɂ��邩���肷��
    int possiblePatterns = baseSize;  // �g�p����Ă��镶����� + 0�i�Ȃ��j
    nPrefixLength = 1;                // �����l�F�擪1����
    while (possiblePatterns <= MAX_INDEX_SIZE) {
        possiblePatterns *= baseSize;  // �p�^�[�������g��
        if (possiblePatterns > MAX_INDEX_SIZE) break;
        nPrefixLength++;  // �擪�������𑝂₷
    }

    // minPLength��24���傫���ꍇ�̓��ʏ���
    if (minPLength > 24) {
        // �@ baseSize��-1����
        baseSize -= 1;

        // �A charPresence�̒l��usedCharCount�ƈ�v���Ă���ꍇ�A0�ɕύX
        for (int i = 0; i <= 'z'; ++i) {
            if (charPresence[i] == usedCharCount) {
                charPresence[i] = 0;
            }
        }

        // �B nPrefixLength��19�ɐݒ肷��
        nPrefixLength = 19;
    }

    // �C���f�b�N�X�̔z����쐬
    int totalIndexes = 1;  // �C���f�b�N�X�z��̑��v�f��
    for (int i = 0; i < nPrefixLength; ++i) {
        totalIndexes *= baseSize;
    }
    indexArray = (int *)calloc(totalIndexes, sizeof(int));  // ���ׂ�0�ŏ�����
    flagArray = (char *)calloc(totalIndexes, sizeof(char)); // �V���ȃt���O�z��i���ׂ�0�ŏ������j

    // P�̕���������ƂɃC���f�b�N�X���v�Z���AindexArray�ɒl��ݒ�
    for (int i = 0; i < Q; ++i) {
        int index = calculateIndex(P[i], nPrefixLength, charPresence, baseSize);
        if (strlen(P[i]) <= nPrefixLength) {
            if (indexArray[index] < 2) indexArray[index] = 1;  // P�̒�������r���镶�����ȉ�
        } else {
            indexArray[index] = 2;  // P�̒�������r���镶�������傫��
        }
    }

    // T�̒����𑪒�
    n = strlen(T);

    // �T�t�B�b�N�X�z����\�z
    suffixArray = (char **)malloc(n * sizeof(char *));  // T�̕����񒷂Ń������m��
    int suffixCount = 0;  // ���ۂɒǉ����ꂽ�T�t�B�b�N�X�̐�
    char *previousSuffix = NULL;  // ���O�̃T�t�B�b�N�X������ւ̃|�C���^

    for (int i = 0; i <= n - minPLength; ++i) {  // T�̒�������ŒZP�̒������������͈͂Ń��[�v
        // T�̕���������1�O�ƈ�v���Ă���ꍇ�̓X�L�b�v
        if (previousSuffix != NULL &&
            strncmp(previousSuffix, &T[i], MAX_P_LEN) == 0) {
            continue;  // �d�����Ă���ꍇ�̓X�L�b�v
        }

        int valid = 0;  // �C���f�b�N�X���L�����𔻒�
        int index = 0;  // �ݐϓI�ɃC���f�b�N�X���v�Z�i�ŏ���0�j
        int base = 1;

        // 1�`nPrefixLength�̃��[�v�icalculateIndex���g�킸�ɗݐόv�Z�𒼐ڎ��s�j
        for (int j = 0; j < nPrefixLength; ++j) {
            if (T[i + j] == '\0') break;  // �����񂪏I�[�ɓ��B�����ꍇ

            index += charPresence[T[i + j]] * base;
            base *= baseSize;

            // indexArray���m�F
            if (indexArray[index] == 1) {
                flagArray[index] = 1;  // �t���O�z���1���Z�b�g
                valid = 0;            // valid���X�V���Ȃ�
            } else if (indexArray[index] == 2) {
                flagArray[index] = 1;  // �t���O�z���1���Z�b�g
                valid = 1;            // valid��1�ɐݒ�
            }
        }

        if (valid) {
            // �T�t�B�b�N�X�z��ɒǉ�
            suffixArray[suffixCount++] = &T[i];
            previousSuffix = &T[i];  // ���O�̃T�t�B�b�N�X���X�V
        }
    }

    // �T�t�B�b�N�X�z����k���i�K�v�ȗv�f���ɍĊ��蓖�āj
    suffixArray = (char **)realloc(suffixArray, suffixCount * sizeof(char *));

    // �T�t�B�b�N�X�z����\�[�g�i�������A�ő�MAX_P_LEN�܂Ŕ�r�j
    qsort(suffixArray, suffixCount, sizeof(char *), compareSuffix);

    // �N�G�����������ďo�̓o�b�t�@�Ɍ��ʂ��i�[
    for (int i = 0; i < Q; ++i) {
        int index = calculateIndex(P[i], nPrefixLength, charPresence, baseSize);
        if (flagArray[index] == 1) {
            if (strlen(P[i]) <= nPrefixLength) {
                outputBuffer[outputIndex++] = '1';  // ���̏�Ō��ʂ��o��
            } else {
                // ��r���镶�����𒴂���ꍇ��bsearch���g�p
                if (bsearch(P[i], suffixArray, suffixCount, sizeof(char *), compareQuery)) {
                    outputBuffer[outputIndex++] = '1';
                } else {
                    outputBuffer[outputIndex++] = '0';
                }
            }
        } else {
            outputBuffer[outputIndex++] = '0';  // �t���O�z��Ŗ��o�^�̏ꍇ
        }
        outputBuffer[outputIndex++] = '\n';  // ���s��ǉ�
    }

    // ���ʂ�W���o�͂�1��ŏ����o��
    fwrite(outputBuffer, 1, outputIndex, stdout);

    return 0;  // ����I��
}
