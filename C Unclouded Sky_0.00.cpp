#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define min(a, b) ((a) < (b) ? (a) : (b))

typedef struct {
    int dx, dy;
} Direction;

// 方向に応じた移動ベクトルを返す関数
Direction getDirection(const char* D) {
    if (D[0] == 'N' && D[1] == 'E') return (Direction) { 1, -1 };
    if (D[0] == 'N' && D[1] == 'W') return (Direction) { 1, 1 };
    if (D[0] == 'N')  return (Direction) { 1, 0 };
    if (D[0] == 'S' && D[1] == 'E') return (Direction) { -1, -1 };
    if (D[0] == 'S' && D[1] == 'W') return (Direction) { -1, 1 };
    if (D[0] == 'E')  return (Direction) { 0, -1 };
    if (D[0] == 'S')  return (Direction) { -1, 0 };
    if (D[0] == 'W')  return (Direction) { 0, 1 };
    return (Direction) { 0, 0 }; // default (invalid)
}

int calculateExitTime(int x, int y, int H, int W, const Direction dir) {
    int time = INT_MAX;

    // 行方向にグリッド外へ出る時間
    if (dir.dx > 0) { // 南方向 (増加)
        time = min(time, (H - x) / dir.dx);
    }
    else if (dir.dx < 0) { // 北方向 (減少)
        time = min(time, (x + 1) / -dir.dx);
    }

    // 列方向にグリッド外へ出る時間
    if (dir.dy > 0) { // 東方向 (増加)
        time = min(time, (W - y) / dir.dy);
    }
    else if (dir.dy < 0) { // 西方向 (減少)
        time = min(time, (y + 1) / -dir.dy);
    }

    return time;
}

int main() {
    int H, W;
    scanf("%d %d", &H, &W);

    char grid[1000][1000 + 1];
    for (int i = 0; i < H; i++) {
        scanf("%s", grid[i]);
    }

    char D[3];
    scanf("%s", D);

    // 風向から移動ベクトルを取得
    Direction dir = getDirection(D);

    int maxTime = 0;

    // 雲の位置を調べ、最も時間がかかるものを見つける
    for (int i = 0; i < H; i++) {
        for (int j = 0; j < W; j++) {
            if (grid[i][j] == '#') {
                // 雲が現在のマス (i, j) に存在
                int time = calculateExitTime(i, j, H, W, dir);
                if (time > maxTime) {
                    maxTime = time;
                }
            }
        }
    }

    // 答えを出力
    printf("%d\n", maxTime);

    return 0;
}
