#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_COMPARE_LEN 1000 // 比較の最大文字数を定義

// 比較関数（qsort用、アドレスの指す文字列を辞書順で比較）
int compare(const void *a, const void *b) {
    const char **pa = (const char **)a;
    const char **pb = (const char **)b;
    return strncmp(*pa, *pb, MAX_COMPARE_LEN); // 最大1000文字まで辞書順比較
}

// クエリパターンとサフィックス配列の要素を比較（bsearch用）
int match_pattern(const void *pattern, const void *suffix) {
    const char *pa = (const char *)pattern;
    const char **pb = (const char **)suffix;
    return strncmp(pa, *pb, strlen(pa)); // パターン長まで比較
}

int main() {
    static char T[1000001]; // 入力文字列 T（最大長 1000000, static に変更）
    static char P[MAX_COMPARE_LEN + 1]; // クエリ文字列 P（static に変更）
    int Q;                             // クエリ数 Q

    // T と Q の読み込み
    scanf("%s", T);
    scanf("%d", &Q);

    int n = strlen(T);

    // 重複を除去したサフィックス配列を作成
    char **unique_suffixes = (char **)malloc(n * sizeof(char *));
    int unique_count = 0;

    // サフィックスを追加しながら（1000文字までを比較して）重複を除去
    for (int i = 0; i < n; i++) {
        if (unique_count == 0 || strncmp(&T[i], unique_suffixes[unique_count - 1], MAX_COMPARE_LEN) != 0) {
            unique_suffixes[unique_count++] = &T[i];
        }
    }

    // 辞書順にソート
    qsort(unique_suffixes, unique_count, sizeof(char *), compare);

    // クエリを処理
    for (int i = 0; i < Q; i++) {
        scanf("%s", P);

        // bsearch を使用してパターンを探す
        void *found = bsearch(P, unique_suffixes, unique_count, sizeof(char *), match_pattern);

        // 結果を出力
        if (found != NULL) {
            printf("1\n"); // パターンが見つかった
        } else {
            printf("0\n"); // パターンが見つからない
        }
    }

    return 0; // メモリの解放処理を削除
}
