#pragma once
#include <vector>
#include <string>
#include <cassert>
#include "Player.h"
#include <string>
#include <Windows.h>

#define CILCLE_LEN 15

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

	HBRUSH hBrushRed;		// = CreateSolidBrush(RGB(255, 0, 0));
	HBRUSH hBrushOrange;	// = CreateSolidBrush(RGB(0xFF, 0x7F, 0));
	HBRUSH hBrushPurple;	// = CreateSolidBrush(RGB(0x8B, 0x0, 0xFF));
	HBRUSH hBrushBrown;		
	HBRUSH hBrushBlue;		// = CreateSolidBrush(RGB(0, 0, 255));
	HBRUSH hBrushGray;		// = CreateSolidBrush(RGB(128, 128, 128));
	HBRUSH hBrushBlack;		// = CreateSolidBrush(RGB(0x0A, 0x0A, 0x0A));

	HBRUSH hBrushGreen0;	// = CreateSolidBrush(RGB(0, 128, 0));
	HBRUSH hBrushGreen1;	// = CreateSolidBrush(RGB(50, 150, 50));
	HBRUSH hBrushGreen2;	// = CreateSolidBrush(RGB(100, 180, 100));
	HBRUSH hBrushGreen3;	// = CreateSolidBrush(RGB(150, 210, 150));
	HBRUSH hBrushGreen4;	// = CreateSolidBrush(RGB(200, 240, 200));
	HBRUSH hBrushGreen5;	// = CreateSolidBrush(RGB(250, 255, 250));

	Grid() {
		hBrushRed = CreateSolidBrush(RGB(255, 0, 0));
		hBrushOrange = CreateSolidBrush(RGB(0xFF, 0x7F, 0));
		hBrushPurple = CreateSolidBrush(RGB(0x8B, 0x0, 0xFF));
		hBrushBrown = CreateSolidBrush(RGB(165, 42, 42));
		hBrushBlue = CreateSolidBrush(RGB(0, 0, 255));
		hBrushGray = CreateSolidBrush(RGB(128, 128, 128));
		hBrushBlack = CreateSolidBrush(RGB(0x0A, 0x0A, 0x0A));

		hBrushGreen0 = CreateSolidBrush(RGB(0, 128, 0));
		hBrushGreen1 = CreateSolidBrush(RGB(50, 150, 50));
		hBrushGreen2 = CreateSolidBrush(RGB(100, 180, 100));
		hBrushGreen3 = CreateSolidBrush(RGB(150, 210, 150));
		hBrushGreen4 = CreateSolidBrush(RGB(200, 240, 200));
		hBrushGreen5 = CreateSolidBrush(RGB(250, 255, 250));
	}
	~Grid() {
		DeleteObject(hBrushRed);
		DeleteObject(hBrushOrange);
		DeleteObject(hBrushPurple);
		DeleteObject(hBrushBrown);
		DeleteObject(hBrushBlue);
		DeleteObject(hBrushGray);
		DeleteObject(hBrushBlack);

		DeleteObject(hBrushGreen0);
		DeleteObject(hBrushGreen1);
		DeleteObject(hBrushGreen2);
		DeleteObject(hBrushGreen3);
		DeleteObject(hBrushGreen4);
		DeleteObject(hBrushGreen5);
	}

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

	void SelectPlayer(int winX, int winY, ThreadSafeUnorderedMap<unsigned int, Player*>& players, bool islogOn = false) {
		//winX -= offsetX;
		//winX /= ((double)cellSize / 64);
		//winY -= offsetY;
		//winY /= ((double)cellSize / 64);

		if (!islogOn) {
			for (auto iter = players.begin(); iter != players.end(); iter++) {
				Player* player = iter->second;
				int cX = player->clntPoint.X;
				int cY = player->clntPoint.Y;
				cX *= ((double)cellSize / 64);
				cX += offsetX;
				cY *= ((double)cellSize / 64);
				cY += offsetY;
				// Ellipse(hdc, cX - CILCLE_LEN, cY - CILCLE_LEN, cX + CILCLE_LEN, cY + CILCLE_LEN);
				if (cX - CILCLE_LEN <= winX && winX <= cX + CILCLE_LEN && cY - CILCLE_LEN <= winY && winY <= cY + CILCLE_LEN) {
					player->focusOn = !player->focusOn;
					break;
				}
			}
		}
		else {
			static Player* logOnPlayer = nullptr;
			if (logOnPlayer) {
				logOnPlayer->logOn = false;
				logOnPlayer = nullptr;
				return;
			}
			
			for (auto iter = players.begin(); iter != players.end(); iter++) {
				Player* player = iter->second;
				int cX = player->clntPoint.X;
				int cY = player->clntPoint.Y;
				cX *= ((double)cellSize / 64);
				cX += offsetX;
				cY *= ((double)cellSize / 64);
				cY += offsetY;
				// Ellipse(hdc, cX - CILCLE_LEN, cY - CILCLE_LEN, cX + CILCLE_LEN, cY + CILCLE_LEN);
				if (cX - CILCLE_LEN <= winX && winX <= cX + CILCLE_LEN && cY - CILCLE_LEN <= winY && winY <= cY + CILCLE_LEN) {
					player->logOn = !player->logOn;
					if (player->logOn) {
						logOnPlayer = player;
						std::cout << "[Host ID: " << player->hostID << " Log On!]" << std::endl;
					}
					break;
				}
			}
		}
	}

	void DrawGrid(HDC hdc, int windowWidth, int windowHeight) {// std::unordered_map<unsigned int, Player*>* players = nullptr, std::mutex* playersMtx = nullptr) {
		//HPEN pen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
		//
		//HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));
		//SelectObject(hdc, pen);
		//for (size_t i = 0; i < cells.size(); ++i) {
		//	for (size_t j = 0; j < cells[i].size(); ++j) {
		//		int x = (cells[i][j].x * cellSize) + offsetX;
		//		int y = (cells[i][j].y * cellSize) + offsetY;
		//
		//		// 테두리 그리기
		//		//HPEN pen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
		//		//HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));
		//		//SelectObject(hdc, pen);
		//		Rectangle(hdc, x, y, x + cellSize, y + cellSize);
		//		//
		//		//SelectObject(hdc, oldBrush);
		//		//DeleteObject(pen);
		//
		//		// 추가 정보를 사용하여 셀을 그리거나 다양한 작업 수행 가능
		//	}
		//}

		int lx = cells[0][0].x * cellSize + offsetX;
		int rx = cells[0][cells[0].size() - 1].x * cellSize + offsetX + cellSize;
		int y = cells[0][0].y * cellSize + offsetY;
		for (size_t i = 0; i < cells.size(); ++i) {
			MoveToEx(hdc, lx, y, NULL);
			LineTo(hdc, rx, y);
			y += cellSize;
		}

		int ty = cells[0][0].y * cellSize + offsetY;
		int dy = cells[cells.size() - 1][0].y * cellSize + offsetY + cellSize;
		int x = cells[0][0].x * cellSize + offsetX;
		for (size_t i = 0; i < cells[0].size(); ++i) {
			MoveToEx(hdc, x, ty, NULL);
			LineTo(hdc, x, dy);
			x += cellSize;
		}
		//SelectObject(hdc, oldBrush);
		//DeleteObject(pen);
	}
	void DrawPlayer(HDC hdc, int windowWidth, int windowHeight, ThreadSafeUnorderedMap<unsigned int, Player*>* players = nullptr) {//, std::mutex* playersMtx = nullptr) {
		HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, hBrushGreen0);
		if (players != nullptr) {
			//playersMtx->lock();
			for (auto iter = players->begin(); iter != players->end(); iter++) {
				Player* player = iter->second;
				Point& sp = player->servPoint;
				int sX = sp.X;
				int sY = sp.Y;
				sX *= ((double)cellSize / 64);
				sX += offsetX;
				sY *= ((double)cellSize / 64);
				sY += offsetY;
				const Point& cp = player->clntPoint;
				int cX = cp.X;
				int cY = cp.Y;
				cX *= ((double)cellSize / 64);
				cX += offsetX;
				cY *= ((double)cellSize / 64);
				cY += offsetY;
				
				if (player->rstSCFlag && player->rstCSFlag) {
					SelectObject(hdc, hBrushBlack);
				}
				else if (player->rstSCFlag) {
					SelectObject(hdc, hBrushRed);
				}
				else if (player->rstCSFlag) {
					SelectObject(hdc, hBrushGray);
				}
				else if (player->finFlag) {
					SelectObject(hdc, hBrushOrange);
				}
				else if (player->focusOn) {
					SelectObject(hdc, hBrushPurple);
				}
				else if (player->logOn) {
					SelectObject(hdc, hBrushBrown);
				}
				else if (player->crtFlag) {
					SelectObject(hdc, hBrushBlue);
				}
				else {
					if (player->byHP >= 90) {
						SelectObject(hdc, hBrushGreen0);
					}
					else if (player->byHP >= 70) {
						SelectObject(hdc, hBrushGreen1);
					}
					else if (player->byHP >= 50) {
						SelectObject(hdc, hBrushGreen2);
					}
					else if (player->byHP >= 30) {
						SelectObject(hdc, hBrushGreen3);
					}
					else if (player->byHP >= 10) {
						SelectObject(hdc, hBrushGreen4);
					}
					else if (player->byHP >= 0) {
						SelectObject(hdc, hBrushGreen5);
					}
				}
				Ellipse(hdc, cX - CILCLE_LEN, cY - CILCLE_LEN, cX + CILCLE_LEN, cY + CILCLE_LEN);
				std::wstring  hostIdText = std::to_wstring(player->hostID);
				//const wchar_t* hostIdText = std::to_wstring(player.hostID);
				TextOut(hdc, cX - 5, cY - 5, hostIdText.c_str(), wcslen(hostIdText.c_str()));
				//SelectObject(hdc, oldBrush);
				//DeleteObject(hBrush);
			}
			//playersMtx->unlock();
		}
		SelectObject(hdc, oldBrush);
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