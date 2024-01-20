#pragma once
#include <vector>
#include <string>
#include <cassert>
#include "Player.h"
#include <string>
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

	void Draw(HDC hdc, int windowWidth, int windowHeight, const std::unordered_map<unsigned int, Player>* players = nullptr, std::mutex* playersMtx = nullptr) {
		bool textFlag = false;

		HPEN pen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
		HBRUSH hBrushServ = CreateSolidBrush(RGB(255, 0, 0));
		HBRUSH hBrushClnt = CreateSolidBrush(RGB(0, 255, 0));

		HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));
		SelectObject(hdc, pen);
		for (size_t i = 0; i < cells.size(); ++i) {
			for (size_t j = 0; j < cells[i].size(); ++j) {
				int x = (cells[i][j].x * cellSize) + offsetX;
				int y = (cells[i][j].y * cellSize) + offsetY;

				// 테두리 그리기
				//HPEN pen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
				//HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));
				//SelectObject(hdc, pen);
				Rectangle(hdc, x, y, x + cellSize, y + cellSize);
				//
				//SelectObject(hdc, oldBrush);
				//DeleteObject(pen);

				// 추가 정보를 사용하여 셀을 그리거나 다양한 작업 수행 가능
			}
		}
		
		oldBrush = (HBRUSH)SelectObject(hdc, hBrushServ);
		if (players != nullptr) {
			playersMtx->lock();
			for (auto iter = players->begin(); iter != players->end(); iter++) {
				const Player& player = iter->second;
				const Point& sp = player.servPoint;
				int sX = sp.X;
				int sY = sp.Y;
				sX *= ((double)cellSize / 64);
				sX += offsetX;
				sY *= ((double)cellSize / 64);
				sY += offsetY;
				const Point& cp = player.clntPoint;
				int cX = cp.X;
				int cY = cp.Y;
				cX *= ((double)cellSize / 64);
				cX += offsetX;
				cY *= ((double)cellSize / 64);
				cY += offsetY;
				
				//HBRUSH hBrush = CreateSolidBrush(RGB(255, 0, 0));
				//HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, hBrush);
				//SelectObject(hdc, hBrushServ);
				//Ellipse(hdc, sX - 20, sY - 20, sX + 20, sY + 20);
				//SelectObject(hdc, oldBrush);
				//DeleteObject(hBrush);

				//hBrush = CreateSolidBrush(RGB(0, 255, 0));
				//oldBrush = (HBRUSH)SelectObject(hdc, hBrush);
				SelectObject(hdc, hBrushClnt);
				Ellipse(hdc, cX - 15, cY - 15, cX + 15, cY + 15);
				std::wstring  hostIdText = std::to_wstring(player.hostID);
				//const wchar_t* hostIdText = std::to_wstring(player.hostID);
				TextOut(hdc, cX - 5, cY - 5, hostIdText.c_str(), wcslen(hostIdText.c_str()));
				//SelectObject(hdc, oldBrush);
				//DeleteObject(hBrush);
			}
			playersMtx->unlock();
		}
		SelectObject(hdc, oldBrush);

		DeleteObject(pen);
		DeleteObject(hBrushServ);
		DeleteObject(hBrushClnt);
	}
	//void DrawPlayer(HDC hdc, PlayerManager* pm) {
	//	pm->pmapMtx.lock();
	//	pm->players.begin();
	//	for (auto it = pm->players.begin(); it != pm->players.end(); it++) {
	//		Player&p = it->second;
	//		
	//	}
	//}

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