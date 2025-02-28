#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 必要なバッファサイズを確保
static char input_buffer[12000000];  // 入力データを一括格納（十分なサイズを確保）
static const char* suffix_array[1000000]; // サフィックス配列（最大Tの長さ）
static const char* P[10000];          // クエリ文字列のポインタ配列（最大10000個のクエリ）
static char output_buffer[100000];   // 出力バッファ（結果を一括出力）

// 二分探索用比較関数（Pの長さで比較）
int compare_query(const void* key, const void* suffix) {
    const char* query = *(const char**)key;
    const char* suffix_str = *(const char**)suffix;
    size_t query_len = strlen(query);  // クエリ文字列の長さを計算
    return strncmp(query, suffix_str, query_len);
}

// 比較関数（サフィックス配列用、最大1000文字まで比較）
int compare_suffixes(const void* a, const void* b) {
    const char* suffix_a = *(const char**)a;
    const char* suffix_b = *(const char**)b;
    return strncmp(suffix_a, suffix_b, 1000);  // 最大1000文字比較
}

int main() {
    // 入力データを一括読み込み
    fread(input_buffer, 1, sizeof(input_buffer), stdin);

    // ポインタ `ptr` を解析用に設定
    char* ptr = input_buffer;

    // 1行目：文字列 T を取得
    char* T = ptr;
    while (*ptr != '\n') ptr++;
    *ptr++ = '\0';  // 改行を '\0' に変換
    int len_T = strlen(T);  // T の長さを取得

    // 2行目：クエリ数 Q を取得
    int Q = atoi(ptr);
    while (*ptr != '\n') ptr++;
    ptr++;  // 改行をスキップ

    // サフィックス配列を構築（T の各文字へのポインタを格納）
    for (int i = 0; i < len_T; i++) {
        suffix_array[i] = &T[i];
    }

    // サフィックス配列を辞書順にソート（最大1000文字比較）
    qsort(suffix_array, len_T, sizeof(const char*), compare_suffixes);

    // 1つ目のループ：クエリ文字列をすべて取得（`P` の各要素にアドレスをセット）
    for (int i = 0; i < Q; i++) {
        P[i] = ptr;  // クエリ文字列の開始位置をセット
        while (*ptr != '\n' && *ptr != '\0') ptr++;
        if (*ptr == '\n') {
            *ptr++ = '\0';  // 改行を '\0' に変換
        }
    }

    // 出力バッファ用のポインタ初期化
    char* out_ptr = output_buffer;

    // 2つ目のループ：検索処理
    for (int i = 0; i < Q; i++) {
        // bsearch を使って部分文字列 (P[i]) をサフィックス配列内で検索
        const char** result = (const char**)bsearch(
            &P[i],            // 探索キー（ポインタのアドレスを渡す）
            suffix_array,     // 探索対象（サフィックス配列）
            len_T,            // 要素数
            sizeof(const char*), // 各要素のサイズ
            compare_query     // 比較関数
        );

        // 検索結果を出力バッファに格納
        *out_ptr++ = (result != NULL ? '1' : '0');
        *out_ptr++ = '\n';  // 改行を追加
    }

    // 出力バッファを標準出力へまとめて書き込み
    fwrite(output_buffer, 1, out_ptr - output_buffer, stdout);

    return 0;
}