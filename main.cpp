#include <raylib.h>
#include "game.h"
#include <ctime>
#include <cstdlib>
#include <string>
#include <algorithm>

static void SetWindowSizeCentered(int newW, int newH) {
	int curW = GetScreenWidth();
	int curH = GetScreenHeight();
	Vector2 pos = GetWindowPosition();
	int curX = (int)pos.x;
	int curY = (int)pos.y;

	int centerX = curX + curW / 2;
	int centerY = curY + curH / 2;

	SetWindowSize(newW, newH);

	int newX = centerX - newW / 2;
	int newY = centerY - newH / 2;

	SetWindowPosition(newX, newY);
}

enum class AppState { Menu, Playing };

int main() {
	srand((unsigned)time(nullptr));

	const int NORMAL_ROWS = 9;
	const int NORMAL_COLS = 9;
	const int NORMAL_MINES = 10;

	const int HARD_ROWS = 16;
	const int HARD_COLS = 16;
	const int HARD_MINES = 40;

	const int MARGIN = 60;

	int windowWidth = NORMAL_COLS * CELL_SIZE + MARGIN * 2;
	int windowHeight = NORMAL_ROWS * CELL_SIZE + MARGIN * 2 + 40;

	InitWindow(windowWidth, windowHeight, "Minesweeper");
	Image icon = LoadImage("Graphics/logo1.png");
	SetWindowIcon(icon);
	UnloadImage(icon);

	SetTargetFPS(60);

	SetExitKey(KEY_NULL);

	Game game(NORMAL_ROWS, NORMAL_COLS, NORMAL_MINES);

	Texture2D bombTex = LoadTexture("Graphics/bomb1.png");
	Texture2D bomb2Tex = LoadTexture("Graphics/bomb2.png");
	Texture2D questionTex = LoadTexture("Graphics/question1.png");
	Texture2D flagTex = LoadTexture("Graphics/flag1.png");
	Texture2D face1 = LoadTexture("Graphics/face01.png");
	Texture2D face2 = LoadTexture("Graphics/face02.png");
	Texture2D face3 = LoadTexture("Graphics/face03.png");
	Texture2D face4 = LoadTexture("Graphics/face04.png");
	game.SetTextures(bombTex, bomb2Tex, questionTex, flagTex);
	game.SetFaceTextures(face1, face2, face3, face4);

	const int gridOffsetX = MARGIN;
	const int gridOffsetY = MARGIN + 20;

	AppState state = AppState::Menu;

	const int btnW = 300;
	const int btnH = 64;
	const int btnSpacing = 20;

	while (!WindowShouldClose()) {
		if (state == AppState::Playing) {
			if (!game.gameOver && !game.victory && !game.firstClick)
				game.timer += GetFrameTime();
		}

		if (IsKeyPressed(KEY_ESCAPE)) {
			if (state == AppState::Playing) {
				state = AppState::Menu;
				int newW = NORMAL_COLS * CELL_SIZE + MARGIN * 2;
				int newH = NORMAL_ROWS * CELL_SIZE + MARGIN * 2 + 40;
				SetWindowSizeCentered(newW, newH);
				windowWidth = newW; windowHeight = newH;
			}
		}

		int menuCenterX = windowWidth / 2;
		int menuCenterY = windowHeight / 2;
		int totalBtnsH = 3 * btnH + 2 * btnSpacing;
		int topY = menuCenterY - totalBtnsH / 2;
		Rectangle startBtn = { (float)(menuCenterX - btnW / 2), (float)(topY), (float)btnW, (float)btnH };
		Rectangle hardBtn = { (float)(menuCenterX - btnW / 2), (float)(topY + btnH + btnSpacing), (float)btnW, (float)btnH };
		Rectangle exitBtn = { (float)(menuCenterX - btnW / 2), (float)(topY + (btnH + btnSpacing) * 2), (float)btnW, (float)btnH };

		int winWidthForGrid = gridOffsetX * 2 + game.GetCols() * CELL_SIZE;
		int faceDrawSize = 40;
		float faceX = (float)(winWidthForGrid / 2 - faceDrawSize / 2);
		float faceY = 20.0f;
		Rectangle faceRect = { faceX, faceY, (float)faceDrawSize, (float)faceDrawSize };

		std::string bombsText = std::to_string(game.flagsLeft);
		if (bombsText.length() < 3) bombsText = std::string(3 - bombsText.length(), '0') + bombsText;
		std::string leftLabel = "Bomb: " + bombsText;
		int leftFont = 25;
		int leftTextW = MeasureText(leftLabel.c_str(), leftFont);
		int leftPadding = 25;
		int leftBoxW = leftTextW + leftPadding * 2;
		int leftBoxH = leftFont + leftPadding;
		int leftX = 20;
		int leftY = 15;
		Rectangle leftBoxRect = { (float)leftX, (float)leftY, (float)leftBoxW, (float)leftBoxH };

		int totalSeconds = (int)game.timer;
		int minutes = totalSeconds / 60;
		int seconds = totalSeconds % 60;
		char timeBuf[8];
		snprintf(timeBuf, sizeof(timeBuf), "%02d:%02d", minutes, seconds);
		std::string rightLabel = std::string("Time: ") + timeBuf;
		int rightFont = 25;
		int rightTextW = MeasureText(rightLabel.c_str(), rightFont);
		int rightPadding = 25;
		int rightBoxW = rightTextW + rightPadding * 2;
		int rightBoxH = rightFont + rightPadding;
		int rightX = windowWidth - rightBoxW - 20;
		int rightY = 15;
		Rectangle rightBoxRect = { (float)rightX, (float)rightY, (float)rightBoxW, (float)rightBoxH };

		if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
			Vector2 m = GetMousePosition();
			if (state == AppState::Menu) {
				if (CheckCollisionPointRec(m, startBtn)) {
					int newW = NORMAL_COLS * CELL_SIZE + MARGIN * 2;
					int newH = NORMAL_ROWS * CELL_SIZE + MARGIN * 2 + 40;
					SetWindowSizeCentered(newW, newH);
					windowWidth = newW; windowHeight = newH;

					game.Init(NORMAL_ROWS, NORMAL_COLS, NORMAL_MINES);
					game.SetTextures(bombTex, bomb2Tex, questionTex, flagTex);
					game.SetFaceTextures(face1, face2, face3, face4);
					state = AppState::Playing;
				}
				else if (CheckCollisionPointRec(m, hardBtn)) {
					int newW = HARD_COLS * CELL_SIZE + MARGIN * 2;
					int newH = HARD_ROWS * CELL_SIZE + MARGIN * 2 + 40;
					SetWindowSizeCentered(newW, newH);
					windowWidth = newW; windowHeight = newH;

					game.Init(HARD_ROWS, HARD_COLS, HARD_MINES);
					game.SetTextures(bombTex, bomb2Tex, questionTex, flagTex);
					game.SetFaceTextures(face1, face2, face3, face4);
					state = AppState::Playing;
				}
				else if (CheckCollisionPointRec(m, exitBtn)) {
					break;
				}
			}
			else { 
				if ((game.gameOver || game.victory) && CheckCollisionPointRec(m, faceRect)) {
					game.NewGame();
				}
				else if (CheckCollisionPointRec(m, leftBoxRect) || CheckCollisionPointRec(m, rightBoxRect) || CheckCollisionPointRec(m, faceRect)) {
					// do nothing
				}
				else {
					game.HandleClick(m, true, gridOffsetX, gridOffsetY);
				}
			}
		}

		if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
			Vector2 m = GetMousePosition();
			if (state == AppState::Menu) {
				// do nothing
			}
			else {
				if (CheckCollisionPointRec(m, leftBoxRect) || CheckCollisionPointRec(m, rightBoxRect) || CheckCollisionPointRec(m, faceRect)) {
					// do nothing
				}
				else {
					game.HandleClick(m, false, gridOffsetX, gridOffsetY);
				}
			}
		}

		BeginDrawing();
		ClearBackground(Color{ 192,192,192 });

		if (state == AppState::Menu) {
			DrawRectangle(0, 0, windowWidth, windowHeight, Color{ 100,149,237,255 });
			const char* title = "MINESWEEPER";
			int titleFont = 55;
			int titleW = MeasureText(title, titleFont);
			DrawText(title, windowWidth / 2 - titleW / 2, 80, titleFont, RAYWHITE);

			Vector2 mousePos = GetMousePosition();
			bool hoverStart = CheckCollisionPointRec(mousePos, startBtn);
			bool hoverHard = CheckCollisionPointRec(mousePos, hardBtn);
			bool hoverExit = CheckCollisionPointRec(mousePos, exitBtn);

			DrawRectangleRec(startBtn, hoverStart ? Color{ 60,60,60,255 } : Color{ 40,40,40,255 });
			DrawRectangleLines((int)startBtn.x, (int)startBtn.y, (int)startBtn.width, (int)startBtn.height, WHITE);
			const char* startTxt = "Normal";
			int sf = 28;
			int sw = MeasureText(startTxt, sf);
			DrawText(startTxt, (int)(startBtn.x + (startBtn.width - sw) / 2), (int)(startBtn.y + (startBtn.height - sf) / 2), sf, RAYWHITE);

			DrawRectangleRec(hardBtn, hoverHard ? Color{ 60,60,60,255 } : Color{ 40,40,40,255 });
			DrawRectangleLines((int)hardBtn.x, (int)hardBtn.y, (int)hardBtn.width, (int)hardBtn.height, WHITE);
			const char* hardTxt = "Hard";
			int hf = 28;
			int hw = MeasureText(hardTxt, hf);
			DrawText(hardTxt, (int)(hardBtn.x + (hardBtn.width - hw) / 2), (int)(hardBtn.y + (hardBtn.height - hf) / 2), hf, RAYWHITE);

			DrawRectangleRec(exitBtn, hoverExit ? Color{ 60,60,60,255 } : Color{ 40,40,40,255 });
			DrawRectangleLines((int)exitBtn.x, (int)exitBtn.y, (int)exitBtn.width, (int)exitBtn.height, WHITE);
			const char* exitTxt = "Exit";
			int ef = 28;
			int ew = MeasureText(exitTxt, ef);
			DrawText(exitTxt, (int)(exitBtn.x + (exitBtn.width - ew) / 2), (int)(exitBtn.y + (exitBtn.height - ef) / 2), ef, RAYWHITE);

			const char* pre = "Press ";
			const char* esc = "Esc";
			const char* post = " to return to the menu";
			const char* second = "while in the game";
			int hintFont = 28;
			int preW = MeasureText(pre, hintFont);
			int escW = MeasureText(esc, hintFont);
			int postW = MeasureText(post, hintFont);
			int totalW = preW + escW + postW;
			int line1Y = windowHeight - 80;
			int hintX = windowWidth / 2 - totalW / 2;
			DrawText(pre, hintX, line1Y, hintFont, WHITE);
			DrawText(esc, hintX + preW, line1Y, hintFont, RED);
			DrawText(post, hintX + preW + escW, line1Y, hintFont, WHITE);
			int secondW = MeasureText(second, hintFont);
			DrawText(second, windowWidth / 2 - secondW / 2, line1Y + hintFont + 6, hintFont, WHITE);
		}
		else {
			DrawRectangle(0, 0, windowWidth, 60, Color{ 128,128,128 });

			DrawRectangle(leftX, leftY, leftBoxW, leftBoxH, BLACK);
			DrawRectangleLines(leftX, leftY, leftBoxW, leftBoxH, WHITE);
			DrawText(leftLabel.c_str(), leftX + leftPadding, leftY + (leftBoxH - leftFont) / 2, leftFont, RED);

			DrawRectangle(rightX, rightY, rightBoxW, rightBoxH, BLACK);
			DrawRectangleLines(rightX, rightY, rightBoxW, rightBoxH, WHITE);
			DrawText(rightLabel.c_str(), rightX + rightPadding, rightY + (rightBoxH - rightFont) / 2, rightFont, RED);

			Rectangle faceDest = faceRect;
			int faceIndex = 0;
			if (game.victory) faceIndex = 3;
			else if (game.gameOver) faceIndex = 2;
			else {
				Vector2 m = GetMousePosition();
				int mx = (int)m.x - gridOffsetX;
				int my = (int)m.y - gridOffsetY;
				int col = mx / CELL_SIZE;
				int row = my / CELL_SIZE;
				if (mx >= 0 && my >= 0 && col >= 0 && col < game.GetCols() && row >= 0 && row < game.GetRows()) {
					if (game.grid[row][col].state == CellState::Questioned) faceIndex = 1;
				}
				else faceIndex = 0;
			}
			Texture2D faceTex = game.faceTextures[faceIndex];
			DrawTexturePro(faceTex, Rectangle{ 0.0f, 0.0f, (float)faceTex.width, (float)faceTex.height }, faceDest, Vector2{ 0.0f, 0.0f }, 0.0f, WHITE);

			game.DrawGrid(gridOffsetX, gridOffsetY);

			if (game.victory || game.gameOver) {
				const char* msg = game.victory ? "YOU WIN!" : "GAME OVER!";
				int fontSize = 40;
				int textW = MeasureText(msg, fontSize);
				int padding = 16;
				int boxW = textW + padding * 2;
				int boxH = fontSize + (int)(padding * 1.5f);
				int boxX = windowWidth / 2 - boxW / 2;
				int boxY = windowHeight - boxH - 7;
				DrawRectangle(boxX, boxY, boxW, boxH, Color{ 0,0,0,200 });
				DrawRectangleLines(boxX, boxY, boxW, boxH, WHITE);
				DrawText(msg, boxX + padding, boxY + padding, fontSize, game.victory ? GREEN : RED);
			}
		}

		EndDrawing();
	}

	if (bombTex.id != 0) UnloadTexture(bombTex);
	if (bomb2Tex.id != 0) UnloadTexture(bomb2Tex);
	if (questionTex.id != 0) UnloadTexture(questionTex);
	if (flagTex.id != 0) UnloadTexture(flagTex);
	if (face1.id != 0) UnloadTexture(face1);
	if (face2.id != 0) UnloadTexture(face2);
	if (face3.id != 0) UnloadTexture(face3);
	if (face4.id != 0) UnloadTexture(face4);

	CloseWindow();
	return 0;
}
