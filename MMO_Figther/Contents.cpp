#include "Contents.h"
#include <ctime>

extern FightGameS2C::Proxy g_Proxy;
std::map<HostID, stObjectInfo*> gClientMap;
std::vector<std::vector<stObjectInfo*>> gClientGrid(dfRANGE_MOVE_BOTTOM + 1, std::vector<stObjectInfo*>(dfRANGE_MOVE_RIGHT + 1, nullptr));
std::map<HostID, bool> gDeleteClientSet;
std::queue<stAttackWork> AtkWorkQueue;

Grid gGrid(dfRANGE_MOVE_BOTTOM, dfRANGE_MOVE_RIGHT);

// ������ Ÿ�̸�
time_t gTime;
std::multimap<time_t, HostID> gTimeMap;

// �޸� Ǯ
JiniPool ObjectPool(sizeof(stObjectInfo), 5000);

//===================================================================================================================

//////////////////////////////
// ���� ����(Ŭ���̾�Ʈ ���� �����鸸 �۽�)
// CRT_CHARACTER		(HostID remote, BYTE byCode, BYTE bySize, BYTE byType, uint32_t ID, BYTE Direction, uint16_t X, uint16_t Y, BYTE HP);
// CRT_OTHER_CHARACTER	(HostID remote, BYTE byCode, BYTE bySize, BYTE byType, uint32_t ID, BYTE Direction, uint16_t X, uint16_t Y, BYTE HP);
// DEL_CHARACTER		(HostID remote, BYTE byCode, BYTE bySize, BYTE byType, uint32_t ID);
// MOVE_START			(HostID remote, BYTE byCode, BYTE bySize, BYTE byType, uint32_t ID, BYTE Direction, uint16_t X, uint16_t Y);
// MOVE_STOP			(HostID remote, BYTE byCode, BYTE bySize, BYTE byType, uint32_t ID, BYTE Direction, uint16_t X, uint16_t Y);
// ATTACK1				(HostID remote, BYTE byCode, BYTE bySize, BYTE byType, uint32_t ID, BYTE Direction, uint16_t X, uint16_t Y);
// ATTACK2				(HostID remote, BYTE byCode, BYTE bySize, BYTE byType, uint32_t ID, BYTE Direction, uint16_t X, uint16_t Y);
// ATTACK3				(HostID remote, BYTE byCode, BYTE bySize, BYTE byType, uint32_t ID, BYTE Direction, uint16_t X, uint16_t Y);
// DAMAGE				(HostID remote, BYTE byCode, BYTE bySize, BYTE byType, uint32_t attker, uint32_t target, BYTE targetHP);
//////////////////////////////

//////////////////////////////
// �׸��� ����
//////////////////////////////
void InsertToGrid(uint16_t Y, uint16_t X, HostID id) {
	if (gClientGrid[Y][X] == nullptr) {
		gClientGrid[Y][X] = gClientMap[id];
	}
	else {
		stObjectInfo* beforePtr = gClientGrid[Y][X];
		stObjectInfo* objPtr = gClientGrid[Y][X]->nextGridObj;
		while (true) {
			if (objPtr == nullptr) {
				beforePtr->nextGridObj = gClientMap[id];
				break;
			}
			else {
				beforePtr = objPtr;
				objPtr = objPtr->nextGridObj;
			}
		}
	}
}
void DeleteFromGrid(uint16_t Y, uint16_t X, HostID id) {
	if (gClientGrid[Y][X] == nullptr) {
		return;
	}

	stObjectInfo* objPtr = gClientGrid[Y][X];
	if (objPtr->uiID == id) {
		gClientGrid[Y][X] = objPtr->nextGridObj;
		objPtr->nextGridObj = nullptr;
	}
	else {
		stObjectInfo* beforePtr = objPtr;
		objPtr = objPtr->nextGridObj;

		while (objPtr != nullptr && objPtr->uiID != id) {
			beforePtr = objPtr;
			objPtr = objPtr->nextGridObj;
		}
		if (objPtr != nullptr) {
			beforePtr->nextGridObj = objPtr->nextGridObj;
			objPtr->nextGridObj = nullptr;
		}
	}
}
/*
* Ŭ���̾�Ʈ ��ǥ ����,
* Y - Cell_Length * CellCol / 2 <= Y <= Y + Cell_Length * CellCol
* X - Cell_Length * CellRow / 2 <= X <= X + Cell_Length * CellRow
*/
void FowardCRTMsg(stObjectInfo* newObject) {		// ���� ������ ��ü�� ���� ���� �ڷᱸ���� ��ϵǱ� �� ȣ��ȴٰ� ����
	uint16_t top = newObject->stPos.usY < dfGridCell_Length * dfGridCell_Row / 2 ? dfRANGE_MOVE_TOP : newObject->stPos.usY - dfGridCell_Length * dfGridCell_Row / 2;
	uint16_t bottom = newObject->stPos.usY + dfGridCell_Length * dfGridCell_Row / 2 > dfRANGE_MOVE_BOTTOM ? dfRANGE_MOVE_BOTTOM : newObject->stPos.usY + dfGridCell_Length * dfGridCell_Row / 2;
	uint16_t left = newObject->stPos.usX < dfGridCell_Length * dfGridCell_Col / 2 ? dfRANGE_MOVE_LEFT : newObject->stPos.usX - dfGridCell_Length * dfGridCell_Col / 2;
	uint16_t right = newObject->stPos.usX + dfGridCell_Length * dfGridCell_Col / 2 > dfRANGE_MOVE_RIGHT ? dfRANGE_MOVE_RIGHT : newObject->stPos.usX + dfGridCell_Length * dfGridCell_Col / 2;
	BYTE bodyLen = sizeof(newObject->uiID) + sizeof(newObject->byDir) + sizeof(newObject->stPos) + sizeof(newObject->byHP);
	for (uint16_t y = top; y <= bottom; y++) {
		for (uint16_t x = left; x <= right; x++) {
			for(stObjectInfo* nearPlayer = gClientGrid[y][x]; nearPlayer != nullptr; nearPlayer = nearPlayer->nextGridObj) {
				g_Proxy.CRT_OTHER_CHARACTER(nearPlayer->uiID, VALID_PACKET_NUM, bodyLen, FightGameS2C::RPC_CRT_OTHER_CHARACTER, newObject->uiID, newObject->byDir, newObject->stPos.usX, newObject->stPos.usY, newObject->byHP);
				g_Proxy.CRT_OTHER_CHARACTER(newObject->uiID, VALID_PACKET_NUM, bodyLen, FightGameS2C::RPC_CRT_OTHER_CHARACTER, nearPlayer->uiID, nearPlayer->byDir, nearPlayer->stPos.usX, nearPlayer->stPos.usY, nearPlayer->byHP);
			}
		}
	}
}
void ForwardDmgMsg(stObjectInfo* attacker, stObjectInfo* target) {
	uint16_t minY = attacker->stPos.usY < target->stPos.usY ? attacker->stPos.usY : target->stPos.usY;
	uint16_t maxY = attacker->stPos.usY > target->stPos.usY ? attacker->stPos.usY : target->stPos.usY;
	uint16_t minX = attacker->stPos.usX < target->stPos.usX ? attacker->stPos.usX : target->stPos.usX;
	uint16_t maxX = attacker->stPos.usX > target->stPos.usX ? attacker->stPos.usX : target->stPos.usX;

	uint16_t top = minY < dfGridCell_Length * dfGridCell_Row / 2 ? dfRANGE_MOVE_TOP : minY - dfGridCell_Length * dfGridCell_Row / 2;
	uint16_t bottom = maxY + dfGridCell_Length * dfGridCell_Row / 2 > dfRANGE_MOVE_BOTTOM ? dfRANGE_MOVE_BOTTOM : maxY + dfGridCell_Length * dfGridCell_Row / 2;
	uint16_t left = minX < dfGridCell_Length * dfGridCell_Col / 2 ? dfRANGE_MOVE_LEFT : minX - dfGridCell_Length * dfGridCell_Col / 2;
	uint16_t right = maxX + dfGridCell_Length * dfGridCell_Col / 2 > dfRANGE_MOVE_RIGHT ? dfRANGE_MOVE_RIGHT : maxX + dfGridCell_Length * dfGridCell_Col / 2;

	BYTE bodyLen = sizeof(attacker->uiID) + sizeof(target->uiID) + sizeof(BYTE);
	for (uint16_t y = top; y <= bottom; y++) {
		for (uint16_t x = left; x <= right; x++) {
			for (stObjectInfo* nearPlayer = gClientGrid[y][x]; nearPlayer != nullptr; nearPlayer = nearPlayer->nextGridObj) {
				g_Proxy.DAMAGE(nearPlayer->uiID, VALID_PACKET_NUM, bodyLen, FightGameS2C::RPC_DAMAGE, attacker->uiID, target->uiID, target->byHP);
			}
		}
	}
}
void ForwardMsgToNear(stObjectInfo* player, RpcID msgID) {
	uint16_t top = player->stPos.usY < dfGridCell_Length * dfGridCell_Row / 2 ? dfRANGE_MOVE_TOP : player->stPos.usY - dfGridCell_Length * dfGridCell_Row / 2;
	uint16_t bottom = player->stPos.usY + dfGridCell_Length * dfGridCell_Row / 2 > dfRANGE_MOVE_BOTTOM ? dfRANGE_MOVE_BOTTOM : player->stPos.usY + dfGridCell_Length * dfGridCell_Row / 2;
	uint16_t left = player->stPos.usX < dfGridCell_Length * dfGridCell_Col / 2 ? dfRANGE_MOVE_LEFT : player->stPos.usX - dfGridCell_Length * dfGridCell_Col / 2;
	uint16_t right = player->stPos.usX + dfGridCell_Length * dfGridCell_Col / 2 > dfRANGE_MOVE_RIGHT ? dfRANGE_MOVE_RIGHT : player->stPos.usX + dfGridCell_Length * dfGridCell_Col / 2;

	switch (msgID) {
	case FightGameS2C::RPC_DEL_CHARACTER: {
		BYTE bodyLen = sizeof(player->uiID);
		for (uint16_t y = top; y <= bottom; y++) {
			for (uint16_t x = left; x <= right; x++) {
				for (stObjectInfo* nearPlayer = gClientGrid[y][x]; nearPlayer != nullptr; nearPlayer = nearPlayer->nextGridObj) {
					if (nearPlayer->uiID != player->uiID) {		// ���� ��󿡴� ������ ��� ����
						g_Proxy.DEL_CHARACTER(nearPlayer->uiID, VALID_PACKET_NUM, bodyLen, FightGameS2C::RPC_DEL_CHARACTER, player->uiID);
					}
				}
			}
		}
		break;
	}
	case FightGameS2C::RPC_MOVE_START: {
		BYTE bodyLen = sizeof(player->uiID) + sizeof(player->byDir) + sizeof(player->stPos.usX) + sizeof(player->stPos.usY);
		for (uint16_t y = top; y <= bottom; y++) {
			for (uint16_t x = left; x <= right; x++) {
				for (stObjectInfo* nearPlayer = gClientGrid[y][x]; nearPlayer != nullptr; nearPlayer = nearPlayer->nextGridObj) {
					if (nearPlayer->uiID != player->uiID) {	// �̵� ����� �޽��� ������ ��� ����						
						//std::cout << "[Forward Start] X: " << x << ", Y: " << y << std::endl;
						g_Proxy.MOVE_START(nearPlayer->uiID, VALID_PACKET_NUM, bodyLen, FightGameS2C::RPC_MOVE_START, player->uiID, player->byDir, player->stPos.usX, player->stPos.usY);
					}
				}
			}
		}
		break;
	}
	case FightGameS2C::RPC_MOVE_STOP: {
		BYTE bodyLen = sizeof(player->uiID) + sizeof(player->byDir) + sizeof(player->stPos.usX) + sizeof(player->stPos.usY);
		for (uint16_t y = top; y <= bottom; y++) {
			for (uint16_t x = left; x <= right; x++) {
				for (stObjectInfo* nearPlayer = gClientGrid[y][x]; nearPlayer != nullptr; nearPlayer = nearPlayer->nextGridObj) {
					if (nearPlayer->uiID != player->uiID) {	// �̵� ����� �޽��� ������ ��� ����
						//std::cout << "[Forward Stop] X: " << x << ", Y: " << y << std::endl;
						g_Proxy.MOVE_STOP(nearPlayer->uiID, VALID_PACKET_NUM, bodyLen, FightGameS2C::RPC_MOVE_STOP, player->uiID, player->byDir, player->stPos.usX, player->stPos.usY);
					}
				}
			}
		}
		break;
	}
	case FightGameS2C::RPC_ATTACK1: {
		BYTE bodyLen = sizeof(player->uiID) + sizeof(player->byDir) + sizeof(player->stPos.usX) + sizeof(player->stPos.usY);
		for (uint16_t y = top; y <= bottom; y++) {
			for (uint16_t x = left; x <= right; x++) {
				for (stObjectInfo* nearPlayer = gClientGrid[y][x]; nearPlayer != nullptr; nearPlayer = nearPlayer->nextGridObj) {
					if (nearPlayer->uiID != player->uiID) {	// �����ڴ� �޽��� ������ ��� ����
						g_Proxy.ATTACK1(nearPlayer->uiID, VALID_PACKET_NUM, bodyLen, FightGameS2C::RPC_ATTACK1, player->uiID, player->byDir, player->stPos.usX, player->stPos.usY);
					}
				}
			}
		}
		break;
	}
	case FightGameS2C::RPC_ATTACK2: {
		BYTE bodyLen = sizeof(player->uiID) + sizeof(player->byDir) + sizeof(player->stPos.usX) + sizeof(player->stPos.usY);
		for (uint16_t y = top; y <= bottom; y++) {
			for (uint16_t x = left; x <= right; x++) {
				for (stObjectInfo* nearPlayer = gClientGrid[y][x]; nearPlayer != nullptr; nearPlayer = nearPlayer->nextGridObj) {
					if (nearPlayer->uiID != player->uiID) {	// �����ڴ� �޽��� ������ ��� ����
						g_Proxy.ATTACK2(nearPlayer->uiID, VALID_PACKET_NUM, bodyLen, FightGameS2C::RPC_ATTACK2, player->uiID, player->byDir, player->stPos.usX, player->stPos.usY);
					}
				}
			}
		}
		break;
	}
	case FightGameS2C::RPC_ATTACK3: {
		BYTE bodyLen = sizeof(player->uiID) + sizeof(player->byDir) + sizeof(player->stPos.usX) + sizeof(player->stPos.usY);
		for (uint16_t y = top; y <= bottom; y++) {
			for (uint16_t x = left; x <= right; x++) {
				for (stObjectInfo* nearPlayer = gClientGrid[y][x]; nearPlayer != nullptr; nearPlayer = nearPlayer->nextGridObj) {
					if (nearPlayer->uiID != player->uiID) {	// �����ڴ� �޽��� ������ ��� ����
						g_Proxy.ATTACK3(nearPlayer->uiID, VALID_PACKET_NUM, bodyLen, FightGameS2C::RPC_ATTACK3, player->uiID, player->byDir, player->stPos.usX, player->stPos.usY);
					}
				}
			}
		}
		break;
	}
	}
}
void FowardCrtDelMsgByMove(stObjectInfo* player, const stPoint& beforePos) {
	if (player->stPos.usY == beforePos.usY && player->stPos.usX == beforePos.usX) {
		return;
	}

	BYTE crtBodyLen = sizeof(player->uiID) + sizeof(player->byDir) + sizeof(player->stPos) + sizeof(player->byHP);
	BYTE moveBodyLen = sizeof(player->uiID) + sizeof(player->byDir) + sizeof(player->stPos);

	uint16_t top = player->stPos.usY < dfGridCell_Length * dfGridCell_Row / 2 ? dfRANGE_MOVE_TOP : player->stPos.usY - dfGridCell_Length * dfGridCell_Row / 2;
	uint16_t bottom = player->stPos.usY + dfGridCell_Length * dfGridCell_Row / 2 > dfRANGE_MOVE_BOTTOM ? dfRANGE_MOVE_BOTTOM : player->stPos.usY + dfGridCell_Length * dfGridCell_Row / 2;
	uint16_t left = player->stPos.usX < dfGridCell_Length * dfGridCell_Col / 2 ? dfRANGE_MOVE_LEFT : player->stPos.usX - dfGridCell_Length * dfGridCell_Col / 2;
	uint16_t right = player->stPos.usX + dfGridCell_Length * dfGridCell_Col / 2 > dfRANGE_MOVE_RIGHT ? dfRANGE_MOVE_RIGHT : player->stPos.usX + dfGridCell_Length * dfGridCell_Col / 2;

	uint16_t topBefore = beforePos.usY < dfGridCell_Length * dfGridCell_Row / 2 ? dfRANGE_MOVE_TOP : beforePos.usY - dfGridCell_Length * dfGridCell_Row / 2;
	uint16_t bottomBefore = beforePos.usY + dfGridCell_Length * dfGridCell_Row / 2 > dfRANGE_MOVE_BOTTOM ? dfRANGE_MOVE_BOTTOM : beforePos.usY + dfGridCell_Length * dfGridCell_Row / 2;
	uint16_t leftBefore = beforePos.usX < dfGridCell_Length * dfGridCell_Col / 2 ? dfRANGE_MOVE_LEFT : beforePos.usX - dfGridCell_Length * dfGridCell_Col / 2;
	uint16_t rightBefore = beforePos.usX + dfGridCell_Length * dfGridCell_Col / 2 > dfRANGE_MOVE_RIGHT ? dfRANGE_MOVE_RIGHT : beforePos.usX + dfGridCell_Length * dfGridCell_Col / 2;

	uint16_t delLeft = 0;
	uint16_t delRight = 0;
	uint16_t crtLeft = 0;
	uint16_t crtRight = 0;
	uint16_t delTop = 0;
	uint16_t delBottom = 0;
	uint16_t crtTop = 0;
	uint16_t crtBottom = 0;

	if (beforePos.usX < player->stPos.usX) {
		delLeft = leftBefore;
		delRight = left;
		crtLeft = rightBefore;
		crtRight = right;
	}
	else if (beforePos.usX > player->stPos.usX) {
		delLeft = right;
		delRight = rightBefore;
		crtLeft = left;
		crtRight = leftBefore;
	}

	if (beforePos.usY < player->stPos.usY) {
		delTop = topBefore;
		delBottom = top;
		crtTop = bottomBefore;
		crtBottom = bottom;
	}
	else if (beforePos.usY > player->stPos.usY) {
		delTop = bottom;
		delBottom = bottomBefore;
		crtTop = top;
		crtBottom = topBefore;
	}

	for (uint16_t y = topBefore; y <= bottomBefore; y++) {
		// x ���� ����
		for (uint16_t x = delLeft; x < delRight; x++) {
			for (stObjectInfo* nearPlayer = gClientGrid[y][x]; nearPlayer != nullptr; nearPlayer = nearPlayer->nextGridObj) {
				if (nearPlayer->uiID != player->uiID) {		// ���� ��󿡴� ������ ��� ����
					g_Proxy.DEL_CHARACTER(nearPlayer->uiID, VALID_PACKET_NUM, sizeof(player->uiID), FightGameS2C::RPC_DEL_CHARACTER, player->uiID);
					g_Proxy.DEL_CHARACTER(player->uiID, VALID_PACKET_NUM, sizeof(player->uiID), FightGameS2C::RPC_DEL_CHARACTER, nearPlayer->uiID);
				}
			}
		}
	}
	for (uint16_t y = top; y <= bottom; y++) {
		// x ���� ����
		for (uint16_t x = crtLeft; x < crtRight; x++) {
			for (stObjectInfo* nearPlayer = gClientGrid[y][x]; nearPlayer != nullptr; nearPlayer = nearPlayer->nextGridObj) {
				if (nearPlayer->uiID != player->uiID) {		// ���� ��󿡴� ������ ��� ����
					g_Proxy.CRT_OTHER_CHARACTER(nearPlayer->uiID, VALID_PACKET_NUM, crtBodyLen, FightGameS2C::RPC_CRT_OTHER_CHARACTER, player->uiID, player->byDir, player->stPos.usX, player->stPos.usY, player->byHP);
					g_Proxy.MOVE_START(nearPlayer->uiID, VALID_PACKET_NUM, moveBodyLen, FightGameS2C::RPC_MOVE_START, player->uiID, player->byDir, player->stPos.usX, player->stPos.usY);
					g_Proxy.CRT_OTHER_CHARACTER(player->uiID, VALID_PACKET_NUM, crtBodyLen, FightGameS2C::RPC_CRT_OTHER_CHARACTER, nearPlayer->uiID, nearPlayer->byDir, nearPlayer->stPos.usX, nearPlayer->stPos.usY, nearPlayer->byHP);
				}
			}
		}
	}

	for (uint16_t x = leftBefore; x <= rightBefore; x++) {
		// y ���� ����
		for (uint16_t y = delTop; y < delBottom; y++) {
			for (stObjectInfo* nearPlayer = gClientGrid[y][x]; nearPlayer != nullptr; nearPlayer = nearPlayer->nextGridObj) {
				if (nearPlayer->uiID != player->uiID) {		// ���� ��󿡴� ������ ��� ����
					g_Proxy.DEL_CHARACTER(nearPlayer->uiID, VALID_PACKET_NUM, sizeof(player->uiID), FightGameS2C::RPC_DEL_CHARACTER, player->uiID);
					g_Proxy.DEL_CHARACTER(player->uiID, VALID_PACKET_NUM, sizeof(player->uiID), FightGameS2C::RPC_DEL_CHARACTER, nearPlayer->uiID);
				}
			}
		}
	}
	for (uint16_t x = left; x <= right; x++) {
		// y ���� ����
		for (uint16_t y = crtTop; y < crtBottom; y++) {
			for (stObjectInfo* nearPlayer = gClientGrid[y][x]; nearPlayer != nullptr; nearPlayer = nearPlayer->nextGridObj) {
				if (nearPlayer->uiID != player->uiID) {		// ���� ��󿡴� ������ ��� ����
					g_Proxy.CRT_OTHER_CHARACTER(nearPlayer->uiID, VALID_PACKET_NUM, crtBodyLen, FightGameS2C::RPC_CRT_OTHER_CHARACTER, player->uiID, player->byDir, player->stPos.usX, player->stPos.usY, player->byHP);
					g_Proxy.MOVE_START(nearPlayer->uiID, VALID_PACKET_NUM, moveBodyLen, FightGameS2C::RPC_MOVE_START, player->uiID, player->byDir, player->stPos.usX, player->stPos.usY);
					g_Proxy.CRT_OTHER_CHARACTER(player->uiID, VALID_PACKET_NUM, crtBodyLen, FightGameS2C::RPC_CRT_OTHER_CHARACTER, nearPlayer->uiID, nearPlayer->byDir, nearPlayer->stPos.usX, nearPlayer->stPos.usY, nearPlayer->byHP);
				}
			}
		}
	}
}

void GetRangeCell(const stPoint& objPos, uint16& topCell, uint16& bottomCell, uint16& leftCell, uint16& rightCell) {
	uint16 cellY = objPos.usY / dfGridCell_Length;
	uint16 cellX = objPos.usX / dfGridCell_Length;

	if (cellY < dfGridCell_Row / 2) {
		topCell = dfRANGE_GRID_TOP;
		bottomCell = dfRANGE_GRID_TOP + dfGridCell_Row;
	}
	else if (dfRANGE_GRID_BOTTOM - dfGridCell_Row / 2 < cellY) {
		topCell = dfRANGE_GRID_BOTTOM - dfGridCell_Row;
		bottomCell = dfRANGE_GRID_BOTTOM;
	}
	else {
		topCell = cellY - dfGridCell_Row / 2;
		bottomCell = cellY + dfGridCell_Row / 2;
	}

	if (cellX < dfGridCell_Col / 2) {
		leftCell = dfRANGE_GRID_LEFT;
		rightCell = dfRANGE_GRID_LEFT + dfGridCell_Col;
	}
	else if (dfRANGE_GRID_RIGHT - dfGridCell_Col / 2 < cellX) {
		leftCell = dfRANGE_GRID_RIGHT - dfGridCell_Col;
		rightCell = dfRANGE_GRID_RIGHT;
	}
	else {
		leftCell = cellX - dfGridCell_Col / 2;
		rightCell = cellX + dfGridCell_Col / 2;
	}
}
void GridResetInterestSpace(stPoint beforePos, stObjectInfo* object, Grid* grid, bool crtFlag) {
	uint16 topCellBefore;
	uint16 bottomCellBefore;
	uint16 leftCellBefore;
	uint16 rightCellBefore;
	GetRangeCell(beforePos, topCellBefore, bottomCellBefore, leftCellBefore, rightCellBefore);

	uint16 topCell;
	uint16 bottomCell;
	uint16 leftCell;
	uint16 rightCell;
	GetRangeCell(object->stPos, topCell, bottomCell, leftCell, rightCell);

	BYTE delBodyLen = sizeof(object->uiID);
	BYTE crtBodyLen = sizeof(object->uiID) + sizeof(object->byDir) + sizeof(object->stPos) + sizeof(object->byHP);
	BYTE moveBodyLen = sizeof(object->uiID) + sizeof(object->byDir) + sizeof(object->stPos);

	std::pair<bool, uint16> delRow = { false, 0 };
	std::pair<bool, uint16> delCol = { false, 0 };
	std::pair<bool, uint16> crtRow = { false, 0 };
	std::pair<bool, uint16> crtCol = { false, 0 };
	if (leftCellBefore < leftCell) {
		// ����: |----------?
		// �̵�:  |=========?
		delCol.first = true;
		delCol.second = leftCellBefore;
	}
	else if (leftCellBefore > leftCell) {
		// ����:  |---------?
		// �̵�: |==========?
		crtCol.first = true;
		crtCol.second = leftCell;
	}
	if (rightCellBefore < rightCell) {
		// ����: ?---------|
		// �̵�: ?==========|
		crtCol.first = true;
		crtCol.second = rightCell;
	}
	else if (rightCellBefore > rightCell) {
		// ����: ?----------|
		// �̵�: ?=========|
		delCol.first = true;
		delCol.second = rightCellBefore;
	}

	if (topCellBefore < topCell) {
		delRow.first = true;
		delRow.second = topCellBefore;
	}
	else if (topCellBefore > topCell) {
		crtRow.first = true;
		crtRow.second = topCell;
	}
	if (bottomCellBefore < bottomCell) {
		crtRow.first = true;
		crtRow.second = bottomCell;
	}
	else if (bottomCellBefore > bottomCell) {
		delRow.first = true;
		delRow.second = bottomCellBefore;
	}

	if (delRow.first && delCol.first) {	// �� �� . . ����
		// (1) ������ ����
		for (stObjectInfo* nearPlayer = grid->cells[delRow.second][delCol.second]; nearPlayer != nullptr; nearPlayer = nearPlayer->nextGridObj) {
			if (nearPlayer->uiID != object->uiID) {		// ���� ��󿡴� ������ ��� ����
				g_Proxy.DEL_CHARACTER(nearPlayer->uiID, VALID_PACKET_NUM, delBodyLen, FightGameS2C::RPC_DEL_CHARACTER, object->uiID);
				g_Proxy.DEL_CHARACTER(object->uiID, VALID_PACKET_NUM, delBodyLen, FightGameS2C::RPC_DEL_CHARACTER, nearPlayer->uiID);
			}
		}
		// (2-1) �� �� ����
		for (uint16 cx = leftCellBefore; cx <= rightCellBefore; cx++) {
			if (cx != delCol.second) {
				uint16 cy = delRow.second;
				for (stObjectInfo* nearPlayer = grid->cells[cy][cx]; nearPlayer != nullptr; nearPlayer = nearPlayer->nextGridObj) {
					if (nearPlayer->uiID != object->uiID) {		// ���� ��󿡴� ������ ��� ����
						g_Proxy.DEL_CHARACTER(nearPlayer->uiID, VALID_PACKET_NUM, delBodyLen, FightGameS2C::RPC_DEL_CHARACTER, object->uiID);
						g_Proxy.DEL_CHARACTER(object->uiID, VALID_PACKET_NUM, delBodyLen, FightGameS2C::RPC_DEL_CHARACTER, nearPlayer->uiID);
					}
				}
			}
		}
		// (2-1) | �� ����
		for (uint16 cy = topCellBefore; cy <= bottomCellBefore; cy++) {
			if (cy != delRow.second) {
				uint16 cx = delCol.second;
				for (stObjectInfo* nearPlayer = grid->cells[cy][cx]; nearPlayer != nullptr; nearPlayer = nearPlayer->nextGridObj) {
					if (nearPlayer->uiID != object->uiID) {		// ���� ��󿡴� ������ ��� ����
						g_Proxy.DEL_CHARACTER(nearPlayer->uiID, VALID_PACKET_NUM, delBodyLen, FightGameS2C::RPC_DEL_CHARACTER, object->uiID);
						g_Proxy.DEL_CHARACTER(object->uiID, VALID_PACKET_NUM, delBodyLen, FightGameS2C::RPC_DEL_CHARACTER, nearPlayer->uiID);
					}
				}
			}
		}
	}
	else if (delRow.first) {	// �� ����
		for (uint16 cx = leftCellBefore; cx <= rightCellBefore; cx++) {
			uint16 cy = delRow.second;
			for (stObjectInfo* nearPlayer = grid->cells[cy][cx]; nearPlayer != nullptr; nearPlayer = nearPlayer->nextGridObj) {
				if (nearPlayer->uiID != object->uiID) {		// ���� ��󿡴� ������ ��� ����
					g_Proxy.DEL_CHARACTER(nearPlayer->uiID, VALID_PACKET_NUM, delBodyLen, FightGameS2C::RPC_DEL_CHARACTER, object->uiID);
					g_Proxy.DEL_CHARACTER(object->uiID, VALID_PACKET_NUM, delBodyLen, FightGameS2C::RPC_DEL_CHARACTER, nearPlayer->uiID);
				}
			}
		}
	}
	else if (delCol.first) {	// | ����
		for (uint16 cy = topCellBefore; cy <= bottomCellBefore; cy++) {
			uint16 cx = delCol.second;
			for (stObjectInfo* nearPlayer = grid->cells[cy][cx]; nearPlayer != nullptr; nearPlayer = nearPlayer->nextGridObj) {
				if (nearPlayer->uiID != object->uiID) {		// ���� ��󿡴� ������ ��� ����
					g_Proxy.DEL_CHARACTER(nearPlayer->uiID, VALID_PACKET_NUM, delBodyLen, FightGameS2C::RPC_DEL_CHARACTER, object->uiID);
					g_Proxy.DEL_CHARACTER(object->uiID, VALID_PACKET_NUM, delBodyLen, FightGameS2C::RPC_DEL_CHARACTER, nearPlayer->uiID);
				}
			}
		}
	}

	if (crtRow.first && crtCol.first) {	// �� �� . . ����
		// (1) ������ ����
		for (stObjectInfo* nearPlayer = grid->cells[crtRow.second][crtCol.second]; nearPlayer != nullptr; nearPlayer = nearPlayer->nextGridObj) {
			if (nearPlayer->uiID != object->uiID) {
				g_Proxy.CRT_OTHER_CHARACTER(nearPlayer->uiID, VALID_PACKET_NUM, crtBodyLen, FightGameS2C::RPC_CRT_OTHER_CHARACTER, object->uiID, object->byDir, object->stPos.usX, object->stPos.usY, object->byHP);
				g_Proxy.MOVE_START(nearPlayer->uiID, VALID_PACKET_NUM, moveBodyLen, FightGameS2C::RPC_MOVE_START, object->uiID, object->byDir, object->stPos.usX, object->stPos.usY);
				g_Proxy.CRT_OTHER_CHARACTER(object->uiID, VALID_PACKET_NUM, crtBodyLen, FightGameS2C::RPC_CRT_OTHER_CHARACTER, nearPlayer->uiID, nearPlayer->byDir, nearPlayer->stPos.usX, nearPlayer->stPos.usY, nearPlayer->byHP);
			}
		}
		// (2-1) �� �� ����
		for (uint16 cx = leftCell; cx <= rightCell; cx++) {
			if (cx != crtCol.second) {
				uint16 cy = crtRow.second;
				for (stObjectInfo* nearPlayer = grid->cells[cy][cx]; nearPlayer != nullptr; nearPlayer = nearPlayer->nextGridObj) {
					if (nearPlayer->uiID != object->uiID) {
						g_Proxy.CRT_OTHER_CHARACTER(nearPlayer->uiID, VALID_PACKET_NUM, crtBodyLen, FightGameS2C::RPC_CRT_OTHER_CHARACTER, object->uiID, object->byDir, object->stPos.usX, object->stPos.usY, object->byHP);
						g_Proxy.MOVE_START(nearPlayer->uiID, VALID_PACKET_NUM, moveBodyLen, FightGameS2C::RPC_MOVE_START, object->uiID, object->byDir, object->stPos.usX, object->stPos.usY);
						g_Proxy.CRT_OTHER_CHARACTER(object->uiID, VALID_PACKET_NUM, crtBodyLen, FightGameS2C::RPC_CRT_OTHER_CHARACTER, nearPlayer->uiID, nearPlayer->byDir, nearPlayer->stPos.usX, nearPlayer->stPos.usY, nearPlayer->byHP);
					}
				}
			}
		}
		// (2-1) | �� ����
		for (uint16 cy = topCell; cy <= bottomCell; cy++) {
			if (cy != crtRow.second) {
				uint16 cx = crtCol.second;
				for (stObjectInfo* nearPlayer = grid->cells[cy][cx]; nearPlayer != nullptr; nearPlayer = nearPlayer->nextGridObj) {
					if (nearPlayer->uiID != object->uiID) {		// ���� ��󿡴� ������ ��� ����
						g_Proxy.CRT_OTHER_CHARACTER(nearPlayer->uiID, VALID_PACKET_NUM, crtBodyLen, FightGameS2C::RPC_CRT_OTHER_CHARACTER, object->uiID, object->byDir, object->stPos.usX, object->stPos.usY, object->byHP);
						g_Proxy.MOVE_START(nearPlayer->uiID, VALID_PACKET_NUM, moveBodyLen, FightGameS2C::RPC_MOVE_START, object->uiID, object->byDir, object->stPos.usX, object->stPos.usY);
						g_Proxy.CRT_OTHER_CHARACTER(object->uiID, VALID_PACKET_NUM, crtBodyLen, FightGameS2C::RPC_CRT_OTHER_CHARACTER, nearPlayer->uiID, nearPlayer->byDir, nearPlayer->stPos.usX, nearPlayer->stPos.usY, nearPlayer->byHP);
					}
				}
			}
		}
	}
	else if (crtRow.first) {	// �� ����
		for (uint16 cx = leftCell; cx <= rightCell; cx++) {
			uint16 cy = crtRow.second;
			for (stObjectInfo* nearPlayer = grid->cells[cy][cx]; nearPlayer != nullptr; nearPlayer = nearPlayer->nextGridObj) {
				if (nearPlayer->uiID != object->uiID) {
					g_Proxy.CRT_OTHER_CHARACTER(nearPlayer->uiID, VALID_PACKET_NUM, crtBodyLen, FightGameS2C::RPC_CRT_OTHER_CHARACTER, object->uiID, object->byDir, object->stPos.usX, object->stPos.usY, object->byHP);
					g_Proxy.MOVE_START(nearPlayer->uiID, VALID_PACKET_NUM, moveBodyLen, FightGameS2C::RPC_MOVE_START, object->uiID, object->byDir, object->stPos.usX, object->stPos.usY);
					g_Proxy.CRT_OTHER_CHARACTER(object->uiID, VALID_PACKET_NUM, crtBodyLen, FightGameS2C::RPC_CRT_OTHER_CHARACTER, nearPlayer->uiID, nearPlayer->byDir, nearPlayer->stPos.usX, nearPlayer->stPos.usY, nearPlayer->byHP);
				}
			}
		}
	}
	else if (crtCol.first) {	// | ����
		for (uint16 cy = topCell; cy <= bottomCell; cy++) {
			uint16 cx = crtCol.second;
			for (stObjectInfo* nearPlayer = grid->cells[cy][cx]; nearPlayer != nullptr; nearPlayer = nearPlayer->nextGridObj) {
				if (nearPlayer->uiID != object->uiID) {		// ���� ��󿡴� ������ ��� ����
					g_Proxy.CRT_OTHER_CHARACTER(nearPlayer->uiID, VALID_PACKET_NUM, crtBodyLen, FightGameS2C::RPC_CRT_OTHER_CHARACTER, object->uiID, object->byDir, object->stPos.usX, object->stPos.usY, object->byHP);
					g_Proxy.MOVE_START(nearPlayer->uiID, VALID_PACKET_NUM, moveBodyLen, FightGameS2C::RPC_MOVE_START, object->uiID, object->byDir, object->stPos.usX, object->stPos.usY);
					g_Proxy.CRT_OTHER_CHARACTER(object->uiID, VALID_PACKET_NUM, crtBodyLen, FightGameS2C::RPC_CRT_OTHER_CHARACTER, nearPlayer->uiID, nearPlayer->byDir, nearPlayer->stPos.usX, nearPlayer->stPos.usY, nearPlayer->byHP);
				}
			}
		}
	}

	//uint16 delTopCell = 0;
	//uint16 delBottomCell = 0;
	//uint16 delLeftCell = 0;
	//uint16 delRightCell = 0;
	//uint16 crtTopCell = 0;
	//uint16 crtBottomCell = 0;
	//uint16 crtLeftCell = 0;
	//uint16 crtRightCell = 0;
	//
	//if (topCellBefore < topCell) {
	//	delTopCell = topCellBefore;
	//	delBottomCell = topCell;
	//	crtTopCell = bottomCellBefore;
	//	crtBottomCell = bottomCell;
	//}
	//else if (topCellBefore > topCell) {
	//	delTopCell = bottomCell;
	//	delBottomCell = bottomCellBefore;
	//	crtTopCell = topCell;
	//	crtBottomCell = topCellBefore;
	//}
	//
	//if (leftCellBefore < leftCell) {
	//	delLeftCell = leftCellBefore;
	//	delRightCell = leftCell;
	//	crtLeftCell = rightCellBefore;
	//	crtRightCell = rightCell;
	//}
	//else if (leftCellBefore > leftCell) {
	//	delLeftCell = rightCell;
	//	delRightCell = rightCellBefore;
	//	crtLeftCell = leftCell;
	//	crtRightCell = leftCellBefore;
	//}
	//
	//for (uint16 cy = topCellBefore; cy <= bottomCellBefore; cy++) {
	//	for (uint16 cx = delLeftCell; cx <= delRightCell; cx++) {
	//		for (stObjectInfo* nearPlayer = grid->cells[cy][cx]; nearPlayer != nullptr; nearPlayer = nearPlayer->nextGridObj) {
	//			if (nearPlayer->uiID != object->uiID) {		// ���� ��󿡴� ������ ��� ����
	//				g_Proxy.DEL_CHARACTER(nearPlayer->uiID, VALID_PACKET_NUM, delBodyLen, FightGameS2C::RPC_DEL_CHARACTER, object->uiID);
	//				g_Proxy.DEL_CHARACTER(object->uiID, VALID_PACKET_NUM, delBodyLen, FightGameS2C::RPC_DEL_CHARACTER, nearPlayer->uiID);
	//			}
	//		}
	//	}
	//}
	//if (crtFlag) {
	//	for (uint16 cy = topCell; cy <= bottomCell; cy++) {
	//		for (uint16 cx = crtLeftCell; cx <= crtRightCell; cx++) {
	//			for (stObjectInfo* nearPlayer = grid->cells[cy][cx]; nearPlayer != nullptr; nearPlayer = nearPlayer->nextGridObj) {
	//				if (nearPlayer->uiID != object->uiID) {	
	//					g_Proxy.CRT_OTHER_CHARACTER(nearPlayer->uiID, VALID_PACKET_NUM, crtBodyLen, FightGameS2C::RPC_CRT_OTHER_CHARACTER, object->uiID, object->byDir, object->stPos.usX, object->stPos.usY, object->byHP);
	//					g_Proxy.MOVE_START(nearPlayer->uiID, VALID_PACKET_NUM, moveBodyLen, FightGameS2C::RPC_MOVE_START, object->uiID, object->byDir, object->stPos.usX, object->stPos.usY);
	//					g_Proxy.CRT_OTHER_CHARACTER(object->uiID, VALID_PACKET_NUM, crtBodyLen, FightGameS2C::RPC_CRT_OTHER_CHARACTER, nearPlayer->uiID, nearPlayer->byDir, nearPlayer->stPos.usX, nearPlayer->stPos.usY, nearPlayer->byHP);
	//				}
	//			}
	//		}
	//	}
	//}
	//for (uint16 cx = leftCellBefore; cx <= rightCellBefore; cx++) {
	//	for (uint16 cy = delTopCell; cy <= delBottomCell; cy++) {
	//		for (stObjectInfo* nearPlayer = grid->cells[cy][cx]; nearPlayer != nullptr; nearPlayer = nearPlayer->nextGridObj) {
	//			if (nearPlayer->uiID != object->uiID) {		// ���� ��󿡴� ������ ��� ����
	//				g_Proxy.DEL_CHARACTER(nearPlayer->uiID, VALID_PACKET_NUM, delBodyLen, FightGameS2C::RPC_DEL_CHARACTER, object->uiID);
	//				g_Proxy.DEL_CHARACTER(object->uiID, VALID_PACKET_NUM, delBodyLen, FightGameS2C::RPC_DEL_CHARACTER, nearPlayer->uiID);
	//			}
	//		}
	//	}
	//}
	//if (crtFlag) {
	//	for (uint16 cx = leftCell; cx <= rightCell; cx++) {
	//		for (uint16 cy = crtTopCell; cy <= crtBottomCell; cy++) {
	//			for (stObjectInfo* nearPlayer = grid->cells[cy][cx]; nearPlayer != nullptr; nearPlayer = nearPlayer->nextGridObj) {
	//				if (nearPlayer->uiID != object->uiID) {		// ���� ��󿡴� ������ ��� ����
	//					g_Proxy.CRT_OTHER_CHARACTER(nearPlayer->uiID, VALID_PACKET_NUM, crtBodyLen, FightGameS2C::RPC_CRT_OTHER_CHARACTER, object->uiID, object->byDir, object->stPos.usX, object->stPos.usY, object->byHP);
	//					g_Proxy.MOVE_START(nearPlayer->uiID, VALID_PACKET_NUM, moveBodyLen, FightGameS2C::RPC_MOVE_START, object->uiID, object->byDir, object->stPos.usX, object->stPos.usY);
	//					g_Proxy.CRT_OTHER_CHARACTER(object->uiID, VALID_PACKET_NUM, crtBodyLen, FightGameS2C::RPC_CRT_OTHER_CHARACTER, nearPlayer->uiID, nearPlayer->byDir, nearPlayer->stPos.usX, nearPlayer->stPos.usY, nearPlayer->byHP);
	//				}
	//			}
	//		}
	//	}
	//}
}
void ForwardMsgToNear(stObjectInfo* object, Grid* grid, RpcID msgID) {
	uint16 topCell;
	uint16 bottomCell;
	uint16 leftCell;
	uint16 rightCell;
	GetRangeCell(object->stPos, topCell, bottomCell, leftCell, rightCell);

	switch (msgID) {
	case FightGameS2C::RPC_DEL_CHARACTER: {
		BYTE bodyLen = sizeof(object->uiID);
		for (uint16_t cy = topCell; cy <= bottomCell; cy++) {
			for (uint16_t cx = leftCell; cx <= rightCell; cx++) {
				for (stObjectInfo* nearPlayer = grid->cells[cy][cx]; nearPlayer != nullptr; nearPlayer = nearPlayer->nextGridObj) {
					//if (nearPlayer->uiID != object->uiID) {		// ���� ��󿡴� ������ ��� ����
						g_Proxy.DEL_CHARACTER(nearPlayer->uiID, VALID_PACKET_NUM, bodyLen, FightGameS2C::RPC_DEL_CHARACTER, object->uiID);
					//}
					// => Ŭ���̾�Ʈ ���� �м� ��� ���� ��󿡵� delete �޽����� �����־���� ����
				}
			}
		}
		break;
	}
	case FightGameS2C::RPC_MOVE_START: {
		BYTE bodyLen = sizeof(object->uiID) + sizeof(object->byDir) + sizeof(object->stPos.usX) + sizeof(object->stPos.usY);
		for (uint16_t cy = topCell; cy <= bottomCell; cy++) {
			for (uint16_t cx = leftCell; cx <= rightCell; cx++) {
				for (stObjectInfo* nearPlayer = grid->cells[cy][cx]; nearPlayer != nullptr; nearPlayer = nearPlayer->nextGridObj) {
					if (nearPlayer->uiID != object->uiID) {		// ���� ��󿡴� ������ ��� ����
						//std::cout << "[Forward Start] X: " << x << ", Y: " << y << std::endl;
						g_Proxy.MOVE_START(nearPlayer->uiID, VALID_PACKET_NUM, bodyLen, FightGameS2C::RPC_MOVE_START, object->uiID, object->byDir, object->stPos.usX, object->stPos.usY);
					}
				}
			}
		}
		break;
	}
	case FightGameS2C::RPC_MOVE_STOP: {
		BYTE bodyLen = sizeof(object->uiID) + sizeof(object->byDir) + sizeof(object->stPos.usX) + sizeof(object->stPos.usY);
		for (uint16_t cy = topCell; cy <= bottomCell; cy++) {
			for (uint16_t cx = leftCell; cx <= rightCell; cx++) {
				for (stObjectInfo* nearPlayer = grid->cells[cy][cx]; nearPlayer != nullptr; nearPlayer = nearPlayer->nextGridObj) {
					if (nearPlayer->uiID != object->uiID) {		// ���� ��󿡴� ������ ��� ����
						//std::cout << "[Forward Stop] X: " << x << ", Y: " << y << std::endl;
						g_Proxy.MOVE_STOP(nearPlayer->uiID, VALID_PACKET_NUM, bodyLen, FightGameS2C::RPC_MOVE_STOP, object->uiID, object->byDir, object->stPos.usX, object->stPos.usY);
					}
				}
			}
		}
		break;
	}
	case FightGameS2C::RPC_ATTACK1: {
		BYTE bodyLen = sizeof(object->uiID) + sizeof(object->byDir) + sizeof(object->stPos.usX) + sizeof(object->stPos.usY);
		for (uint16_t cy = topCell; cy <= bottomCell; cy++) {
			for (uint16_t cx = leftCell; cx <= rightCell; cx++) {
				for (stObjectInfo* nearPlayer = grid->cells[cy][cx]; nearPlayer != nullptr; nearPlayer = nearPlayer->nextGridObj) {
					if (nearPlayer->uiID != object->uiID) {		// ���� ��󿡴� ������ ��� ����
						g_Proxy.ATTACK1(nearPlayer->uiID, VALID_PACKET_NUM, bodyLen, FightGameS2C::RPC_ATTACK1, object->uiID, object->byDir, object->stPos.usX, object->stPos.usY);
					}
				}
			}
		}
		break;
	}
	case FightGameS2C::RPC_ATTACK2: {
		BYTE bodyLen = sizeof(object->uiID) + sizeof(object->byDir) + sizeof(object->stPos.usX) + sizeof(object->stPos.usY);
		for (uint16_t cy = topCell; cy <= bottomCell; cy++) {
			for (uint16_t cx = leftCell; cx <= rightCell; cx++) {
				for (stObjectInfo* nearPlayer = grid->cells[cy][cx]; nearPlayer != nullptr; nearPlayer = nearPlayer->nextGridObj) {
					if (nearPlayer->uiID != object->uiID) {		// ���� ��󿡴� ������ ��� ����
						g_Proxy.ATTACK2(nearPlayer->uiID, VALID_PACKET_NUM, bodyLen, FightGameS2C::RPC_ATTACK2, object->uiID, object->byDir, object->stPos.usX, object->stPos.usY);
					}
				}
			}
		}
		break;
	}
	case FightGameS2C::RPC_ATTACK3: {
		BYTE bodyLen = sizeof(object->uiID) + sizeof(object->byDir) + sizeof(object->stPos.usX) + sizeof(object->stPos.usY);
		for (uint16_t cy = topCell; cy <= bottomCell; cy++) {
			for (uint16_t cx = leftCell; cx <= rightCell; cx++) {
				for (stObjectInfo* nearPlayer = grid->cells[cy][cx]; nearPlayer != nullptr; nearPlayer = nearPlayer->nextGridObj) {
					if (nearPlayer->uiID != object->uiID) {		// ���� ��󿡴� ������ ��� ����
						g_Proxy.ATTACK3(nearPlayer->uiID, VALID_PACKET_NUM, bodyLen, FightGameS2C::RPC_ATTACK3, object->uiID, object->byDir, object->stPos.usX, object->stPos.usY);
					}
				}
			}
		}
		break;
	}
	}
}
void ForwardCRTMsg(stObjectInfo* object, Grid* grid)
{
	uint16 topCell;
	uint16 bottomCell;
	uint16 leftCell;
	uint16 rightCell;
	GetRangeCell(object->stPos, topCell, bottomCell, leftCell, rightCell);

	BYTE bodyLen = sizeof(object->uiID) + sizeof(object->byDir) + sizeof(object->stPos) + sizeof(object->byHP);

	for (uint16 cy = topCell; cy <= bottomCell; cy++) {
		for (uint16 cx = leftCell; cx <= rightCell; cx++) {
			stObjectInfo* nearobj = grid->cells[cy][cx];
			for (stObjectInfo* nearPlayer = grid->cells[cy][cx]; nearPlayer != nullptr; nearPlayer = nearPlayer->nextGridObj) {
				g_Proxy.CRT_OTHER_CHARACTER(nearPlayer->uiID, VALID_PACKET_NUM, bodyLen, FightGameS2C::RPC_CRT_OTHER_CHARACTER, object->uiID, object->byDir, object->stPos.usX, object->stPos.usY, object->byHP);
				g_Proxy.CRT_OTHER_CHARACTER(object->uiID, VALID_PACKET_NUM, bodyLen, FightGameS2C::RPC_CRT_OTHER_CHARACTER, nearPlayer->uiID, nearPlayer->byDir, nearPlayer->stPos.usX, nearPlayer->stPos.usY, nearPlayer->byHP);
			}
		}
	}

}
void ForwardDmgMsg(stObjectInfo* attacker, stObjectInfo* target, Grid* grid) {
	uint16 topCellAttaker = 0;
	uint16 bottomCellAttaker = 0;
	uint16 leftCellAttaker = 0;
	uint16 rightCellAttaker = 0;

	GetRangeCell(attacker->stPos, topCellAttaker, bottomCellAttaker, leftCellAttaker, rightCellAttaker);

	//uint16 topCellTarget;
	//uint16 bottomCellTarget;
	//uint16 leftCellTarget;
	//uint16 rightCellTarget;

	BYTE bodyLen = sizeof(attacker->uiID) + sizeof(target->uiID) + sizeof(BYTE);

	for (uint16_t cy = topCellAttaker; cy <= bottomCellAttaker; cy++) {
		for (uint16_t cx = leftCellAttaker; cx <= rightCellAttaker; cx++) {
			for (stObjectInfo* nearPlayer = grid->cells[cy][cx]; nearPlayer != nullptr; nearPlayer = nearPlayer->nextGridObj) {
				g_Proxy.DAMAGE(nearPlayer->uiID, VALID_PACKET_NUM, bodyLen, FightGameS2C::RPC_DAMAGE, attacker->uiID, target->uiID, target->byHP);
			}
		}
	}
}

//////////////////////////////
// ��ǥ ���� �� ����ȭ ó��
// MMO���ʹ� ������ �����ӷ��� �������� ���� ����ؾ� �Ѵ�. 
// ���� Ŭ���̾�Ʈ�� ���� ���� �ʰ� �������� ��Ȳ�̶�� ��ǥ�� Ʋ�����ٰ� ������ ���� ���� �ƴ� ��ũ�� ���ߵ��� �Ѵ�.
//////////////////////////////
void SyncPosition(stObjectInfo* player) {
	BYTE bodyLen = sizeof(uint32_t) + sizeof(uint16_t) + sizeof(uint16_t);
	g_Proxy.SYNC(player->uiID, VALID_PACKET_NUM, bodyLen, FightGameS2C::RPC_SYNC, player->uiID, player->stPos.usX, player->stPos.usY);
}

//////////////////////////////
// ĳ���� ����
//////////////////////////////
stPoint GetRandomPosition() {
	stPoint pos;
	srand(time(NULL));
	//pos.usX = (rand() % (dfRANGE_MOVE_RIGHT - dfRANGE_MOVE_LEFT - 1)) + dfRANGE_MOVE_LEFT + 1;
	//pos.usY = (rand() % (dfRANGE_MOVE_BOTTOM - dfRANGE_MOVE_TOP - 1)) + dfRANGE_MOVE_TOP + 1;

	pos.usX = rand() % dfRANGE_MOVE_RIGHT + 1;
	pos.usY = rand() % dfRANGE_MOVE_BOTTOM + 1;

	return pos;
}
void CreateFighter(HostID hostID) {
#ifdef  DEFAULT_POOL
	stObjectInfo* newObject = new stObjectInfo();
#endif //  DEFAULT_POOL
#ifdef JINI_POOL
	stObjectInfo* newObject = reinterpret_cast<stObjectInfo*>(ObjectPool.AllocMem());
#endif // JINI_POOL
	newObject->uiID = hostID;
	newObject->stPos = GetRandomPosition();
	newObject->byHP = dfDEAFAULT_INIT_HP;
	newObject->byDir = rand() % 2 == 0 ? dfPACKET_MOVE_DIR_LL : dfPACKET_MOVE_DIR_RR;
	
	/***** �߿� ���� *****/
	// ��ü Ǯ�� ���ؼ� ��ü �޸𸮸� �Ҵ�ް� �ִ�.
	// ���� ������ �����ڰ� ȣ����� �ʴ´�.
	// ������ �̿��ϴ� ������
	//bool bMoveFlag = false;
	//bool bFirstMoveFlag = false;
	//stObjectInfo* nextGridObj = nullptr;
	//stObjectInfo* prevGridObj = nullptr;
	// ���� ���� �ʱ�ȭ�� �̷������ �ʴ´�.
	// �ᱹ ���� �ʱ�ȭ�ϵ�, placement_new�� ȣ�����־�� �Ѵ�.
	newObject->bMoveFlag = false;
	newObject->bFirstMoveFlag = false;
	newObject->nextGridObj = nullptr;
	newObject->prevGridObj = nullptr;

	//std::cout << "[CREATE] S(" << newObject->stPos.usX << ", " << newObject->stPos.usY << ")" << std::endl;

	// ���� Ÿ�̸ӷ� �ʱ�ȭ
	newObject->lastEchoTime = gTime;
	gTimeMap.insert({ gTime, hostID });

	BYTE bodyLen = sizeof(newObject->uiID) + sizeof(newObject->byDir) + sizeof(newObject->stPos) + sizeof(newObject->byHP);
	g_Proxy.CRT_CHARACTER(newObject->uiID, VALID_PACKET_NUM, bodyLen, FightGameS2C::RPC_CRT_CHARACTER, newObject->uiID, newObject->byDir, newObject->stPos.usX, newObject->stPos.usY, newObject->byHP);

	if (gClientMap.find(hostID) == gClientMap.end()) {
		gClientMap.insert({ hostID, newObject });

#ifdef DUMB_SPACE_DIV
		FowardCRTMsg(newObject);
		InsertToGrid(newObject->stPos.usY, newObject->stPos.usX, hostID);
#endif
#ifdef FIXED_GRID_SPACE_DIV
		ForwardCRTMsg(newObject, &gGrid);
		gGrid.Add(newObject);
#endif
	}
	else {
		ERROR_EXCEPTION_WINDOW(L"CreateFighter", L"gClientMap.find(hostID) != gClientMap.end()");
	}
}
void DeleteFighter(HostID hostID, bool netcoreSide) {
	if (gDeleteClientSet.find(hostID) == gDeleteClientSet.end()) {
		gDeleteClientSet.insert({ hostID, netcoreSide });
	}
}

//////////////////////////////
// ĳ���� �̵� �� ����
//////////////////////////////
void MoveFigter(HostID hostID, BYTE Direction, uint16_t X, uint16_t Y) {
	// ĳ���Ͱ� �ױ� �� move �޽����� ���۵��� Ȯ��(����)
	if (gClientMap.find(hostID) == gClientMap.end()) {
		return;
	}

	stObjectInfo* player = gClientMap[hostID];
	player->byDir = Direction;
	player->bMoveFlag = true;
	player->bFirstMoveFlag = true;

	// ��ǥ�� Ʋ������ Ŭ���̾�Ʈ�� ���� ���� �ƴ϶� Sync�� �����.
	if (abs(X - player->stPos.usX) > dfERROR_RANGE || abs(Y - player->stPos.usY) > dfERROR_RANGE) {
		// ��ǥ ������ dfERROR_RANGE���� ũ�� SYNC �޽��� ����
		//std::cout << "[SYNC_START] C(" << X << ", " << Y << ") S(" << player->stPos.usX << ", " << player->stPos.usY << ")" << std::endl;
		SyncPosition(player);
	}
	else {
		// �ƴ϶�� Ŭ���̾�Ʈ ��ǥ�� �̼��� ������ �����.
		if (player->stPos.usY != Y || player->stPos.usX != X) {
#ifdef DUMB_SPACE_DIV	
			DeleteFromGrid(player->stPos.usY, player->stPos.usX, player->uiID);
#endif
#ifdef FIXED_GRID_SPACE_DIV
			gGrid.Delete(player);
#endif	

			stPoint beforePos = player->stPos;
			player->stPos.usY = Y;
			player->stPos.usX = X;

#ifdef DUMB_SPACE_DIV
			InsertToGrid(player->stPos.usY, player->stPos.usX, player->uiID);
			FowardCrtDelMsgByMove(player, beforePos);
#endif
#ifdef FIXED_GRID_SPACE_DIV
			//GridResetInterestSpace(beforePos, player, &gGrid, false);
			gGrid.Add(player);
#endif 
		}
	}
	

#ifdef DUMB_SPACE_DIV
	// �ֺ� Ŭ���̾�Ʈ�� START �޽��� ������
	ForwardMsgToNear(player, FightGameS2C::RPC_MOVE_START);
#endif
#ifdef FIXED_GRID_SPACE_DIV
	ForwardMsgToNear(player, &gGrid, FightGameS2C::RPC_MOVE_START);
#endif 
}
void StopFigther(HostID hostID, BYTE Direction, uint16_t X, uint16_t Y) {
	// ĳ���Ͱ� �ױ� �� move �޽����� ���۵��� Ȯ��(����)
	if (gClientMap.find(hostID) == gClientMap.end()) {
		return;
	}

	stObjectInfo* player = gClientMap[hostID];
	player->byDir = Direction;
	player->bMoveFlag = false;

	// ��ǥ�� Ʋ������ Ŭ���̾�Ʈ�� ���� ���� �ƴ϶� Sync�� �����.
	if (abs(X - player->stPos.usX) > dfERROR_RANGE || abs(Y - player->stPos.usY) > dfERROR_RANGE) {
		// ��ǥ ������ dfERROR_RANGE���� ũ�� SYNC �޽��� ����
		//std::cout << "[SYNC_STOP] C(" << X << ", " << Y << ") S(" << player->stPos.usX << ", " << player->stPos.usY << ")" << std::endl;
		SyncPosition(player);
	}
	else {
		// �ƴ϶�� Ŭ���̾�Ʈ ��ǥ�� �̼��� ������ �����.
		if (player->stPos.usY != Y || player->stPos.usX != X) {
#ifdef DUMB_SPACE_DIV	
			DeleteFromGrid(player->stPos.usY, player->stPos.usX, player->uiID);
#endif
#ifdef FIXED_GRID_SPACE_DIV
			gGrid.Delete(player);
#endif	

			stPoint beforePos = player->stPos;
			player->stPos.usY = Y;
			player->stPos.usX = X;

#ifdef DUMB_SPACE_DIV
			InsertToGrid(player->stPos.usY, player->stPos.usX, player->uiID);
			FowardCrtDelMsgByMove(player, beforePos);
#endif
#ifdef FIXED_GRID_SPACE_DIV
			//GridResetInterestSpace(beforePos, player, &gGrid, false);
			gGrid.Add(player);
#endif 
		}
	}

#ifdef DUMB_SPACE_DIV
	// �ֺ� Ŭ���̾�Ʈ�� STOP �޽��� ������
	ForwardMsgToNear(player, FightGameS2C::RPC_MOVE_STOP);
#endif
#ifdef FIXED_GRID_SPACE_DIV
	ForwardMsgToNear(player, &gGrid, FightGameS2C::RPC_MOVE_STOP);
#endif
}

//////////////////////////////
// ĳ���� ����
//////////////////////////////
void AttackFighter(HostID hostID, BYTE Direction, uint16_t X, uint16_t Y, enAttackType atkType) {
	// ĳ���Ͱ� �ױ� �� move �޽����� ���۵��� Ȯ��(����)
	if (gClientMap.find(hostID) == gClientMap.end()) {
		return;
	}

	stObjectInfo* player = gClientMap[hostID];
	player->byDir = Direction;

	// ��ǥ�� Ʋ������ Ŭ���̾�Ʈ�� ���� ���� �ƴ϶� Sync�� �����.
	if (abs(X - player->stPos.usX) > dfERROR_RANGE || abs(Y - player->stPos.usY) > dfERROR_RANGE) {
		// ��ǥ ������ dfERROR_RANGE���� ũ�� SYNC �޽��� ����
		SyncPosition(player);
	}
	else {
		// �ƴ϶�� Ŭ���̾�Ʈ ��ǥ�� �̼��� ������ �����.
		if (player->stPos.usY != Y || player->stPos.usX != X) {
#ifdef DUMB_SPACE_DIV	
			DeleteFromGrid(player->stPos.usY, player->stPos.usX, player->uiID);
#endif
#ifdef FIXED_GRID_SPACE_DIV
			gGrid.Delete(player);
#endif	

			stPoint beforePos = player->stPos;
			player->stPos.usY = Y;
			player->stPos.usX = X;

#ifdef DUMB_SPACE_DIV
			InsertToGrid(player->stPos.usY, player->stPos.usX, player->uiID);
			FowardCrtDelMsgByMove(player, beforePos);
#endif
#ifdef FIXED_GRID_SPACE_DIV
			//GridResetInterestSpace(beforePos, player, &gGrid, false);
			gGrid.Add(player);
#endif 
		}
	}

	// �۾� ť ť��
	stAttackWork atkWork;
	atkWork.uiID = player->uiID;
	atkWork.byDir = player->byDir;
	atkWork.usX = player->stPos.usX;
	atkWork.usY = player->stPos.usY;
	atkWork.byType = atkType;
	AtkWorkQueue.push(atkWork);

	// �ֺ� Ŭ���̾�Ʈ�� ATTACK �޽��� ������
	switch (atkType) {
	case enAttackType::ATTACK1: {
#ifdef DUMB_SPACE_DIV
		ForwardMsgToNear(player, FightGameS2C::RPC_ATTACK1);
#endif
#ifdef FIXED_GRID_SPACE_DIV
		ForwardMsgToNear(player, &gGrid, FightGameS2C::RPC_ATTACK1);
#endif
		break;
	}
	case enAttackType::ATTACK2: {
#ifdef DUMB_SPACE_DIV
		ForwardMsgToNear(player, FightGameS2C::RPC_ATTACK2);
#endif
#ifdef FIXED_GRID_SPACE_DIV
		ForwardMsgToNear(player, &gGrid, FightGameS2C::RPC_ATTACK2);
#endif
		break;
	}
	case enAttackType::ATTACK3: {
#ifdef DUMB_SPACE_DIV
		ForwardMsgToNear(player, FightGameS2C::RPC_ATTACK3);
#endif
#ifdef FIXED_GRID_SPACE_DIV
		ForwardMsgToNear(player, &gGrid, FightGameS2C::RPC_ATTACK3);
#endif
		break;
	}
	}
}

void ReceiveEcho(HostID hostID, uint32_t time)
{
	//std::cout << "[Echo] hostID: " << hostID << ", time: " << time << std::endl;
	//time_t t = time;
	//struct tm ltm;
	//localtime_s(&ltm, &t);
	//cout << "year: " << ltm.tm_year + 1900 << endl;
	//cout << "month: " << ltm.tm_mon + 1 << endl;
	//cout << "day: " << ltm.tm_mday << endl;
	//cout << "hour: " << ltm.tm_hour << endl;
	//cout << "min: " << ltm.tm_min << endl;
	//cout << "sec: " << ltm.tm_sec << endl;
	BYTE bodyLen = sizeof(time);
	g_Proxy.ECHO(hostID, VALID_PACKET_NUM, bodyLen, FightGameS2C::RPC_ECHO, time);

	// Ÿ�̸� ����
	if (gClientMap.find(hostID) != gClientMap.end()) {
		stObjectInfo* client = gClientMap[hostID];
		time_t beforeTime = client->lastEchoTime;
		if (beforeTime != gTime) {
			auto rangeIter = gTimeMap.equal_range(beforeTime);
			for (auto iter = rangeIter.first; iter != rangeIter.second; iter++) {
				if (iter->second == hostID) {
					gTimeMap.erase(iter);
					break;
				}
			}

			client->lastEchoTime = gTime;
			gTimeMap.insert({gTime, hostID});
		}

	}
}

//////////////////////////////
// Batch Process
//////////////////////////////
void BatchDeleteClientWork() {
	for (auto delCleint : gDeleteClientSet) {
		HostID hostID = delCleint.first;
		bool netcoreSide = delCleint.second;

		if (gClientMap.find(hostID) != gClientMap.end()) {
			stObjectInfo* client = gClientMap[hostID];

			// �ھ ���� ���� ��û
			if (!netcoreSide) {
				g_Proxy.Disconnect(hostID);
			}

#ifdef DUMB_SPACE_DIV
			ForwardMsgToNear(client, FightGameS2C::RPC_DEL_CHARACTER);
			DeleteFromGrid(client->stPos.usY, client->stPos.usX, client->uiID);
#endif
#ifdef FIXED_GRID_SPACE_DIV
			ForwardMsgToNear(client, &gGrid, FightGameS2C::RPC_DEL_CHARACTER);
			gGrid.Delete(client);
#endif 

#ifdef DEFAULT_POOL
			delete client;
#endif // DEFAULT_POOL
#ifdef JINI_POOL
			ObjectPool.ReturnMem(reinterpret_cast<BYTE*>(client));
#endif // JINI_POOL

			gClientMap.erase(hostID);
		}
		else {
			ERROR_EXCEPTION_WINDOW(L"BatchDeleteClientWork", L"gClientMap.find(hostID) == gClientMap.end()");
		}
	}
	gDeleteClientSet.clear();
}
void AttackWork(HostID atkerID, HostID targetID, enAttackType atkType) {

	BYTE damage = 0;
	switch (atkType) {
	case enAttackType::ATTACK1:
	{ damage = dfATTACK1_DAMAGE; }
	break;
	case enAttackType::ATTACK2:
	{ damage = dfATTACK2_DAMAGE; }
	break;
	case enAttackType::ATTACK3:
	{ damage = dfATTACK3_DAMAGE; }
	break;
	}

	if (gClientMap.find(targetID) != gClientMap.end() && gClientMap.find(atkerID) != gClientMap.end()) {
		stObjectInfo* attacker = gClientMap[atkerID];
		stObjectInfo* target = gClientMap[targetID];
		if (target->byHP <= damage) {
			// ����
			target->byHP = 0;
			DeleteFighter(targetID);
		}
		else {
			target->byHP -= damage;
		}

#ifdef DUMB_SPACE_DIV
		ForwardDmgMsg(attacker, target);
#endif
#ifdef FIXED_GRID_SPACE_DIV
		ForwardDmgMsg(attacker, target, &gGrid);
#endif
	}
	else {
		ERROR_EXCEPTION_WINDOW(L"AttackWork(..)", L"������ �Ǵ� Ÿ�� ��� ����");
	}
}
void BatchAttackWork() {
	while (!AtkWorkQueue.empty()) {
		stAttackWork& atkWork = AtkWorkQueue.front();
		AtkWorkQueue.pop();

		int32 atkRangeY = 0;
		int32 atkRangeX = 0;
		if (atkWork.byType == enAttackType::ATTACK1) {
			atkRangeY = dfATTACK1_RANGE_Y;
			atkRangeX = dfATTACK1_RANGE_X;
		}
		else if (atkWork.byType == enAttackType::ATTACK2) {
			atkRangeY = dfATTACK2_RANGE_Y;
			atkRangeX = dfATTACK2_RANGE_X;
		}
		else if (atkWork.byType == enAttackType::ATTACK3) {
			atkRangeY = dfATTACK3_RANGE_Y;
			atkRangeX = dfATTACK3_RANGE_X;
		}
		else {
			ERROR_EXCEPTION_WINDOW(L"MAIN(���� ó��)", L"UNVALID ATK TYPE");
		}

#ifdef DUMB_SPACE_DIV
		for (int32 y = -atkRangeY / 2; y <= atkRangeY / 2; y++) {
			int32 targetY = static_cast<int32>(atkWork.usY) + y;
			if (targetY >= dfRANGE_MOVE_TOP && targetY <= dfRANGE_MOVE_BOTTOM) {
				for (int32 x = 0; x <= atkRangeX; x++) {
					int32 targetX = static_cast<int32>(atkWork.usX);
					if (atkWork.byDir == dfPACKET_MOVE_DIR_LL) {
						targetX -= x;
					}
					else {
						targetX += x;
					}

					if (targetX >= dfRANGE_MOVE_LEFT && targetX <= dfRANGE_MOVE_RIGHT) {
						stObjectInfo* objPtr = gClientGrid[targetY][targetX];
						while (objPtr != nullptr) {
							if (objPtr->uiID != atkWork.uiID) {
								AttackWork(atkWork.uiID, objPtr->uiID, atkWork.byType);
							}
							objPtr = objPtr->nextGridObj;
						}
					}
				}
			}
		}

#endif
#ifdef FIXED_GRID_SPACE_DIV
		uint16 atkRangeTop = atkWork.usY < atkRangeY / 2 ? dfRANGE_MOVE_TOP : atkWork.usY - atkRangeY / 2;
		uint16 atkRangeBottom = atkWork.usY + atkRangeY / 2 > dfRANGE_MOVE_BOTTOM ? dfRANGE_MOVE_BOTTOM : atkWork.usY + atkRangeY / 2;
		uint16 atkRangeRight;
		uint16 atkRangeLeft;
		if (atkWork.byDir == dfPACKET_MOVE_DIR_LL) {
			atkRangeRight = atkWork.usX;
			atkRangeLeft = atkWork.usX < atkRangeX ? dfRANGE_MOVE_LEFT : atkWork.usX - atkRangeX;
		}
		else {
			atkRangeRight = atkWork.usX + atkRangeX > dfRANGE_MOVE_RIGHT ? dfRANGE_MOVE_RIGHT : atkWork.usX + atkRangeX;
			atkRangeLeft = atkWork.usX;
		}

		for (uint16 cy = atkRangeTop / dfGridCell_Length; cy <= atkRangeBottom / dfGridCell_Length; cy++) {
			for (uint16 cx = atkRangeLeft / dfGridCell_Length; cx <= atkRangeRight / dfGridCell_Length; cx++) {
				for (stObjectInfo* target = gGrid.cells[cy][cx]; target != nullptr; target = target->nextGridObj) {
					if (atkWork.uiID != target->uiID) {
						if (atkRangeTop <= target->stPos.usY && target->stPos.usY <= atkRangeBottom && atkRangeLeft <= target->stPos.usX && target->stPos.usX <= atkRangeRight) {
							AttackWork(atkWork.uiID, target->uiID, atkWork.byType);
						}
					}
				}
			}
		}
#endif
	}
}
void BatchMoveWork(uint16 calibration) {
	for (auto iter : gClientMap) {
		stObjectInfo* object = iter.second;
		if (object->bMoveFlag) {
			if (object->bFirstMoveFlag) {
				object->bFirstMoveFlag = false;
				continue;
			}

#ifdef DUMB_SPACE_DIV	
			DeleteFromGrid(object->stPos.usY, object->stPos.usX, object->uiID);
#endif
#ifdef FIXED_GRID_SPACE_DIV
			gGrid.Delete(object);
#endif	
			stPoint beforePos = object->stPos;

			switch (object->byDir) {
			case dfPACKET_MOVE_DIR_LL:
				if (object->stPos.usX < DELTA_X * (calibration + 1)) {			// ���� ������ ���� �߰�
					object->stPos.usX = dfRANGE_MOVE_LEFT;
				}
				else {
					object->stPos.usX -= DELTA_X * (calibration + 1);
				}
				break;

			case dfPACKET_MOVE_DIR_LU:
				if (object->stPos.usX < DELTA_X * (calibration + 1)) {
					object->stPos.usX = dfRANGE_MOVE_LEFT;
				}
				else {
					object->stPos.usX -= DELTA_X * (calibration + 1);
				}
				if (object->stPos.usY < DELTA_Y * (calibration + 1)) {
					object->stPos.usY = dfRANGE_MOVE_TOP;
				}
				else {
					object->stPos.usY -= DELTA_Y * (calibration + 1);
				}
				break;

			case dfPACKET_MOVE_DIR_UU:
				if (object->stPos.usY < DELTA_Y * (calibration + 1)) {
					object->stPos.usY = dfRANGE_MOVE_TOP;
				}
				else {
					object->stPos.usY -= DELTA_Y * (calibration + 1);
				}
				break;

			case dfPACKET_MOVE_DIR_RU:
				if (object->stPos.usX + DELTA_X * (calibration + 1) > dfRANGE_MOVE_RIGHT) {
					object->stPos.usX = dfRANGE_MOVE_RIGHT;
				}
				else {
					object->stPos.usX += DELTA_X * (calibration + 1);
				}
				if (object->stPos.usY < DELTA_Y * (calibration + 1)) {
					object->stPos.usY = dfRANGE_MOVE_TOP;
				}
				else {
					object->stPos.usY -= DELTA_Y * (calibration + 1);
				}
				break;

			case dfPACKET_MOVE_DIR_RR:
				if (object->stPos.usX + DELTA_X * (calibration + 1) > dfRANGE_MOVE_RIGHT) {
					object->stPos.usX = dfRANGE_MOVE_RIGHT;
				}
				else {
					object->stPos.usX += DELTA_X * (calibration + 1);
				}
				break;

			case dfPACKET_MOVE_DIR_RD:
				if (object->stPos.usX + DELTA_X * (calibration + 1) > dfRANGE_MOVE_RIGHT) {
					object->stPos.usX = dfRANGE_MOVE_RIGHT;
				}
				else {
					object->stPos.usX += DELTA_X * (calibration + 1);
				}
				if (object->stPos.usY + DELTA_Y * (calibration + 1) > dfRANGE_MOVE_BOTTOM) {
					object->stPos.usY = dfRANGE_MOVE_BOTTOM;
				}
				else {
					object->stPos.usY += DELTA_Y * (calibration + 1);
				}
				break;

			case dfPACKET_MOVE_DIR_DD:
				if (object->stPos.usY + DELTA_Y * (calibration + 1) > dfRANGE_MOVE_BOTTOM) {
					object->stPos.usY = dfRANGE_MOVE_BOTTOM;
				}
				else {
					object->stPos.usY += DELTA_Y * (calibration + 1);
				}
				break;

			case dfPACKET_MOVE_DIR_LD:
				if (object->stPos.usX < DELTA_X * (calibration + 1)) {
					object->stPos.usX = dfRANGE_MOVE_LEFT;
				}
				else {
					object->stPos.usX -= DELTA_X * (calibration + 1);
				}
				if (object->stPos.usY + DELTA_Y * (calibration + 1) > dfRANGE_MOVE_BOTTOM) {
					object->stPos.usY = dfRANGE_MOVE_BOTTOM;
				}
				else {
					object->stPos.usY += DELTA_Y * (calibration + 1);
				}
				break;
			default:
				break;
			}

#ifdef DUMB_SPACE_DIV
			InsertToGrid(object->stPos.usY, object->stPos.usX, object->uiID);
			FowardCrtDelMsgByMove(object, beforePos);
#endif
#ifdef FIXED_GRID_SPACE_DIV
			GridResetInterestSpace(beforePos, object, &gGrid, true);
			gGrid.Add(object);
#endif 
		}
	}
}

void BatchTimeOutCheck() {
	for (auto iter = gTimeMap.begin(); iter != gTimeMap.end(); iter++) {
		if (iter->first + 30 >= gTime) {
			// ���� ó��...
			// linger �ɼ� ����
		}
	}
}