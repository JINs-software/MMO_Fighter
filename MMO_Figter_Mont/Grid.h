#pragma once
#include <vector>
#include <string>
#include <cassert>
#include "Player.h"
#include <Windows.h>

struct Cell {
	int x;
	int y;
	Cell(int xPos, int yPos) : x(xPos), y(yPos) {}
};

struct Grid {
public:
	int cellSize;	// 단일 Cell의 한 변의 길이
	int offsetX;
	int offsetY;
	int cellX;
	int cellY;
	std::vector<std::vector<Cell>> cells;

	void SetGridCell(int size, int cY, int cX) {
		cellSize = size;
		cellX = cX;
		cellY = cY;

		for (int i = 0; i < cellY; ++i) {
			std::vector<Cell> row;
			for (int j = 0; j < cellX; ++j) {
				row.emplace_back(j, i);
			}
			cells.emplace_back(row);
		}
	}

	void Draw(HDC hdc, int windowWidth, int windowHeight) {
		bool textFlag = false;

		for (size_t i = 0; i < cells.size(); ++i) {
			for (size_t j = 0; j < cells[i].size(); ++j) {
				int x = (cells[i][j].x * cellSize) + offsetX;
				int y = (cells[i][j].y * cellSize) + offsetY;

				// 테두리 그리기
				HPEN pen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
				HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));
				SelectObject(hdc, pen);
				Rectangle(hdc, x, y, x + cellSize, y + cellSize);

				SelectObject(hdc, oldBrush);
				DeleteObject(pen);

				// 추가 정보를 사용하여 셀을 그리거나 다양한 작업 수행 가능
			}
		}
	}
	void DrawPlayer(HDC hdc, PlayerManager* pm) {
		pm->pmapMtx.lock();
		pm->players.begin();
		for (auto it = pm->players.begin(); it != pm->players.end(); it++) {
			Player&p = it->second;
			
		}
	}

	void MoveLeft() {
		offsetX += cellSize * 2;
	}

	void MoveRight() {
		offsetX -= cellSize * 2;
	}
	void MoveUp() {
		offsetY += cellSize * 2;
	}

	void MoveDown() {
		offsetY -= cellSize * 2;
	}
};