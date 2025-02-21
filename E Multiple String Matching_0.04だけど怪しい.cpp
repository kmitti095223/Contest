#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
#define PREFIX_INDEX_SIZE (256*256*256) // 先頭4文字のインデックス用
//#define PREFIX_INDEX_SIZE (63*63*63*63) // 先頭4文字のインデックス用

// 入力バッファ（全データを一括読み込み）
char inputBuffer[INPUT_BUFFER_SIZE];

// 出力バッファ（高速出力用）
char outputBuffer[OUTPUT_BUFFER_SIZE];
int outputBufferPosition = 0;

// サフィックス配列（検索対象の接尾辞を格納）
char* suffixArray[MAX_TEXT_LENGTH];
int suffixArraySize = 0;

// クエリ文字列の配列
char* P[MAX_QUERY_COUNT];

// 先頭文字チェック用の配列（インデックス化して検索高速化）
int prefixLookupTable[PREFIX_INDEX_SIZE];
int prefixFlagTable[PREFIX_INDEX_SIZE];

// 文字インデックス計算用のパラメータ
int characterBase = 62; // 文字を数値化する基数
int maxPrefixLength = 4; // 最大チェック長

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
inline int prefixCompare(const void* key, const void* element) {
    return strncmp((const char*)key, *(const char**)element, strlen((const char*)key));
}

// 高速入力処理（改行区切りの文字列を取得）
char* fastReadLine(char** cursor) {
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

       int length = strlen(P[i]);
        if (length < len_min)
            len_min = length;
        
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
    if (uniqueCharacterCount <= 2) {
        characterBase = 3;
        maxPrefixLength = 14;

        if (len_min > maxPrefixLength) {
            for (int i = '0'; i <= 'z'; i++) {
                if (characterMap[i] == 2) {
                    characterMap[i] = 0;
                }
            }
            characterBase = 2;
            maxPrefixLength = 21; // これがベスト だけど、デバッグ実行すると例外
        }
    }

    // クエリのプレフィックスをインデックス化
    for (int i = 0; i < Q; i++) {
        int index = computePrefixIndex(P[i]);
        int length = countCharacters(P[i]);
        if (length > maxPrefixLength)
            prefixLookupTable[index] = 2;
        else if (prefixLookupTable[index] == 0)
            prefixLookupTable[index] = 1;
    }

    int textLength = strlen(T);

    // サフィックス配列の作成（検索対象の接尾辞を収集）
    for (int i = 0; i < textLength- len_min+1; i++) {

        if (i != 0)
            if (strncmp(&T[i], &T[i - 1], 1000) == 0)
                continue;

        char* currentSuffix = &T[i];
        int prefixIndices[16] = { 0 };
        int flag = 0;
        int tempIndex = 0;

        for (int j = 0; j < maxPrefixLength; j++) {
            if (T[i + j] == 0)
                break;

            prefixIndices[j] = tempIndex * characterBase + characterMap[T[i + j]];
            tempIndex = prefixIndices[j];
            if (prefixLookupTable[prefixIndices[j]])
                prefixFlagTable[prefixIndices[j]] = 1;

            if (prefixLookupTable[prefixIndices[j]] == 2)
                flag = 1;
        }

        if (flag) {
            suffixArray[suffixArraySize++] = currentSuffix;
#ifdef WINDOWS_DEBUG
            debugMatchCount++;
#endif
        }
    }

    // サフィックス配列をソート
    qsort(suffixArray, suffixArraySize, sizeof(char*), compareSuffixes);

    // クエリごとの検索処理
    for (int i = 0; i < Q; i++) {
        int index = computePrefixIndex(P[i]);

//        if (strcmp(P[i], "oo") == 0)
//            i = i;

        if (prefixFlagTable[index]) {
            int length = countCharacters(P[i]);
            if (length > maxPrefixLength) {
#ifdef WINDOWS_DEBUG
                debugBinarySearchCount++;
#endif
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
