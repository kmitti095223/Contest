#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cmath>

#ifdef WINDOWS_DEBUG
#include <windows.h>
LARGE_INTEGER frequency, startTime, endTime;
int debugMatchCount = 0, debugBinarySearchCount = 0;
#endif

#define MAX_TEXT_LENGTH 1000000
#define MAX_PATTERN_LENGTH 1001
#define MAX_QUERY_COUNT 10000
#define INPUT_BUFFER_SIZE 20000000
#define OUTPUT_BUFFER_SIZE 100000

// 入力バッファ（全データを一括読み込み）
char inputBuffer[INPUT_BUFFER_SIZE];

// 出力バッファ（高速出力用）
char outputBuffer[OUTPUT_BUFFER_SIZE];
int outputBufferPosition = 0;

// サフィックス配列（検索対象の接尾辞を格納）
char** suffixArray;
int suffixArraySize = 0;

// クエリ文字列の配列
char* P[MAX_QUERY_COUNT];

int P2[MAX_QUERY_COUNT];
int P_Len[MAX_QUERY_COUNT];

// 先頭文字チェック用の配列（インデックス化して検索高速化）
typedef struct {
    char P_Exists;
    char prefixFlagTable;
    short WK;
} PrefixData;
PrefixData* data;

// 文字インデックス計算用のパラメータ
int characterBase = -1; // 文字を数値化する基数
int maxPrefixLength = -1; // 最大チェック長

// 文字列の長さを最大5文字までカウントする関数
inline int countCharacters(const char* str) {
    int count = 0;
    while (count <= maxPrefixLength && str[count] != '\0') {
        count++;
    }
    return count;
}

// 文字のマッピングテーブル
int characterMap[256] = { 0 };

// サフィックス比較関数（先頭1000文字まで比較）
inline int compareSuffixes(const void* a, const void* b) {
    return strncmp(*(const char**)a, *(const char**)b, 1000);
}

// 二分探索用の比較関数（前方一致）
int COMP_LEN;
inline int prefixCompare(const void* key, const void* element) {
    return strncmp((const char*)key, *(const char**)element, COMP_LEN);
    //return strncmp((const char*)key, *(const char**)element, strlen((const char*)key));
}

// 高速入力処理（改行区切りの文字列を取得）
inline char* fastReadLine(char** cursor) {
    char* lineStart = *cursor;
    while (**cursor != '\n' && **cursor != '\0') {
        (*cursor)++;
    }
    if (**cursor == '\n') {
        **cursor = '\0';
        (*cursor)++;
    }
    return lineStart;
}

// 文字列のインデックスを計算する関数（先頭4文字を数値化）
inline int computePrefixIndex(const char* str) {
    int index = 0;
    int length = countCharacters(str);
    for (int i = 0; i < length && i < maxPrefixLength; i++) {
        index = index * characterBase + characterMap[str[i]];
    }
    return index;
}


int fastPower(int base, int exponent) {
    int result = 1;
    while (exponent > 0) {
        if (exponent % 2 == 1) { // 奇数なら掛ける
            result *= base;
        }
        base *= base; // 底を2乗
        exponent /= 2; // 指数を半減
    }
    return result;
}

int main() {
#ifdef WINDOWS_DEBUG
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&startTime);
#endif

    // 入力データを一括読み込み
    fread(inputBuffer, 1, INPUT_BUFFER_SIZE, stdin);
    char* cursor = inputBuffer;

    // クエリの先頭文字チェック用
    char characterPresence['z' + 1] = { 0 };

    while (1) {
        if (*cursor == '\n')
            break;
        characterPresence[*cursor] = 1;
        cursor++;
    }
    cursor++;
    while (1) {
        if (*cursor == '\n')
            break;
        cursor++;
    }
    cursor++;
    while (1) {
        if (*cursor == 0)
            break;
        characterPresence[*cursor] = 1;
        cursor++;
    }

    cursor = inputBuffer;

    // テキストデータを取得
    char* T = fastReadLine(&cursor);
    int Q = atoi(fastReadLine(&cursor));

    int len_min = 10000;

    for (int i = 0; i < Q; i++) {
        P[i] = fastReadLine(&cursor);
        //    characterPresence[*P[i]] = 1;

        P_Len[i] = strlen(P[i]);
        if (P_Len[i] < len_min)
            len_min = P_Len[i];

    }

    // 文字の種類数をカウントし、マッピングテーブルを作成
    int uniqueCharacterCount = 0;
    for (int i = '0'; i <= 'z'; i++) {
        if (characterPresence[i]) {
            uniqueCharacterCount++;
            characterMap[i] = uniqueCharacterCount;
        }
    }

    // 文字の種類数が少ない場合、基数を変更
    characterBase = uniqueCharacterCount + 1; // 文字を数値化する基数
    maxPrefixLength = (int)(std::log(256 * 256 * 64) / std::log(characterBase));; // 最大チェック長

    if (uniqueCharacterCount <= 2) {

        if (len_min > maxPrefixLength) {
            characterBase = 2;
            //maxPrefixLength = 19; // 18973
            maxPrefixLength = 20; // 9537
            // maxPrefixLength = 21; // 4798
            // maxPrefixLength = 22; // 2372
            // maxPrefixLength = 24; // 612
            // maxPrefixLength = 26; // 147
            for (int i = '0'; i <= 'z'; i++) {
                if (characterMap[i] == 2) {
                    characterMap[i] = 0;
                }
            }
        }
    }

    int PREFIX_INDEX_SIZE = fastPower(characterBase, maxPrefixLength);
    suffixArray = new char* [PREFIX_INDEX_SIZE];
    data = new   PrefixData[PREFIX_INDEX_SIZE];
    memset(data, 0, sizeof(PrefixData) * PREFIX_INDEX_SIZE);

    memset(P2, 0, sizeof(P2));

    int* prefixIndices = new int[maxPrefixLength];

    // クエリのプレフィックスをインデックス化
    for (int i = 0; i < Q; i++) {
        int index = computePrefixIndex(P[i]);
        int length = (P_Len[i] >= maxPrefixLength ? maxPrefixLength + 1 : maxPrefixLength);
        if (length > maxPrefixLength) {

            data[index].P_Exists = 2;

            if (data[index].WK == -1)
                data[index].WK = (short)i;
            else
                data[index].WK = -2;

        }
        else if (data[index].P_Exists == 0)
            data[index].P_Exists = 1;
    }

    int textLength = strlen(T);

    // サフィックス配列の作成（検索対象の接尾辞を収集）
    for (int i = 0; i < textLength - len_min + 1; i++) {

        if (uniqueCharacterCount <= 2)
            if (i != 0)
                if (strncmp(&T[i], &T[i - 1], 1000) == 0)
                    continue;

        char* currentSuffix = &T[i];

        // 上でnew
        // int *prefixIndices = new int[maxPrefixLength];

        // 無くても大丈夫
        // memset(prefixIndices, 0, sizeof(int)* maxPrefixLength);

        int flag = 0;
        int tempIndex = 0;

        //if (strncmp(currentSuffix, "oo", 2) == 0)
        //    i = i;

        for (int j = 0; j < maxPrefixLength; j++) { // このforループが遅い
            if (T[i + j] == 0)
                break;

            tempIndex = tempIndex * characterBase + characterMap[T[i + j]];
            if (data[tempIndex].P_Exists) {

                //if (tempIndex == 52)
                //    i = i;

                data[tempIndex].prefixFlagTable = 1;


                if (data[tempIndex].P_Exists == 2)
                    flag = 1;
            }


            prefixIndices[j] = tempIndex;
        }


        if (flag) {
            int IndexEx = prefixIndices[maxPrefixLength - 1];
            if (data[IndexEx].WK >= 0) {
                // printf(P[WK[prefixIndices[maxPrefixLength - 1]]]);

                if (P2[data[IndexEx].WK] != 1) {

                    if (strncmp(P[data[IndexEx].WK], currentSuffix, P_Len[data[IndexEx].WK]) == 0)
                        P2[data[IndexEx].WK] = 1;
                    else {
                        P2[data[IndexEx].WK] = -1;
                    }
                }
            }
            else
            {
                if (data[IndexEx].WK == -2) {
                    suffixArray[suffixArraySize++] = currentSuffix;
#ifdef WINDOWS_DEBUG
                    debugMatchCount++;
#endif
                }
            }
        }
    }

    // サフィックス配列をソート
    qsort(suffixArray, suffixArraySize, sizeof(char*), compareSuffixes);

    // クエリごとの検索処理
    for (int i = 0; i < Q; i++) {
        int index = computePrefixIndex(P[i]);

        //if (strcmp(P[i], "iooi") == 0)
        //    i = i;

        if (P2[i] == 1)
        {
            outputBuffer[outputBufferPosition++] = '1';
        }
        else if (P2[i] == -1)
        {
            outputBuffer[outputBufferPosition++] = '0';
        }
        else {
            if (data[index].prefixFlagTable) {
                int length = (P_Len[i] >= maxPrefixLength ? maxPrefixLength + 1 : maxPrefixLength);
                if (length > maxPrefixLength) {
#ifdef WINDOWS_DEBUG
                    debugBinarySearchCount++;
#endif
                    COMP_LEN = P_Len[i];
                    char** result = (char**)bsearch(P[i], suffixArray, suffixArraySize, sizeof(char*), prefixCompare);
                    outputBuffer[outputBufferPosition++] = (result != NULL) ? '1' : '0';
                }
                else {
                    outputBuffer[outputBufferPosition++] = '1';
                }
            }
            else {
                outputBuffer[outputBufferPosition++] = '0';
            }
        }
        outputBuffer[outputBufferPosition++] = '\n';
    }

    // 出力バッファを一括出力
    fwrite(outputBuffer, 1, outputBufferPosition, stdout);

#ifdef WINDOWS_DEBUG
    QueryPerformanceCounter(&endTime);
    printf("Execution Time: %dms\n", (int)((double)(endTime.QuadPart - startTime.QuadPart) * 1000.0 / frequency.QuadPart));
    printf("DebugMatchCount=%d  DebugBinarySearchCount=%d\n", debugMatchCount, debugBinarySearchCount);
    while (1) getchar();
#endif
    return 0;
}
