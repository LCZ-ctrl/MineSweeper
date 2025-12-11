#include "game.h"
#include <cstdlib>
#include <ctime>
#include <cmath>

Game::Game(int r, int c, int m) {
    Init(r, c, m);
}

void Game::Init(int r, int c, int m) {
    rows = r;
    cols = c;
    mineCount = m;

    grid.assign(rows, std::vector<Cell>(cols));
    NewGame();
}

void Game::NewGame() {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            grid[i][j] = Cell{};
        }
    }
    flagsLeft = mineCount;
    revealedCnt = 0;
    timer = 0.0f;
    gameOver = false;
    victory = false;
    firstClick = true;
    minesPlaced = false;
    explodedRow = explodedCol = -1;
}

void Game::SetTextures(Texture2D bomb, Texture2D bombExploded, Texture2D question, Texture2D flag) {
    bombTexture = bomb;
    bombExplodedTexture = bombExploded;
    questionTexture = question;
    flagTexture = flag;
}

void Game::SetFaceTextures(Texture2D f1, Texture2D f2, Texture2D f3, Texture2D f4) {
    faceTextures[0] = f1; faceTextures[1] = f2; faceTextures[2] = f3; faceTextures[3] = f4;
}

void Game::PlaceMines(int row, int col) {
    int placed = 0;
    while (placed < mineCount) {
        int r = rand() % rows;
        int c = rand() % cols;
        if (abs(r - row) <= 1 && abs(c - col) <= 1) continue; 
        if (!grid[r][c].isMine) {
            grid[r][c].isMine = true;
            placed++;
        }
    }
    CalculateNumbers();
    minesPlaced = true;
}

void Game::CalculateNumbers() {
    int dx[8] = { -1, -1, -1, 0, 0, 1, 1, 1 };
    int dy[8] = { -1, 0, 1, -1, 1, -1, 0, 1 };
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (!grid[i][j].isMine) {
                int cnt = 0;
                for (int d = 0; d < 8; d++) {
                    int nx = i + dx[d];
                    int ny = j + dy[d];
                    if (nx >= 0 && nx < rows && ny >= 0 && ny < cols) {
                        if (grid[nx][ny].isMine) cnt++;
                    }
                }
                grid[i][j].adjacent = cnt;
            }
        }
    }
}

void Game::Reveal(int row, int col) {
    if (row < 0 || row >= rows || col < 0 || col >= cols) return;
    if (grid[row][col].state != CellState::Hidden) return;
    if (grid[row][col].isMine) {
        grid[row][col].state = CellState::Revealed;
        gameOver = true;
        explodedRow = row;
        explodedCol = col;
        return;
    }
    grid[row][col].state = CellState::Revealed;
    revealedCnt++;
    if (grid[row][col].adjacent == 0) FloodReveal(row, col);
}

void Game::FloodReveal(int row, int col) {
    int dx[8] = { -1, -1, -1, 0, 0, 1, 1, 1 };
    int dy[8] = { -1, 0, 1, -1, 1, -1, 0, 1 };
    for (int d = 0; d < 8; d++) {
        int nx = row + dx[d];
        int ny = col + dy[d];
        if (nx >= 0 && nx < rows && ny >= 0 && ny < cols) {
            if (grid[nx][ny].state == CellState::Hidden) {
                Reveal(nx, ny);
            }
        }
    }
}

bool Game::CheckVictory() {
    if (!minesPlaced) return false;
    if (revealedCnt == rows * cols - mineCount) return true;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (grid[i][j].isMine && grid[i][j].state != CellState::Flagged) return false;
        }
    }
    return true;
}

Color GetNumberColor(int n) {
    switch (n) {
    case 1: return BLUE;
    case 2: return GREEN;
    case 3: return RED;
    case 4: return DARKBLUE;
    case 5: return MAROON;
    case 6: return SKYBLUE;
    case 7: return BLACK;
    case 8: return GRAY;
    default: return BLACK;
    }
}

static void DrawTextureInCell(const Texture2D& tex, int px, int py) {
    if (tex.id == 0) return;
    float texW = (float)tex.width;
    float texH = (float)tex.height;
    float pad = 2.0f;
    float dstW = (float)CELL_SIZE - pad * 2.0f;
    float dstH = (float)CELL_SIZE - pad * 2.0f;
    float scale = fmaxf(dstW / texW, dstH / texH);
    float srcW = dstW / scale;
    float srcH = dstH / scale;
    float srcX = (texW - srcW) * 0.5f;
    float srcY = (texH - srcH) * 0.5f;
    Rectangle src = { srcX, srcY, srcW, srcH };
    Rectangle dest = { (float)(px + pad), (float)(py + pad), dstW, dstH };
    Vector2 origin = { 0.0f, 0.0f };
    DrawTexturePro(tex, src, dest, origin, 0.0f, WHITE);
}

void Game::HandleClick(Vector2 mouse, bool leftClick, int offsetX, int offsetY) {
    if (gameOver || victory) return;

    if (mouse.x < offsetX || mouse.y < offsetY) return;
    if (mouse.x >= offsetX + cols * CELL_SIZE) return;
    if (mouse.y >= offsetY + rows * CELL_SIZE) return;

    int col = (int)((mouse.x - offsetX) / CELL_SIZE);
    int row = (int)((mouse.y - offsetY) / CELL_SIZE);
    if (col < 0 || col >= cols || row < 0 || row >= rows) return;

    Cell& cell = grid[row][col];

    if (leftClick) {
        if (cell.state != CellState::Hidden) return;
        if (firstClick) {
            PlaceMines(row, col);
            firstClick = false;
        }
        Reveal(row, col);
        if (gameOver) return;
        victory = CheckVictory();
    }
    else {
        switch (cell.state) {
        case CellState::Hidden:
            if (flagsLeft > 0) { cell.state = CellState::Flagged; flagsLeft--; }
            break;
        case CellState::Flagged:
            cell.state = CellState::Questioned;
            flagsLeft++;
            break;
        case CellState::Questioned:
            cell.state = CellState::Hidden;
            break;
        case CellState::Revealed:
            break;
        }
        if (CheckVictory()) victory = true;
    }
}

void Game::DrawGrid(int offsetX, int offsetY) {
    Color unopenedBg = Color{ 160,160,160,255 };
    Color openedBg = Color{ 205,205,205,255 };
    Color borderColor = DARKGRAY;
    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < cols; col++) {
            int px = offsetX + col * CELL_SIZE;
            int py = offsetY + row * CELL_SIZE;
            Cell& c = grid[row][col];
            if (c.state == CellState::Revealed) {
                DrawRectangle(px, py, CELL_SIZE, CELL_SIZE, openedBg);
            }
            else {
                DrawRectangle(px, py, CELL_SIZE, CELL_SIZE, unopenedBg);
                DrawRectangle(px + 1, py + 1, CELL_SIZE - 2, 2, Color{ 230,230,230,255 });
                DrawRectangle(px + 1, py + 1, 2, CELL_SIZE - 2, Color{ 230,230,230,255 });
            }
            DrawRectangleLines(px, py, CELL_SIZE, CELL_SIZE, borderColor);

            if (c.state == CellState::Revealed) {
                if (c.isMine) {
                    if ((col == explodedCol && row == explodedRow) && bombExplodedTexture.id != 0) {
                        DrawTextureInCell(bombExplodedTexture, px, py);
                    }
                    else if (bombTexture.id != 0) {
                        DrawTextureInCell(bombTexture, px, py);
                    }
                    else {
                        DrawRectangle(px + 6, py + 6, CELL_SIZE - 12, CELL_SIZE - 12, BLACK);
                        DrawText("X", px + (CELL_SIZE - MeasureText("X", 26)) / 2, py + (CELL_SIZE - 26) / 2, 26, RED);
                    }
                }
                else if (c.adjacent > 0) {
                    char buf[4];
                    buf[0] = '0' + c.adjacent; buf[1] = '\0';
                    int fontSize = 30;
                    int tw = MeasureText(buf, fontSize);
                    DrawText(buf, px + (CELL_SIZE - tw) / 2, py + (CELL_SIZE - fontSize) / 2, fontSize, GetNumberColor(c.adjacent));
                }
            }
            else {
                if (c.state == CellState::Flagged) {
                    if (flagTexture.id != 0) DrawTextureInCell(flagTexture, px, py);
                    else {
                        const char* ftxt = "F";
                        int fontSize = 30;
                        int tw = MeasureText(ftxt, fontSize);
                        DrawText(ftxt, px + (CELL_SIZE - tw) / 2, py + (CELL_SIZE - fontSize) / 2, fontSize, RED);
                    }
                }
                else if (c.state == CellState::Questioned) {
                    if (questionTexture.id != 0) DrawTextureInCell(questionTexture, px, py);
                    else {
                        const char* qtxt = "?";
                        int fontSize = 34;
                        int tw = MeasureText(qtxt, fontSize);
                        DrawText(qtxt, px + (CELL_SIZE - tw) / 2, py + (CELL_SIZE - fontSize) / 2, fontSize, BLACK);
                    }
                }
            }

            if (gameOver && c.isMine && c.state != CellState::Revealed) {
                if (col == explodedCol && row == explodedRow) {
                    if (bombExplodedTexture.id != 0) DrawTextureInCell(bombExplodedTexture, px, py);
                    else {
                        DrawRectangle(px + 6, py + 6, CELL_SIZE - 12, CELL_SIZE - 12, BLACK);
                        DrawText("X", px + (CELL_SIZE - MeasureText("X", 26)) / 2, py + (CELL_SIZE - 26) / 2, 26, RED);
                    }
                }
                else {
                    if (bombTexture.id != 0) DrawTextureInCell(bombTexture, px, py);
                    else {
                        DrawRectangle(px + 6, py + 6, CELL_SIZE - 12, CELL_SIZE - 12, BLACK);
                        DrawText("X", px + (CELL_SIZE - MeasureText("X", 26)) / 2, py + (CELL_SIZE - 26) / 2, 26, RED);
                    }
                }
            }
        }
    }
}
