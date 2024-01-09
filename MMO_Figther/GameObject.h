#pragma once
#include "Types.h"

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