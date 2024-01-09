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

	// ������ �̿��ϴ� ������
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
	uint32	uiID;	// ���� ��ü
	BYTE	byDir;
	uint16	usX;
	uint16	usY;
	enAttackType	byType;	// ���� Ÿ��
};