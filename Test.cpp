#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cmath>

#define INPUT_BUFFER_SIZE 12000000  // 入力バッファのサイズ（12MB）
#define OUTPUT_BUFFER_SIZE 1200000  // 出力バッファのサイズ（1.2MB）
#define MAX_T_LEN 1000000           // Tの最大長
#define MAX_P_LEN 1000              // Pの最大長（比較する最大文字数）
#define MAX_Q 10000                 // クエリの最大数
#define MAX_INDEX_SIZE 16777216     // 配列の最大インデックスサイズ（256×256×256）

#ifdef WINDOWS_DEBUG
#include <windows.h>
LARGE_INTEGER frequency, startTime, endTime;
int debugMatchCount = 0, debugBinarySearchCount = 0;
#endif

// サフィックス配列用の比較関数（最大MAX_P_LENまで比較）
int compareSuffix(const void* a, const void* b) {
    const char* suffixA = *(const char**)a;
    const char* suffixB = *(const char**)b;
    return strncmp(suffixA, suffixB, MAX_P_LEN);  // 最大MAX_P_LEN文字まで比較
}

// bsearch用のクエリ比較関数（クエリ文字列とサフィックスを比較、必要文字数だけ比較）
int compareQuery(const void* a, const void* b) {
    const char* query = (const char*)a;
    const char* suffix = *(const char**)b;
    return strncmp(query, suffix, strlen(query));  // クエリ文字列の長さだけ比較
}


// 文字列と先頭nPrefixLengthの文字数を基にインデックスを計算する関数
int calculateIndex(const char* str, int nPrefixLength, int* charPresence, int bit_per_char) {
    int index = 0;  // インデックスの初期値
    
    index = charPresence[str[0]];

    int i;
    for (i = 1; i < nPrefixLength; ++i) {
        if (str[i] == '\0') {
            break;  // 文字列が終端に到達した場合
        }

        // 現在の文字の値（charPresenceで取得）
        index = index << bit_per_char;  // 基数を拡張
        index += charPresence[str[i]] ;
    }
    //for (; i < nPrefixLength; ++i) {       
    //    index = index << bit_per_char;  // 基数を拡張
    //}


    return index;
}

int main() {

#ifdef WINDOWS_DEBUG
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&startTime);
#endif

    static char inputBuffer[INPUT_BUFFER_SIZE];   // 入力全体を格納する固定長バッファ
    static char outputBuffer[OUTPUT_BUFFER_SIZE]; // 出力内容を格納する固定長バッファ
    static char* P[MAX_Q];                        // クエリ文字列の参照配列（静的変数）
    static int charPresence['z' + 1] = { 0 };       // 各文字の出現フラグ配列
    char* T;                                      // 主文字列T
    char* ptr = inputBuffer;                      // 入力バッファ操作用ポインタ
    char** suffixArray;                           // サフィックス配列
    int Q, TLen, outputIndex = 0;                 // クエリ数、Tの長さ、出力バッファの位置
    int usedCharCount = 0;                        // 使用されている文字の数
    int nPrefixLength = 1;                        // 使用する先頭文字数（最終結果）
    int minPLength = MAX_P_LEN;                   // クエリの最短長の初期値
    int* indexArray;                              // インデックスごとの値を持つ配列
    char* flagArray;                              // インデックスが存在するかを記録する配列

    // 標準入力からすべてを読み込む
    size_t bytesRead = fread(inputBuffer, 1, INPUT_BUFFER_SIZE, stdin);
    inputBuffer[bytesRead] = '\n';  // 終端に改行を追加

    // Tを取得（最初の行）
    T = ptr;
    while (*ptr != '\n') ++ptr;
    *ptr++ = '\0';  // 改行をNULLに置換し次に進む

    // Qを取得（2行目の整数、クエリ数）
    Q = atoi(ptr);
    while (*ptr != '\n') ++ptr;
    *ptr++ = '\0';  // 改行をNULLに置換し次に進む

    // クエリを取得して参照配列Pに格納
    for (int i = 0; i < Q; ++i) {
        P[i] = ptr;  // クエリの位置をPに格納
        int length = 0;
        while (*ptr != '\n') {
            ++ptr;
            ++length;  // クエリの長さを計測
        }
        if (length < minPLength) {
            minPLength = length;  // 最短クエリ長を更新
        }
        *ptr++ = '\0';  // 改行をNULLに置換し次に進む
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

    int bit_per_char = 0;

    // 基数を計算



    if (minPLength > 20) 
        nPrefixLength = 20;

    int baseSize;
    if (usedCharCount <= 2)
    {
        baseSize = 2;
        bit_per_char = 2;
        
        if (minPLength > 20) {
            nPrefixLength = 20;
            bit_per_char = 1;

            for (int i = 0; i <= 'z'; ++i) {
                if (charPresence[i] == usedCharCount) {
                    charPresence[i] = 0;
                }
            }
        }
    }
    else
    {
        baseSize = usedCharCount + 1;
        bit_per_char = ((usedCharCount + 1) <= 1) ? 1 : static_cast<int>(std::ceil(std::log2((usedCharCount + 1))));
        nPrefixLength = 24 / bit_per_char;

        bit_per_char = 6;
        nPrefixLength = 3;


        // 先頭n文字を何文字にするか決定する
        // int possiblePatterns = baseSize;  // 使用されている文字種類 + 0（なし）
        //nPrefixLength = 1;                // 初期値：先頭1文字
        //while (possiblePatterns <= MAX_INDEX_SIZE) {
        //    possiblePatterns *= baseSize;  // パターン数を拡張
        //    if (possiblePatterns > MAX_INDEX_SIZE) break;
        //    nPrefixLength++;  // 先頭文字数を増やす
        //}
    }

    // インデックスの配列を作成
    int totalIndexes = (int) pow(2, bit_per_char*nPrefixLength);  // インデックス配列の総要素数

    indexArray = (int*)calloc(totalIndexes, sizeof(int));  // すべて0で初期化
    flagArray = (char*)calloc(totalIndexes, sizeof(char)); // 新たなフラグ配列（すべて0で初期化）

    // Pの文字列をもとにインデックスを計算し、indexArrayに値を設定
    for (int i = 0; i < Q; ++i) {
        int index = calculateIndex(P[i], nPrefixLength, charPresence, bit_per_char);
        if (strlen(P[i]) <= nPrefixLength) {
            if (indexArray[index] < 2) indexArray[index] = 1;  // Pの長さが比較する文字数以下
        }
        else {
            indexArray[index] = 2;  // Pの長さが比較する文字数より大きい
        }
    }

    // Tの長さを測定
    TLen = strlen(T);

    // サフィックス配列を構築
    suffixArray = (char**)malloc(TLen * sizeof(char*));  // Tの文字列長でメモリ確保
    int suffixCount = 0;  // 実際に追加されたサフィックスの数
    char* previousSuffix = NULL;  // 直前のサフィックス文字列へのポインタ

    int mask = (1U << (bit_per_char* (nPrefixLength -1))) - 1;
    int start = ((minPLength-1) < (nPrefixLength - 1)) ? minPLength - 1 : nPrefixLength - 1;
    
    int prev_index = calculateIndex(T,nPrefixLength,charPresence,bit_per_char);
    prev_index = prev_index >> bit_per_char;

    int t_end = TLen - minPLength ;

    int set_flag = 1;

    for (int i = 0; i <= t_end; ++i) {  // Tの長さから最短Pの長さを引いた範囲でループ
        // Tの部分文字列が1つ前と一致している場合はスキップ
        if (previousSuffix != NULL &&
            strncmp(previousSuffix, &T[i], MAX_P_LEN) == 0) {
            continue;  // 重複している場合はスキップ
        }

        int valid = 0;  // インデックスが有効かを判定
        int index = 0;  // 累積的にインデックスを計算（最初は0）

        int counter = nPrefixLength;
        if (counter >= (TLen - i + 1)) {
            set_flag = 0;
        }
        if (counter >= (TLen - i)) {
            counter = TLen - i;
        } 

        if (set_flag) {
            index = prev_index & mask;
            index = index << bit_per_char;
            index += charPresence[T[i+ nPrefixLength -1]];
        }
        else {
            // index = index >> bit_per_char;
            index = prev_index;

            int mask2 = (1U << (bit_per_char * counter )) - 1;
            index = prev_index & mask2;

            index = index;
        }
        prev_index = index;

        for (int j = counter; j != start; --j) {

            // インデックスに基づく処理の順序を修正
            if (indexArray[index] == 2) {
                flagArray[index] = 1;
                valid = 1;
            }
            else if (indexArray[index] == 1) {
                flagArray[index] = 1;
            }

            index = index >> bit_per_char;

            // minPLength >= nPrefixLengthの場合は最後のループの処理のみ実行
//            if (minPLength >= nPrefixLength && j < nPrefixLength - 1) continue;

        }

        if (valid) {
            // サフィックス配列に追加
            suffixArray[suffixCount++] = &T[i];
            previousSuffix = &T[i];
        }
    }

    // サフィックス配列をソート（辞書順、最大MAX_P_LENまで比較）
    qsort(suffixArray, suffixCount, sizeof(char*), compareSuffix);

    // クエリを処理して出力バッファに結果を格納
    for (int i = 0; i < Q; ++i) {
        int index = calculateIndex(P[i], nPrefixLength, charPresence, bit_per_char);
        if (flagArray[index] == 1) {
            if (strlen(P[i]) <= nPrefixLength) {
                outputBuffer[outputIndex++] = '1';
            }
            else {
                if (bsearch(P[i], suffixArray, suffixCount, sizeof(char*), compareQuery)) {
                    outputBuffer[outputIndex++] = '1';
                }
                else {
                    outputBuffer[outputIndex++] = '0';
                }
            }
        }
        else {
            outputBuffer[outputIndex++] = '0';
        }
        outputBuffer[outputIndex++] = '\n';
    }

    // 結果を標準出力に1回で書き出す
    fwrite(outputBuffer, 1, outputIndex, stdout);


#ifdef WINDOWS_DEBUG
    QueryPerformanceCounter(&endTime);
    printf("Execution Time: %dms\n", (int)((double)(endTime.QuadPart - startTime.QuadPart) * 1000.0 / frequency.QuadPart));
    printf("DebugMatchCount=%d  DebugBinarySearchCount=%d\n", debugMatchCount, debugBinarySearchCount);
    while (1) getchar();
#endif
    return 0;
}