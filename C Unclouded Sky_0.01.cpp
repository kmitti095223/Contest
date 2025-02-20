#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <climits>

using namespace std;

struct Direction {
    int dx, dy;
};

// 方向に応じた移動ベクトルを返す関数
Direction getDirection(const string& D) {
    if (D == "N")  return {1,  0};
    if (D == "NE") return {1, -1};
    if (D == "E")  return {0, -1};
    if (D == "SE") return {-1, -1};
    if (D == "S")  return {-1,  0};
    if (D == "SW") return {-1,  1};
    if (D == "W")  return {0,  1};
    if (D == "NW") return {1,  1};
    throw invalid_argument("Invalid direction");
}

int calculateExitTime(int x, int y, int H, int W, const Direction& dir) {
    int time = INT_MAX;

    // 行方向にグリッド外へ出る時間
    if (dir.dx > 0) { // 南方向 (増加)
        time = min(time, (H - x) / dir.dx);
    } else if (dir.dx < 0) { // 北方向 (減少)
        time = min(time, (x + 1) / -dir.dx);
    }

    // 列方向にグリッド外へ出る時間
    if (dir.dy > 0) { // 東方向 (増加)
        time = min(time, (W - y) / dir.dy);
    } else if (dir.dy < 0) { // 西方向 (減少)
        time = min(time, (y + 1) / -dir.dy);
    }

    return time;
}

int main() {
    int H, W;
    cin >> H >> W;

    vector<string> grid(H);
    for (int i = 0; i < H; i++) {
        cin >> grid[i];
    }

    string D;
    cin >> D;

    // 風向から移動ベクトルを取得
    Direction dir = getDirection(D);

    int maxTime = 0;

    // 雲の位置を調べ、最も時間がかかるものを見つける
    for (int i = 0; i < H; i++) {
        for (int j = 0; j < W; j++) {
            if (grid[i][j] == '#') {
                // 雲が現在のマス (i, j) に存在
                int time = calculateExitTime(i, j, H, W, dir);
                maxTime = max(maxTime, time);
            }
        }
    }

    // 答えを出力
    cout << maxTime << endl;

    return 0;
}