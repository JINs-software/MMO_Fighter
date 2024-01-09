#include "Contents.h"

extern FightGameS2C::Proxy g_Proxy;
std::map<HostID, stObjectInfo*> gClientMap;
std::vector<std::vector<stObjectInfo*>> gClientGrid(dfRANGE_MOVE_BOTTOM + 1, std::vector<stObjectInfo*>(dfRANGE_MOVE_RIGHT + 1, nullptr));
std::set<HostID> gDeleteClientSet;
std::queue<stAttackWork> AtkWorkQueue;

//////////////////////////////
// 전송 관리(클라이언트 인접 정보들만 송신)
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
/*
* 클라이언트 좌표 기준,
* Y - Cell_Length * CellCol / 2 <= Y <= Y + Cell_Length * CellCol
* X - Cell_Length * CellRow / 2 <= X <= X + Cell_Length * CellRow
*/
void FowardCRTMsg(stObjectInfo* newObject) {		// 새로 생성될 객체가 서버 관리 자료구조에 등록되기 전 호출된다고 가정
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
void FowardMsgToNearClient(stObjectInfo* player, RpcID msgID) {
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
					if (nearPlayer->uiID != player->uiID) {		// 삭제 대상에는 포워딩 대상 제외
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
					if (nearPlayer->uiID != player->uiID) {	// 이동 대상은 메시지 포워딩 대상 제외						
						std::cout << "[Forward Start] X: " << x << ", Y: " << y << std::endl;
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
					if (nearPlayer->uiID != player->uiID) {	// 이동 대상은 메시지 포워딩 대상 제외
						std::cout << "[Forward Stop] X: " << x << ", Y: " << y << std::endl;
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
					if (nearPlayer->uiID != player->uiID) {	// 공격자는 메시지 포워딩 대상 제외
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
					if (nearPlayer->uiID != player->uiID) {	// 공격자는 메시지 포워딩 대상 제외
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
					if (nearPlayer->uiID != player->uiID) {	// 공격자는 메시지 포워딩 대상 제외
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
		// x 변동 삭제
		for (uint16_t x = delLeft; x < delRight; x++) {
			for (stObjectInfo* nearPlayer = gClientGrid[y][x]; nearPlayer != nullptr; nearPlayer = nearPlayer->nextGridObj) {
				if (nearPlayer->uiID != player->uiID) {		// 삭제 대상에는 포워딩 대상 제외
					g_Proxy.DEL_CHARACTER(nearPlayer->uiID, VALID_PACKET_NUM, sizeof(player->uiID), FightGameS2C::RPC_DEL_CHARACTER, player->uiID);
					g_Proxy.DEL_CHARACTER(player->uiID, VALID_PACKET_NUM, sizeof(player->uiID), FightGameS2C::RPC_DEL_CHARACTER, nearPlayer->uiID);
				}
			}
		}
	}
	for (uint16_t y = top; y <= bottom; y++) {
		// x 변동 생성
		for (uint16_t x = crtLeft; x < crtRight; x++) {
			for (stObjectInfo* nearPlayer = gClientGrid[y][x]; nearPlayer != nullptr; nearPlayer = nearPlayer->nextGridObj) {
				if (nearPlayer->uiID != player->uiID) {		// 삭제 대상에는 포워딩 대상 제외
					g_Proxy.CRT_OTHER_CHARACTER(nearPlayer->uiID, VALID_PACKET_NUM, crtBodyLen, FightGameS2C::RPC_CRT_OTHER_CHARACTER, player->uiID, player->byDir, player->stPos.usX, player->stPos.usY, player->byHP);
					g_Proxy.MOVE_START(nearPlayer->uiID, VALID_PACKET_NUM, moveBodyLen, FightGameS2C::RPC_MOVE_START, player->uiID, player->byDir, player->stPos.usX, player->stPos.usY);
					g_Proxy.CRT_OTHER_CHARACTER(player->uiID, VALID_PACKET_NUM, crtBodyLen, FightGameS2C::RPC_CRT_OTHER_CHARACTER, nearPlayer->uiID, nearPlayer->byDir, nearPlayer->stPos.usX, nearPlayer->stPos.usY, nearPlayer->byHP);
				}
			}
		}
	}

	for (uint16_t x = leftBefore; x <= rightBefore; x++) {
		// y 변동 삭제
		for (uint16_t y = delTop; y < delBottom; y++) {
			for (stObjectInfo* nearPlayer = gClientGrid[y][x]; nearPlayer != nullptr; nearPlayer = nearPlayer->nextGridObj) {
				if (nearPlayer->uiID != player->uiID) {		// 삭제 대상에는 포워딩 대상 제외
					g_Proxy.DEL_CHARACTER(nearPlayer->uiID, VALID_PACKET_NUM, sizeof(player->uiID), FightGameS2C::RPC_DEL_CHARACTER, player->uiID);
					g_Proxy.DEL_CHARACTER(player->uiID, VALID_PACKET_NUM, sizeof(player->uiID), FightGameS2C::RPC_DEL_CHARACTER, nearPlayer->uiID);
				}
			}
		}
	}
	for (uint16_t x = left; x <= right; x++) {
		// y 변동 생성
		for (uint16_t y = crtTop; y < crtBottom; y++) {
			for (stObjectInfo* nearPlayer = gClientGrid[y][x]; nearPlayer != nullptr; nearPlayer = nearPlayer->nextGridObj) {
				if (nearPlayer->uiID != player->uiID) {		// 삭제 대상에는 포워딩 대상 제외
					g_Proxy.CRT_OTHER_CHARACTER(nearPlayer->uiID, VALID_PACKET_NUM, crtBodyLen, FightGameS2C::RPC_CRT_OTHER_CHARACTER, player->uiID, player->byDir, player->stPos.usX, player->stPos.usY, player->byHP);
					g_Proxy.MOVE_START(nearPlayer->uiID, VALID_PACKET_NUM, moveBodyLen, FightGameS2C::RPC_MOVE_START, player->uiID, player->byDir, player->stPos.usX, player->stPos.usY);
					g_Proxy.CRT_OTHER_CHARACTER(player->uiID, VALID_PACKET_NUM, crtBodyLen, FightGameS2C::RPC_CRT_OTHER_CHARACTER, nearPlayer->uiID, nearPlayer->byDir, nearPlayer->stPos.usX, nearPlayer->stPos.usY, nearPlayer->byHP);
				}
			}
		}
	}
}


//////////////////////////////
// 좌표 검증 및 동기화 처리
// MMO부터는 서버의 프레임률이 떨어지는 것을 고려해야 한다. 
// 따라서 클라이언트가 핵을 쓰지 않고 정상적인 상황이라면 좌표가 틀어졌다고 무작정 끊는 것이 아닌 싱크를 맞추도록 한다.
//////////////////////////////
void SyncPosition(stObjectInfo* player) {
	BYTE bodyLen = sizeof(uint32_t) + sizeof(uint16_t) + sizeof(uint16_t);
	g_Proxy.SYNC(player->uiID, VALID_PACKET_NUM, bodyLen, FightGameS2C::RPC_SYNC, player->uiID, player->stPos.usX, player->stPos.usY);
}

//////////////////////////////
// 그리드 관리
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


//////////////////////////////
// 캐릭터 생성
//////////////////////////////
stPoint GetRandomPosition() {
	stPoint pos;
	srand(time(NULL));
	pos.usX = (rand() % (dfRANGE_MOVE_RIGHT - dfRANGE_MOVE_LEFT - 1)) + dfRANGE_MOVE_LEFT + 1;
	pos.usY = (rand() % (dfRANGE_MOVE_BOTTOM - dfRANGE_MOVE_TOP - 1)) + dfRANGE_MOVE_TOP + 1;

	pos.usX %= 300;
	pos.usY %= 300;



	return pos;
}
void CreateFighter(HostID hostID) {
	stObjectInfo* newObject = new stObjectInfo();
	newObject->uiID = hostID;
	newObject->stPos = GetRandomPosition();
	newObject->byHP = dfDEAFAULT_INIT_HP;
	newObject->byDir = rand() % 2 == 0 ? dfPACKET_MOVE_DIR_LL : dfPACKET_MOVE_DIR_RR;

	BYTE bodyLen = sizeof(newObject->uiID) + sizeof(newObject->byDir) + sizeof(newObject->stPos) + sizeof(newObject->byHP);
	g_Proxy.CRT_CHARACTER(newObject->uiID, VALID_PACKET_NUM, bodyLen, FightGameS2C::RPC_CRT_CHARACTER, newObject->uiID, newObject->byDir, newObject->stPos.usX, newObject->stPos.usY, newObject->byHP);

	FowardCRTMsg(newObject);

	if (gClientMap.find(hostID) == gClientMap.end()) {
		gClientMap.insert({ hostID, newObject });

		InsertToGrid(newObject->stPos.usY, newObject->stPos.usX, hostID);
	}
	else {
		ERROR_EXCEPTION_WINDOW(L"CreateFighter", L"gClientMap.find(hostID) != gClientMap.end()");
	}
}
void DeleteFighter(HostID hostID) {
	if (gDeleteClientSet.find(hostID) == gDeleteClientSet.end()) {
		gDeleteClientSet.insert(hostID);
	}
}

//////////////////////////////
// 캐릭터 이동 및 중지
//////////////////////////////
void MoveFigter(HostID hostID, BYTE Direction, uint16_t X, uint16_t Y) {
	stObjectInfo* player = gClientMap[hostID];
	player->byDir = Direction;
	player->bMoveFlag = true;
	player->bFirstMoveFlag = true;

	// SYNC 메시지가 너무 빈번하다. 
	// 패킷 분석 결과 클라이언트에서는 이동 방향에 대해 1프레임 이동한 좌표를 보내준다.
	stPoint beforePos = player->stPos;
	DeleteFromGrid(player->stPos.usY, player->stPos.usX, player->uiID);
	switch (player->byDir) {
	case dfPACKET_MOVE_DIR_LL:
		player->stPos.usX -= DELTA_X;
		break;

	case dfPACKET_MOVE_DIR_LU:
		player->stPos.usX -= DELTA_X;
		break;

	case dfPACKET_MOVE_DIR_UU:
		player->stPos.usY -= DELTA_Y;
		break;

	case dfPACKET_MOVE_DIR_RU:
		player->stPos.usX += DELTA_X;
		player->stPos.usY -= DELTA_Y;
		break;

	case dfPACKET_MOVE_DIR_RR:
		player->stPos.usX += DELTA_X;
		break;

	case dfPACKET_MOVE_DIR_RD:
		player->stPos.usX += DELTA_X;
		player->stPos.usY += DELTA_Y;
		break;

	case dfPACKET_MOVE_DIR_DD:
		player->stPos.usY += DELTA_Y;
		break;

	case dfPACKET_MOVE_DIR_LD:
		player->stPos.usX -= DELTA_X;
		break;
	default:
		break;
	}
	if (player->stPos.usX < dfRANGE_MOVE_LEFT) {
		player->stPos.usX = dfRANGE_MOVE_LEFT;
	}
	if (player->stPos.usX > dfRANGE_MOVE_RIGHT) {
		player->stPos.usX = dfRANGE_MOVE_RIGHT;
	}
	if (player->stPos.usY < dfRANGE_MOVE_TOP) {
		player->stPos.usY = dfRANGE_MOVE_TOP;
	}
	if (player->stPos.usY > dfRANGE_MOVE_BOTTOM) {
		player->stPos.usY = dfRANGE_MOVE_BOTTOM;
	}
	InsertToGrid(player->stPos.usY, player->stPos.usX, player->uiID);
	FowardCrtDelMsgByMove(player, beforePos);

	// 오로지 서버의 데이터를 신뢰
	// 단 좌표가 틀어지면 클라이언트를 끊는 것이 아니라 Sync를 맞춘다.
	if (X != player->stPos.usX || Y != player->stPos.usY) {
		std::cout << "[SYNC / MOVE] X: " << X << ", pX: " << player->stPos.usX << " / Y: " << Y << ", pY: " << player->stPos.usY << std::endl;
		SyncPosition(player);
	}
	// 주변 클라이언트에 START 메시지 포워딩
	FowardMsgToNearClient(player, FightGameS2C::RPC_MOVE_START);
	

}
void StopFigther(HostID hostID, BYTE Direction, uint16_t X, uint16_t Y) {
	// 오로지 서버의 데이터를 신뢰
	// 단 좌표가 틀어지면 클라이언트를 끊는 것이 아니라 Sync를 맞춘다.
	stObjectInfo* player = gClientMap[hostID];
	if (X != player->stPos.usX || Y != player->stPos.usY) {
		std::cout << "[SYNC / STOP] X: " << X << ", pX: " << player->stPos.usX << " / Y: " << Y << ", pY: " << player->stPos.usY << std::endl;
		SyncPosition(player);
	}

	player->byDir = Direction;
	player->bMoveFlag = false;

	// 주변 클라이언트에 STOP 메시지 포워딩
	FowardMsgToNearClient(player, FightGameS2C::RPC_MOVE_STOP);
}

//////////////////////////////
// 캐릭터 공격
//////////////////////////////
void AttackFighter(HostID hostID, BYTE Direction, uint16_t X, uint16_t Y, enAttackType atkType) {
	stObjectInfo* player = gClientMap[hostID];
	if (X != player->stPos.usX || Y != player->stPos.usY) {
		std::cout << "[SYNC / ATTACK] X: " << X << ", pX: " << player->stPos.usX << " / Y: " << Y << ", pY: " << player->stPos.usY << std::endl;
		SyncPosition(player);
	}

	player->byDir = Direction;

	// 작업 큐 큐잉
	stAttackWork atkWork;
	atkWork.uiID = player->uiID;
	atkWork.byDir = player->byDir;
	atkWork.usX = player->stPos.usX;
	atkWork.usY = player->stPos.usY;
	atkWork.byType = atkType;
	AtkWorkQueue.push(atkWork);

	// 주변 클라이언트에 ATTACK 메시지 포워딩
	switch (atkType) {
	case enAttackType::ATTACK1: {
		FowardMsgToNearClient(player, FightGameS2C::RPC_ATTACK1);
		break;
	}
	case enAttackType::ATTACK2: {
		FowardMsgToNearClient(player, FightGameS2C::RPC_ATTACK2);
		break;
	}
	case enAttackType::ATTACK3: {
		FowardMsgToNearClient(player, FightGameS2C::RPC_ATTACK3);
		break;
	}
	}
}

void ReceiveEcho(HostID hostID, uint32_t time)
{
	std::cout << "[Echo] hostID: " << hostID << ", time: " << time << std::endl;
	BYTE bodyLen = sizeof(time);
	g_Proxy.ECHO(hostID, VALID_PACKET_NUM, bodyLen, FightGameS2C::RPC_ECHO, time);
}

//////////////////////////////
// Batch Process
//////////////////////////////
void BatchDeleteClientWork() {
	for (HostID hostID : gDeleteClientSet) {
		if (gClientMap.find(hostID) != gClientMap.end()) {
			stObjectInfo* client = gClientMap[hostID];

			FowardMsgToNearClient(client, FightGameS2C::RPC_DEL_CHARACTER);

			// 코어에 연결 종료 요청
			g_Proxy.Disconnect(hostID);

			DeleteFromGrid(client->stPos.usY, client->stPos.usX, client->uiID);
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
			// 죽음
			target->byHP = 0;
			DeleteFighter(targetID);
		}
		else {
			target->byHP -= damage;
		}

		ForwardDmgMsg(attacker, target);
	}
	else {
		ERROR_EXCEPTION_WINDOW(L"AttackWork(..)", L"공격자 또는 타겟 대상 없음");
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
			ERROR_EXCEPTION_WINDOW(L"MAIN(공격 처리)", L"UNVALID ATK TYPE");
		}


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
	}
}
void BatchMoveWork() {
	for (auto iter : gClientMap) {
		stObjectInfo* object = iter.second;
		if (object->bMoveFlag) {
			if (object->bFirstMoveFlag) {
				object->bFirstMoveFlag = false;
				continue;
			}
			stPoint beforePos = object->stPos;
			// 기존 위치 삭제
			DeleteFromGrid(object->stPos.usY, object->stPos.usX, object->uiID);

			switch (object->byDir) {
			case dfPACKET_MOVE_DIR_LL:
				object->stPos.usX -= DELTA_X;
				break;

			case dfPACKET_MOVE_DIR_LU:
				object->stPos.usX -= DELTA_X;
				break;

			case dfPACKET_MOVE_DIR_UU:
				object->stPos.usY -= DELTA_Y;
				break;

			case dfPACKET_MOVE_DIR_RU:
				object->stPos.usX += DELTA_X;
				object->stPos.usY -= DELTA_Y;
				break;

			case dfPACKET_MOVE_DIR_RR:
				object->stPos.usX += DELTA_X;
				break;

			case dfPACKET_MOVE_DIR_RD:
				object->stPos.usX += DELTA_X;
				object->stPos.usY += DELTA_Y;
				break;

			case dfPACKET_MOVE_DIR_DD:
				object->stPos.usY += DELTA_Y;
				break;

			case dfPACKET_MOVE_DIR_LD:
				object->stPos.usX -= DELTA_X;
				break;
			default:
				break;
			}
			if (object->stPos.usX < dfRANGE_MOVE_LEFT) {
				object->stPos.usX = dfRANGE_MOVE_LEFT;
			}
			if (object->stPos.usX > dfRANGE_MOVE_RIGHT) {
				object->stPos.usX = dfRANGE_MOVE_RIGHT;
			}
			if (object->stPos.usY < dfRANGE_MOVE_TOP) {
				object->stPos.usY = dfRANGE_MOVE_TOP;
			}
			if (object->stPos.usY > dfRANGE_MOVE_BOTTOM) {
				object->stPos.usY = dfRANGE_MOVE_BOTTOM;
			}

			InsertToGrid(object->stPos.usY, object->stPos.usX, object->uiID);
			FowardCrtDelMsgByMove(object, beforePos);
		}
	}
}