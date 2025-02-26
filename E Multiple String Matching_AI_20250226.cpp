#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INPUT_BUFFER_SIZE 12000000  // 入力バッファのサイズ（12MB）
#define OUTPUT_BUFFER_SIZE 1200000  // 出力バッファのサイズ（1.2MB）
#define MAX_T_LEN 1000000           // Tの最大長
#define MAX_P_LEN 1000              // Pの最大長
#define MAX_Q 10000                 // クエリの最大数
#define MAX_INDEX_SIZE 16777216     // 配列の最大インデックスサイズ（256×256×256）

// サフィックス配列用の比較関数（最大MAX_P_LENまで比較）
int compareSuffix(const void *a, const void *b) {
    const char *suffixA = *(const char **)a;
    const char *suffixB = *(const char **)b;
    return strncmp(suffixA, suffixB, MAX_P_LEN);  // 最大MAX_P_LEN文字まで比較
}

// bsearch用のクエリ比較関数（クエリ文字列とサフィックスを比較、必要文字数だけ比較）
int compareQuery(const void *a, const void *b) {
    const char *query = (const char *)a;
    const char *suffix = *(const char **)b;
    return strncmp(query, suffix, strlen(query));  // クエリの長さだけ比較
}

// 文字列と先頭nPrefixLengthの文字数を基にインデックスを計算する関数
int calculateIndex(const char *str, int nPrefixLength, int *charPresence, int usedCharCount) {
    int index = 0;  // インデックスの初期値
    int base = 1;   // 現在の桁の基数（1, (usedCharCount+1), ...）

    // nPrefixLength文字分を処理
    for (int i = 0; i < nPrefixLength; ++i) {
        if (str[i] == '\0') {
            break;  // 文字列が終端に到達した場合
        }

        // 現在の文字の値（charPresenceで取得）
        index += charPresence[str[i]] * base;
        base *= (usedCharCount + 1);  // 基数を拡張
    }

    return index;
}

int main() {
    static char inputBuffer[INPUT_BUFFER_SIZE];   // 入力全体を格納する固定長バッファ
    static char outputBuffer[OUTPUT_BUFFER_SIZE]; // 出力内容を格納する固定長バッファ
    static char *P[MAX_Q];                        // クエリ文字列の参照配列（静的変数）
    static int charPresence['z' + 1] = {0};       // 各文字の出現フラグ配列
    char *T;                                      // 主文字列T
    char *ptr = inputBuffer;                      // 入力バッファ操作用ポインタ
    char **suffixArray;                           // サフィックス配列
    int Q, n, outputIndex = 0;                    // クエリ数、Tの長さ、出力バッファの位置
    int usedCharCount = 0;                        // 使用されている文字の数
    int nPrefixLength = 1;                        // 使用する先頭文字数（最終結果）
    int *indexArray;                              // インデックスごとの値を持つ配列

    // 標準入力からすべてを読み込む
    size_t bytesRead = fread(inputBuffer, 1, INPUT_BUFFER_SIZE, stdin);
    inputBuffer[bytesRead] = '\n';  // 終端にNULLの代わりに改行文字を追加

    // Tを取得（最初の行）
    T = ptr;
    while (*ptr != '\n') ++ptr;
    *ptr++ = '\n';  // 改行をNULL文字に変換し次に進む

    // Qを取得（2行目の整数、クエリ数）
    Q = atoi(ptr);
    while (*ptr != '\n') ++ptr;
    ++ptr;

    // クエリを取得して参照配列Pに格納
    for (int i = 0; i < Q; ++i) {
        P[i] = ptr;  // クエリの位置をPに格納
        while (*ptr != '\n') ++ptr;
        *ptr++ = '\n';  // 改行をNULL文字に変換し次に進む
    }

    // TとPに含まれる文字をチェックし、charPresence配列にセット
    for (int i = 0; T[i] != '\0'; ++i) {
        charPresence[T[i]] = 1;  // Tに含まれる文字をセット
    }
    for (int i = 0; i < Q; ++i) {
        for (int j = 0; P[i][j] != '\0'; ++j) {
            charPresence[P[i][j]] = 1;  // Pに含まれる文字をセット
        }
    }

    // TとPに含まれる文字を数値に変換するテーブルを構築
    int value = 1;  // 数値化開始値（= 使用済み文字のカウント + 1）
    for (int i = 0; i <= 'z'; ++i) {  // 'z'の文字コードまでループ
        if (charPresence[i] == 1) {
            charPresence[i] = value++;  // 1がセットされている要素に番号を割り振る
            ++usedCharCount;  // 使用された文字をカウント
        }
    }

    // 先頭n文字を何文字にするか決定する
    int possiblePatterns = usedCharCount + 1;  // 使用されている文字種類 + 0（なし）
    nPrefixLength = 1;                         // 初期値：先頭1文字
    while (possiblePatterns <= MAX_INDEX_SIZE) {
        possiblePatterns *= (usedCharCount + 1);  // パターン数を拡張
        if (possiblePatterns > MAX_INDEX_SIZE) break;
        nPrefixLength++;  // 先頭文字数を増やす
    }

    // インデックスの配列を作成
    int totalIndexes = 1;  // インデックス配列の総要素数
    for (int i = 0; i < nPrefixLength; ++i) {
        totalIndexes *= (usedCharCount + 1);
    }
    indexArray = (int *)calloc(totalIndexes, sizeof(int));  // すべて0で初期化

    // Pの文字列をもとにインデックスを計算し、indexArrayに値を設定
    for (int i = 0; i < Q; ++i) {
        int index = calculateIndex(P[i], nPrefixLength, charPresence, usedCharCount);
        indexArray[index] = 1;  // インデックス位置に1をセット
    }

    // Tの長さを測定
    n = strlen(T);

    // サフィックス配列を構築
    suffixArray = (char **)malloc(n * sizeof(char *));
    for (int i = 0; i < n; ++i) {
        suffixArray[i] = &T[i];
    }

    // サフィックス配列をソート（辞書順、最大MAX_P_LENまで比較）
    qsort(suffixArray, n, sizeof(char *), compareSuffix);

    // クエリを処理して出力バッファに結果を格納
    for (int i = 0; i < Q; ++i) {
        if (bsearch(P[i], suffixArray, n, sizeof(char *), compareQuery)) {
            outputBuffer[outputIndex++] = '1';
        } else {
            outputBuffer[outputIndex++] = '0';
        }
        outputBuffer[outputIndex++] = '\n';  // 改行を追加
    }

    // 結果を標準出力に1回で書き出す
    fwrite(outputBuffer, 1, outputIndex, stdout);

    // 動的メモリ解放
    free(indexArray);
    free(suffixArray);

    return 0;  // 正常終了
}