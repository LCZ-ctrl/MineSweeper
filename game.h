#pragma once
#include <raylib.h>
#include <vector>

const int CELL_SIZE = 48; 

enum class CellState {
    Hidden,
    Revealed,
    Flagged,
    Questioned
};

struct Cell {
    bool isMine = false; 
    int adjacent = 0; 
    CellState state = CellState::Hidden;
};

class Game {
public:
    int rows;
    int cols;
    int mineCount;

    std::vector<std::vector<Cell>> grid;
    int flagsLeft;
    int revealedCnt;
    float timer;
    bool gameOver;
    bool victory;
    bool firstClick;
    bool minesPlaced;

    int explodedRow;
    int explodedCol;

    Texture2D bombTexture = { 0 };
    Texture2D bombExplodedTexture = { 0 };
    Texture2D questionTexture = { 0 };
    Texture2D flagTexture = { 0 };
    Texture2D faceTextures[4] = { {0},{0},{0},{0} };

    Game(int rows = 9, int cols = 9, int mines = 10);
    void Init(int rows, int cols, int mines);
    void NewGame();

    void DrawGrid(int offsetX, int offsetY);
    void HandleClick(Vector2 mouse, bool leftClick, int offsetX, int offsetY);
    void Reveal(int row, int col);
    void FloodReveal(int row, int col);
    void PlaceMines(int row, int col);
    void CalculateNumbers();
    bool CheckVictory();

    void SetTextures(Texture2D bomb, Texture2D bombExploded, Texture2D question, Texture2D flag);
    void SetFaceTextures(Texture2D f1, Texture2D f2, Texture2D f3, Texture2D f4);

    int GetRows() const { return rows; }
    int GetCols() const { return cols; }
    int GetMineCount() const { return mineCount; }
};
