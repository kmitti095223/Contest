#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef WINDOWS_DEBUG
#include <windows.h>
LARGE_INTEGER m_Freq, m_Start, m_End;
int nCount1 = 0, nCount2 = 0;
#endif

#define MAX_LENGTH 1000000
#define MAX_QUERY_LENGTH 1001
#define MAX_QUERY_COUNT 10000 // 質問の最大件数
#define BUFFER_SIZE 20000000  // 標準入力バッファの長さ（十分な長さを確保）
#define OUTPUT_BUFFER_SIZE 100000      // 標準出力バッファの長さ
#define INDEX_SIZE 14776336   // 先頭文字チェック用配列のサイズ（62^4）

// 入力バッファ（1度で全てのデータを読み込む）
char inputBuffer[BUFFER_SIZE];

// 出力バッファ
char outputBuffer[OUTPUT_BUFFER_SIZE];
int outputPos = 0;

// サフィックス配列（最大1000000要素）
char* suffixArray[MAX_LENGTH];
int suffixArraySize = { 0 }; // サフィックス配列の実際のサイズ

char* P[MAX_QUERY_COUNT];

// char* PiArray[MAX_QUERY_COUNT];    // Piの先頭アドレスを格納する配列

int prefixCheck[INDEX_SIZE];  // 先頭文字チェック用配列（62^4）
int prefixCheck2[INDEX_SIZE];  // 先頭文字チェック用配列（62^4）

// 文字数を最大5文字までカウントする関数
inline int countChars(const char* str) {
    int count = 0;
    while (count < 5 && str[count] != '\0') {
        count++;
    }
    return  count;
}

// 文字を数値（0～61）に変換する関数
inline int charToidx(char c) {
    if ('A' <= c && c <= 'Z') {
        return c - 'A'; // 'A' は 0～25
    }
    else if ('a' <= c && c <= 'z') {
        return c - 'a' + 26; // 'a' は 26～51
    }
    else { // '0'～'9' の場合
        return c - '0' + 52; // '0' は 52～61
    }
}

// 辞書順比較用の関数（先頭1000文字まで比較）
inline int compare(const void* a, const void* b) {
    const char** ptrA = (const char**)a;
    const char** ptrB = (const char**)b;

    // 先頭1000文字で比較を行う
    return strncmp(*ptrA, *ptrB, 1000);
}

// bsearch用の比較関数（前方一致を考慮）
inline int prefixCompare(const void* key, const void* element) {
    const char* p = (const char*)key;
    const char* suffix = *(const char**)element;

    // Pの長さだけ比較を行う
    return strncmp(p, suffix, strlen(p));
}


// 高速な文字列処理で入力を取得
char* fastReadLine(char** cursor) {
    char* lineStart = *cursor;
    while (**cursor != '\n' && **cursor != '\0') {
        (*cursor)++;
    }
    if (**cursor == '\n') {
        **cursor = '\0'; // 改行文字を終端文字に置換
        (*cursor)++;
    }
    return lineStart;
}

// 文字列のインデックスを計算する関数
inline int calculateIndex(const char* str, int n = 4) {
    const int BASE = 62; // 基数
    int index = 0;
    int length = countChars(str);

    for (int i = 0; i < length && i < n; i++) {
        index = index * BASE + charToidx(str[i]);
    }

    return index;
}

int main() {

#ifdef WINDOWS_DEBUG // デバッグ用処理時間計測
    QueryPerformanceFrequency(&m_Freq);
    QueryPerformanceCounter(&m_Start);
#endif

    // 入力データを一度に読み込む
    fread(inputBuffer, 1, BUFFER_SIZE, stdin);
    char* cursor = inputBuffer;

    char* T = fastReadLine(&cursor);
    int n = strlen(T);

    // 質問数を取得
    int Q = atoi(fastReadLine(&cursor)); // 高速に整数を取得


    for (int i = 0; i < Q; i++) {
        P[i] = fastReadLine(&cursor); // 各クエリを高速に取得

        // 先頭4文字を元にインデックスを計算してチェックフラグを立てる
        int index = calculateIndex(P[i]);

        int v = countChars(P[i]);
        if (v>=5)
            prefixCheck[index] = 2;
        else
            prefixCheck[index] = 1;

    }


    // サフィックス配列の構築
    for (int i = 0; i < n; i++) {
        // 現在の部分文字列
        char* currentSuffix = &T[i];


        int index1 = calculateIndex(&T[i], 1);
        int index2 = calculateIndex(&T[i], 2);
        int index3 = calculateIndex(&T[i], 3);
        int index4 = calculateIndex(&T[i], 4);

        if (prefixCheck[index1]) prefixCheck2[index1] = 1;
        if (prefixCheck[index2]) prefixCheck2[index2] = 1;
        if (prefixCheck[index3]) prefixCheck2[index3] = 1;
        if (prefixCheck[index4]) prefixCheck2[index4] = 1;

        if (prefixCheck[index1]==2 || prefixCheck[index2] == 2 || prefixCheck[index3] == 2 || prefixCheck[index4] == 2)
        {
            // 配列に追加するのは初回または1000文字までの比較で異なる場合のみ
            if (suffixArraySize == 0 || strncmp(suffixArray[suffixArraySize - 1], currentSuffix, 1000) != 0) {
                suffixArray[suffixArraySize++] = currentSuffix;
                    nCount1++;
            }
        }
    }

    // サフィックス配列のソート（先頭1000文字で比較）
    qsort(suffixArray, suffixArraySize, sizeof(char*), compare);

    for (int i = 0; i < Q; i++) {

        int index1 = calculateIndex(P[i]);

        if (prefixCheck2[index1])
        {
            int len = countChars(P[i]);
            if (len == 5) {
                // bsearchを使用して二分探索
                char** result = (char**)bsearch(P[i], suffixArray, suffixArraySize, sizeof(char*), prefixCompare);
                if (result != NULL) {
                    outputBuffer[outputPos++] = '1';
                }
                else {
                    outputBuffer[outputPos++] = '0';
                }
                // nCount2++;
            }
            else
            {
                outputBuffer[outputPos++] = '1';
            }
        }
        else
        {
            outputBuffer[outputPos++] = '0';
        }
        outputBuffer[outputPos++] = '\n';
    }

    fwrite(outputBuffer, 1, outputPos, stdout);

#ifdef WINDOWS_DEBUG // デバッグ用処理時間計測
    QueryPerformanceCounter(&m_End);
    printf("実行時間:%dms\n", (int)((double)(m_End.QuadPart - m_Start.QuadPart) * 1000.0 / m_Freq.QuadPart));
    printf("nCount1=%d  nCount2=%d\n", nCount1, nCount2);
    while (1) getchar();
#endif
    return 0;
}