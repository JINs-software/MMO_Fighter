#pragma once
#include "Types.h"
#include "Configuration.h"

struct stPoint {
	uint16	usX;
	uint16	usY;
};

struct stObjectInfo {
	uint32	uiID;
	BYTE	byDir;
	stPoint stPos;
	BYTE	byHP;

	// 서버가 이용하는 데이터
	bool bMoveFlag = false;
	bool bFirstMoveFlag = false;

	stObjectInfo* nextGridObj = nullptr;

	// [고정 크기 격자 방식]
	stObjectInfo* prevGridObj = nullptr;
};

struct Grid {
	Grid(uint16 rangeY, uint16 rangeX) {
		for (uint16 y = 0; y < CELLS_Y; y++) {
			for (uint16 x = 0; x < CELLS_X; x++) {
				cells[y][x] = nullptr;
			}
		}
	}

	static const uint16 CELLS_Y = dfRANGE_MOVE_BOTTOM / dfGridCell_Length + 1;
	static const uint16 CELLS_X = dfRANGE_MOVE_RIGHT / dfGridCell_Length + 1;
	stObjectInfo* cells[CELLS_Y][CELLS_X];

	void Add(stObjectInfo* obj);
	void Delete(stObjectInfo* obj);
};

enum class enAttackType {
	ATTACK1 = 1,
	ATTACK2,
	ATTACK3
};
struct stAttackWork {
	uint32	uiID;	// 공격 주체
	BYTE	byDir;
	uint16	usX;
	uint16	usY;
	enAttackType	byType;	// 공격 타입
};