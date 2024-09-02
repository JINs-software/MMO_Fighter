#include "Contents.h"
#include <ctime>

#include "RPC/Proxy_FightGameCrtDel.h"
#include "RPC/Proxy_FightGameMove.h"
#include "RPC/Proxy_FightGameAttack.h"
#include "RPC/Proxy_FightGameDamage.h"
#include "RPC/Proxy_FightGameComm.h"

using namespace std;

extern FightGameCrtDel_S2C::Proxy g_ProxyCrtDel;
extern FightGameMove_S2C::Proxy g_ProxyMove;
extern FightGameAttack_S2C::Proxy g_ProxyAttack;
extern FightGameDamage_S2C::Proxy g_ProxyDamage;
extern FightGameComm_S2C::Proxy g_ProxyComm;

std::map<HostID, stObjectInfo*> g_ClientMap;
std::vector<std::vector<stObjectInfo*>> g_ClientGrid(dfRANGE_MOVE_BOTTOM + 1, std::vector<stObjectInfo*>(dfRANGE_MOVE_RIGHT + 1, nullptr));
std::map<HostID, bool> g_DeleteClientSet;
std::queue<stAttackWork> g_AtkWorkQueue;

Grid g_Grid(dfRANGE_MOVE_BOTTOM, dfRANGE_MOVE_RIGHT);

// 전역의 타이머
time_t g_Time;
//std::multimap<time_t, HostID> g_TimeMap;
std::set<pair<time_t, HostID>/*, std::greater<pair<time_t, HostID>>*/> g_TimeSet;

// 메모리 풀
JiniPool g_ObjectPool(sizeof(stObjectInfo), MAXIMUM_FIGHTER);

// 시간 측정
uint16 g_SyncPerLoop;
clock_t g_Duration_TimeOutCheck;
clock_t g_Duration_AttackWork;
clock_t g_Duration_DeleteClient;
clock_t g_Duration_MoveWork;
uint16 g_DeleteClientCnt;
uint16 g_DisconnectedClient;

//===================================================================================================================

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

//////////////////////////////
// 그리드 관리
//////////////////////////////
void InsertToGrid(uint16_t Y, uint16_t X, HostID id) {
	if (g_ClientGrid[Y][X] == nullptr) {
		g_ClientGrid[Y][X] = g_ClientMap[id];
	}
	else {
		stObjectInfo* beforePtr = g_ClientGrid[Y][X];
		stObjectInfo* objPtr = g_ClientGrid[Y][X]->nextGridObj;
		while (true) {
			if (objPtr == nullptr) {
				beforePtr->nextGridObj = g_ClientMap[id];
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
	if (g_ClientGrid[Y][X] == nullptr) {
		return;
	}

	stObjectInfo* objPtr = g_ClientGrid[Y][X];
	if (objPtr->uiID == id) {
		g_ClientGrid[Y][X] = objPtr->nextGridObj;
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
* 클라이언트 좌표 기준,
* Y - Cell_Length * CellCol / 2 <= Y <= Y + Cell_Length * CellCol
* X - Cell_Length * CellRow / 2 <= X <= X + Cell_Length * CellRow
*/
void FowardCRTMsg(stObjectInfo* newObject) {		// 새로 생성될 객체가 서버 관리 자료구조에 등록되기 전 호출된다고 가정
	bool proxyRet;
	uint16_t top = newObject->stPos.usY < dfGridCell_Length * dfGridCell_Row / 2 ? dfRANGE_MOVE_TOP : newObject->stPos.usY - dfGridCell_Length * dfGridCell_Row / 2;
	uint16_t bottom = newObject->stPos.usY + dfGridCell_Length * dfGridCell_Row / 2 > dfRANGE_MOVE_BOTTOM ? dfRANGE_MOVE_BOTTOM : newObject->stPos.usY + dfGridCell_Length * dfGridCell_Row / 2;
	uint16_t left = newObject->stPos.usX < dfGridCell_Length * dfGridCell_Col / 2 ? dfRANGE_MOVE_LEFT : newObject->stPos.usX - dfGridCell_Length * dfGridCell_Col / 2;
	uint16_t right = newObject->stPos.usX + dfGridCell_Length * dfGridCell_Col / 2 > dfRANGE_MOVE_RIGHT ? dfRANGE_MOVE_RIGHT : newObject->stPos.usX + dfGridCell_Length * dfGridCell_Col / 2;
	//BYTE bodyLen = sizeof(newObject->uiID) + sizeof(newObject->byDir) + sizeof(newObject->stPos) + sizeof(newObject->byHP);
	for (uint16_t y = top; y <= bottom; y++) {
		for (uint16_t x = left; x <= right; x++) {
			for(stObjectInfo* nearPlayer = g_ClientGrid[y][x]; nearPlayer != nullptr; nearPlayer = nearPlayer->nextGridObj) {
				proxyRet = g_ProxyCrtDel.CRT_OTHER_CHARACTER(nearPlayer->uiID, newObject->uiID, newObject->byDir, newObject->stPos.usX, newObject->stPos.usY, newObject->byHP);
				assert(proxyRet);
				proxyRet = g_ProxyCrtDel.CRT_OTHER_CHARACTER(newObject->uiID, nearPlayer->uiID, nearPlayer->byDir, nearPlayer->stPos.usX, nearPlayer->stPos.usY, nearPlayer->byHP);
				assert(proxyRet);
			}
		}
	}
}
void ForwardDmgMsg(stObjectInfo* attacker, stObjectInfo* target) {
	bool proxyRet;
	uint16_t minY = attacker->stPos.usY < target->stPos.usY ? attacker->stPos.usY : target->stPos.usY;
	uint16_t maxY = attacker->stPos.usY > target->stPos.usY ? attacker->stPos.usY : target->stPos.usY;
	uint16_t minX = attacker->stPos.usX < target->stPos.usX ? attacker->stPos.usX : target->stPos.usX;
	uint16_t maxX = attacker->stPos.usX > target->stPos.usX ? attacker->stPos.usX : target->stPos.usX;

	uint16_t top = minY < dfGridCell_Length * dfGridCell_Row / 2 ? dfRANGE_MOVE_TOP : minY - dfGridCell_Length * dfGridCell_Row / 2;
	uint16_t bottom = maxY + dfGridCell_Length * dfGridCell_Row / 2 > dfRANGE_MOVE_BOTTOM ? dfRANGE_MOVE_BOTTOM : maxY + dfGridCell_Length * dfGridCell_Row / 2;
	uint16_t left = minX < dfGridCell_Length * dfGridCell_Col / 2 ? dfRANGE_MOVE_LEFT : minX - dfGridCell_Length * dfGridCell_Col / 2;
	uint16_t right = maxX + dfGridCell_Length * dfGridCell_Col / 2 > dfRANGE_MOVE_RIGHT ? dfRANGE_MOVE_RIGHT : maxX + dfGridCell_Length * dfGridCell_Col / 2;

	//BYTE bodyLen = sizeof(attacker->uiID) + sizeof(target->uiID) + sizeof(BYTE);
	for (uint16_t y = top; y <= bottom; y++) {
		for (uint16_t x = left; x <= right; x++) {
			for (stObjectInfo* nearPlayer = g_ClientGrid[y][x]; nearPlayer != nullptr; nearPlayer = nearPlayer->nextGridObj) {
				proxyRet = g_ProxyDamage.DAMAGE(nearPlayer->uiID, attacker->uiID, target->uiID, target->byHP);
				assert(proxyRet);
			}
		}
	}
}
void ForwardMsgToNear(stObjectInfo* player, RpcID msgID) {
	bool proxyRet;
	uint16_t top = player->stPos.usY < dfGridCell_Length * dfGridCell_Row / 2 ? dfRANGE_MOVE_TOP : player->stPos.usY - dfGridCell_Length * dfGridCell_Row / 2;
	uint16_t bottom = player->stPos.usY + dfGridCell_Length * dfGridCell_Row / 2 > dfRANGE_MOVE_BOTTOM ? dfRANGE_MOVE_BOTTOM : player->stPos.usY + dfGridCell_Length * dfGridCell_Row / 2;
	uint16_t left = player->stPos.usX < dfGridCell_Length * dfGridCell_Col / 2 ? dfRANGE_MOVE_LEFT : player->stPos.usX - dfGridCell_Length * dfGridCell_Col / 2;
	uint16_t right = player->stPos.usX + dfGridCell_Length * dfGridCell_Col / 2 > dfRANGE_MOVE_RIGHT ? dfRANGE_MOVE_RIGHT : player->stPos.usX + dfGridCell_Length * dfGridCell_Col / 2;

	switch (msgID) {
	case FightGameCrtDel_S2C::RPC_DEL_CHARACTER: {
		//BYTE bodyLen = sizeof(player->uiID);
		for (uint16_t y = top; y <= bottom; y++) {
			for (uint16_t x = left; x <= right; x++) {
				for (stObjectInfo* nearPlayer = g_ClientGrid[y][x]; nearPlayer != nullptr; nearPlayer = nearPlayer->nextGridObj) {
					if (nearPlayer->uiID != player->uiID) {		// 삭제 대상에는 포워딩 대상 제외
						proxyRet = g_ProxyCrtDel.DEL_CHARACTER(nearPlayer->uiID, player->uiID);
						assert(proxyRet);
					}
				}
			}
		}
		break;
	}
	case FightGameMove_S2C::RPC_MOVE_START: {
		//BYTE bodyLen = sizeof(player->uiID) + sizeof(player->byDir) + sizeof(player->stPos.usX) + sizeof(player->stPos.usY);
		for (uint16_t y = top; y <= bottom; y++) {
			for (uint16_t x = left; x <= right; x++) {
				for (stObjectInfo* nearPlayer = g_ClientGrid[y][x]; nearPlayer != nullptr; nearPlayer = nearPlayer->nextGridObj) {
					if (nearPlayer->uiID != player->uiID) {	// 이동 대상은 메시지 포워딩 대상 제외						
						//std::cout << "[Forward Start] X: " << x << ", Y: " << y << std::endl;
						proxyRet = g_ProxyMove.MOVE_START(nearPlayer->uiID, player->uiID, player->byDir, player->stPos.usX, player->stPos.usY);
						assert(proxyRet);
					}
				}
			}
		}
		break;
	}
	case FightGameMove_S2C::RPC_MOVE_STOP: {
		//BYTE bodyLen = sizeof(player->uiID) + sizeof(player->byDir) + sizeof(player->stPos.usX) + sizeof(player->stPos.usY);
		for (uint16_t y = top; y <= bottom; y++) {
			for (uint16_t x = left; x <= right; x++) {
				for (stObjectInfo* nearPlayer = g_ClientGrid[y][x]; nearPlayer != nullptr; nearPlayer = nearPlayer->nextGridObj) {
					if (nearPlayer->uiID != player->uiID) {	// 이동 대상은 메시지 포워딩 대상 제외
						//std::cout << "[Forward Stop] X: " << x << ", Y: " << y << std::endl;
						proxyRet = g_ProxyMove.MOVE_STOP(nearPlayer->uiID, player->uiID, player->byDir, player->stPos.usX, player->stPos.usY);
						assert(proxyRet);
					}
				}
			}
		}
		break;
	}
	case FightGameAttack_S2C::RPC_ATTACK1: {
		//BYTE bodyLen = sizeof(player->uiID) + sizeof(player->byDir) + sizeof(player->stPos.usX) + sizeof(player->stPos.usY);
		for (uint16_t y = top; y <= bottom; y++) {
			for (uint16_t x = left; x <= right; x++) {
				for (stObjectInfo* nearPlayer = g_ClientGrid[y][x]; nearPlayer != nullptr; nearPlayer = nearPlayer->nextGridObj) {
					if (nearPlayer->uiID != player->uiID) {	// 공격자는 메시지 포워딩 대상 제외
						proxyRet = g_ProxyAttack.ATTACK1(nearPlayer->uiID, player->uiID, player->byDir, player->stPos.usX, player->stPos.usY);
						assert(proxyRet);
					}
				}
			}
		}
		break;
	}
	case FightGameAttack_S2C::RPC_ATTACK2: {
		//BYTE bodyLen = sizeof(player->uiID) + sizeof(player->byDir) + sizeof(player->stPos.usX) + sizeof(player->stPos.usY);
		for (uint16_t y = top; y <= bottom; y++) {
			for (uint16_t x = left; x <= right; x++) {
				for (stObjectInfo* nearPlayer = g_ClientGrid[y][x]; nearPlayer != nullptr; nearPlayer = nearPlayer->nextGridObj) {
					if (nearPlayer->uiID != player->uiID) {	// 공격자는 메시지 포워딩 대상 제외
						proxyRet = g_ProxyAttack.ATTACK2(nearPlayer->uiID, player->uiID, player->byDir, player->stPos.usX, player->stPos.usY);
						assert(proxyRet);
					}
				}
			}
		}
		break;
	}
	case FightGameAttack_S2C::RPC_ATTACK3: {
		//BYTE bodyLen = sizeof(player->uiID) + sizeof(player->byDir) + sizeof(player->stPos.usX) + sizeof(player->stPos.usY);
		for (uint16_t y = top; y <= bottom; y++) {
			for (uint16_t x = left; x <= right; x++) {
				for (stObjectInfo* nearPlayer = g_ClientGrid[y][x]; nearPlayer != nullptr; nearPlayer = nearPlayer->nextGridObj) {
					if (nearPlayer->uiID != player->uiID) {	// 공격자는 메시지 포워딩 대상 제외
						proxyRet = g_ProxyAttack.ATTACK3(nearPlayer->uiID, player->uiID, player->byDir, player->stPos.usX, player->stPos.usY);
						assert(proxyRet);
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

	bool proxyRet;
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
			for (stObjectInfo* nearPlayer = g_ClientGrid[y][x]; nearPlayer != nullptr; nearPlayer = nearPlayer->nextGridObj) {
				if (nearPlayer->uiID != player->uiID) {		// 삭제 대상에는 포워딩 대상 제외
					proxyRet = g_ProxyCrtDel.DEL_CHARACTER(nearPlayer->uiID, player->uiID);
					assert(proxyRet);
					proxyRet = g_ProxyCrtDel.DEL_CHARACTER(player->uiID, nearPlayer->uiID);
					assert(proxyRet);
				}
			}
		}
	}
	for (uint16_t y = top; y <= bottom; y++) {
		// x 변동 생성
		for (uint16_t x = crtLeft; x < crtRight; x++) {

			for (stObjectInfo* nearPlayer = g_ClientGrid[y][x]; nearPlayer != nullptr; nearPlayer = nearPlayer->nextGridObj) {
				if (nearPlayer->uiID != player->uiID) {		// 삭제 대상에는 포워딩 대상 제외
					proxyRet = g_ProxyCrtDel.CRT_OTHER_CHARACTER(nearPlayer->uiID, player->uiID, player->byDir, player->stPos.usX, player->stPos.usY, player->byHP);
					assert(proxyRet);
					proxyRet = g_ProxyMove.MOVE_START(nearPlayer->uiID, player->uiID, player->byDir, player->stPos.usX, player->stPos.usY);
					assert(proxyRet);
					proxyRet = g_ProxyCrtDel.CRT_OTHER_CHARACTER(player->uiID, nearPlayer->uiID, nearPlayer->byDir, nearPlayer->stPos.usX, nearPlayer->stPos.usY, nearPlayer->byHP);
					assert(proxyRet);
				}
			}
		}
	}

	for (uint16_t x = leftBefore; x <= rightBefore; x++) {
		// y 변동 삭제
		for (uint16_t y = delTop; y < delBottom; y++) {
			for (stObjectInfo* nearPlayer = g_ClientGrid[y][x]; nearPlayer != nullptr; nearPlayer = nearPlayer->nextGridObj) {
				if (nearPlayer->uiID != player->uiID) {		// 삭제 대상에는 포워딩 대상 제외
					proxyRet = g_ProxyCrtDel.DEL_CHARACTER(nearPlayer->uiID, player->uiID);
					assert(proxyRet);
					proxyRet = g_ProxyCrtDel.DEL_CHARACTER(player->uiID, nearPlayer->uiID);
					assert(proxyRet);
				}
			}
		}
	}
	for (uint16_t x = left; x <= right; x++) {
		// y 변동 생성
		for (uint16_t y = crtTop; y < crtBottom; y++) {
			for (stObjectInfo* nearPlayer = g_ClientGrid[y][x]; nearPlayer != nullptr; nearPlayer = nearPlayer->nextGridObj) {
				if (nearPlayer->uiID != player->uiID) {		// 삭제 대상에는 포워딩 대상 제외
					proxyRet = g_ProxyCrtDel.CRT_OTHER_CHARACTER(nearPlayer->uiID, player->uiID, player->byDir, player->stPos.usX, player->stPos.usY, player->byHP);
					assert(proxyRet);
					proxyRet = g_ProxyMove.MOVE_START(nearPlayer->uiID, player->uiID, player->byDir, player->stPos.usX, player->stPos.usY);
					assert(proxyRet);
					proxyRet = g_ProxyCrtDel.CRT_OTHER_CHARACTER(player->uiID, nearPlayer->uiID, nearPlayer->byDir, nearPlayer->stPos.usX, nearPlayer->stPos.usY, nearPlayer->byHP);
					assert(proxyRet);
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
	else if (cellY <= dfGridCell_Row) {
		// 테두리에 인접한 캐릭터는 바라보지만, 
		// 테두리에서 조금 벗어난 캐릭터는 테두리에 인접한 캐릭터를 바라보지 못하는 상황을 막기위해
		// 테두리에서 조금 벗어난 캐릭터의 관심 범위를 넓힌다.
		topCell = dfRANGE_GRID_TOP;
		bottomCell = cellY + dfGridCell_Row / 2;
	}
	else if (dfRANGE_GRID_BOTTOM - dfGridCell_Row / 2 < cellY) {
		topCell = dfRANGE_GRID_BOTTOM - dfGridCell_Row;
		bottomCell = dfRANGE_GRID_BOTTOM;
	}
	else if (dfRANGE_GRID_BOTTOM - dfGridCell_Row <= cellY) {
		// ""
		topCell = cellY - dfGridCell_Row / 2;
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
	else if (cellX <= dfGridCell_Col) {
		// ""
		leftCell = dfRANGE_GRID_LEFT;
		rightCell = cellX + dfGridCell_Col / 2;
	}
	else if (dfRANGE_GRID_RIGHT - dfGridCell_Col / 2 < cellX) {
		leftCell = dfRANGE_GRID_RIGHT - dfGridCell_Col;
		rightCell = dfRANGE_GRID_RIGHT;
	}
	else if (dfRANGE_GRID_RIGHT - dfGridCell_Col <= cellX) {
		// ""
		leftCell = cellX - dfGridCell_Col / 2;
		rightCell = dfRANGE_GRID_RIGHT;
	}
	else {
		leftCell = cellX - dfGridCell_Col / 2;
		rightCell = cellX + dfGridCell_Col / 2;
	}
}
void GridResetInterestSpace(stPoint beforePos, stObjectInfo* object, Grid* grid, bool crtFlag) {
	bool proxyRet;

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

	//std::pair<bool, uint16> delRow = { false, 0 };
	//std::pair<bool, uint16> delCol = { false, 0 };
	//std::pair<bool, uint16> crtRow = { false, 0 };
	//std::pair<bool, uint16> crtCol = { false, 0 };
	std::pair<bool, pair<uint16, uint16>> delRow = { false, {0, 0} };
	std::pair<bool, pair<uint16, uint16>> delCol = { false, {0, 0} };
	std::pair<bool, pair<uint16, uint16>> crtRow = { false, {0, 0} };
	std::pair<bool, pair<uint16, uint16>> crtCol = { false, {0, 0} };
	if (leftCellBefore < leftCell) {
		// 기존: |----------?
		// 이동:  |=========?
		delCol.first = true;
		//delCol.second = leftCellBefore;
		delCol.second.first = leftCellBefore;
		delCol.second.second = leftCell - 1;
	}
	else if (leftCellBefore > leftCell) {
		// 기존:  |---------?
		// 이동: |==========?
		crtCol.first = true;
		//crtCol.second = leftCell;
		crtCol.second.first = leftCell;
		crtCol.second.second = leftCellBefore - 1;
	}
	if (rightCellBefore < rightCell) {
		// 기존: ?---------|
		// 이동: ?==========|
		crtCol.first = true;
		//crtCol.second = rightCell;
		crtCol.second.first = rightCellBefore + 1;
		crtCol.second.second = rightCell;
	}
	else if (rightCellBefore > rightCell) {
		// 기존: ?----------|
		// 이동: ?=========|
		delCol.first = true;
		//delCol.second = rightCellBefore;
		delCol.second.first = rightCell + 1;
		delCol.second.second = rightCellBefore;
	}

	if (topCellBefore < topCell) {
		// |
		// | |
		// | |
		// ? ?
		delRow.first = true;
		//delRow.second = topCellBefore;
		delRow.second.first = topCellBefore;
		delRow.second.second = topCell - 1;
	}
	else if (topCellBefore > topCell) {
		//   |
		// | |
		// | |
		// ? ?
		crtRow.first = true;
		//crtRow.second = topCell;
		crtRow.second.first = topCell;
		crtRow.second.second = topCellBefore - 1;
	}
	if (bottomCellBefore < bottomCell) {
		// ? ?
		// | |
		// | |
		//   |
		crtRow.first = true;
		//crtRow.second = bottomCell;
		crtRow.second.first = bottomCellBefore + 1;
		crtRow.second.second = bottomCell;
	}
	else if (bottomCellBefore > bottomCell) {
		// ? ?
		// | |
		// | |
		// | 
		delRow.first = true;
		//delRow.second = bottomCellBefore;
		delRow.second.first = bottomCell + 1;
		delRow.second.second = bottomCellBefore;
	}

	if (delRow.first && delCol.first) {	// ㄴ ㄱ . . 삭제
		// (1) 꼭지점 삭제
		//for (stObjectInfo* nearPlayer = grid->cells[delRow.second][delCol.second]; nearPlayer != nullptr; nearPlayer = nearPlayer->nextGridObj) {
		//	if (nearPlayer->uiID != object->uiID) {		// 삭제 대상에는 포워딩 대상 제외
		//		g_ProxyCrtDel.DEL_CHARACTER(nearPlayer->uiID, object->uiID);
		//		g_ProxyCrtDel.DEL_CHARACTER(object->uiID, nearPlayer->uiID);
		//	}
		//}
		for (uint16 cx = delCol.second.first; cx <= delCol.second.second; cx++) {
			for (uint16 cy = delRow.second.first; cy <= delRow.second.second; cy++) {
				for (stObjectInfo* nearPlayer = grid->cells[cy][cx]; nearPlayer != nullptr; nearPlayer = nearPlayer->nextGridObj) {
					proxyRet = g_ProxyCrtDel.DEL_CHARACTER(nearPlayer->uiID, object->uiID);
					assert(proxyRet);
					proxyRet = g_ProxyCrtDel.DEL_CHARACTER(object->uiID, nearPlayer->uiID);
					assert(proxyRet);
				}
			}
		}

		// (2-1) ㅡ 변 삭제
		for (uint16 cx = leftCellBefore; cx <= rightCellBefore; cx++) {
			//if (cx != delCol.second) {
			//	uint16 cy = delRow.second;
			//	for (stObjectInfo* nearPlayer = grid->cells[cy][cx]; nearPlayer != nullptr; nearPlayer = nearPlayer->nextGridObj) {
			//		if (nearPlayer->uiID != object->uiID) {		// 삭제 대상에는 포워딩 대상 제외
			//			g_ProxyCrtDel.DEL_CHARACTER(nearPlayer->uiID, object->uiID);
			//			g_ProxyCrtDel.DEL_CHARACTER(object->uiID, nearPlayer->uiID);
			//		}
			//	}
			//}
			if (delCol.second.first <= cx && cx <= delCol.second.second) {
				continue;
			}
			else {
				for (uint16 cy = delRow.second.first; cy <= delRow.second.second; cy++) {
					for (stObjectInfo* nearPlayer = grid->cells[cy][cx]; nearPlayer != nullptr; nearPlayer = nearPlayer->nextGridObj) {
						if (nearPlayer->uiID != object->uiID) {		// 삭제 대상에는 포워딩 대상 제외
							proxyRet = g_ProxyCrtDel.DEL_CHARACTER(nearPlayer->uiID, object->uiID);
							assert(proxyRet);
							proxyRet = g_ProxyCrtDel.DEL_CHARACTER(object->uiID, nearPlayer->uiID);
							assert(proxyRet);
						}
					}
				}
			}
		}

		// (2-1) | 변 삭제
		for (uint16 cy = topCellBefore; cy <= bottomCellBefore; cy++) {
			//if (cy != delRow.second) {
			//	uint16 cx = delCol.second;
			//	for (stObjectInfo* nearPlayer = grid->cells[cy][cx]; nearPlayer != nullptr; nearPlayer = nearPlayer->nextGridObj) {
			//		if (nearPlayer->uiID != object->uiID) {		// 삭제 대상에는 포워딩 대상 제외
			//			g_ProxyCrtDel.DEL_CHARACTER(nearPlayer->uiID, object->uiID);
			//			g_ProxyCrtDel.DEL_CHARACTER(object->uiID, nearPlayer->uiID);
			//		}
			//	}
			//}
			if (delRow.second.first <= cy && cy <= delRow.second.second) {
				continue;
			}
			else {
				for(uint16 cx = delCol.second.first; cx <= delCol.second.second; cx++) {
					for (stObjectInfo* nearPlayer = grid->cells[cy][cx]; nearPlayer != nullptr; nearPlayer = nearPlayer->nextGridObj) {
						if (nearPlayer->uiID != object->uiID) {		// 삭제 대상에는 포워딩 대상 제외
							proxyRet = g_ProxyCrtDel.DEL_CHARACTER(nearPlayer->uiID, object->uiID);
							assert(proxyRet);
							proxyRet = g_ProxyCrtDel.DEL_CHARACTER(object->uiID, nearPlayer->uiID);
							assert(proxyRet);
						}
					}
				}
			}
		}
	}
	else if (delRow.first) {	// ㅡ 삭제
		//for (uint16 cx = leftCellBefore; cx <= rightCellBefore; cx++) {
		//	uint16 cy = delRow.second;
		//	for (stObjectInfo* nearPlayer = grid->cells[cy][cx]; nearPlayer != nullptr; nearPlayer = nearPlayer->nextGridObj) {
		//		if (nearPlayer->uiID != object->uiID) {		// 삭제 대상에는 포워딩 대상 제외
		//			g_ProxyCrtDel.DEL_CHARACTER(nearPlayer->uiID, object->uiID);
		//			g_ProxyCrtDel.DEL_CHARACTER(object->uiID, nearPlayer->uiID);
		//		}
		//	}
		//}
		for (uint16 cx = leftCellBefore; cx <= rightCellBefore; cx++) {
			for (uint16 cy = delRow.second.first; cy <= delRow.second.second; cy++) {
				for (stObjectInfo* nearPlayer = grid->cells[cy][cx]; nearPlayer != nullptr; nearPlayer = nearPlayer->nextGridObj) {
					if (nearPlayer->uiID != object->uiID) {		// 삭제 대상에는 포워딩 대상 제외
						proxyRet = g_ProxyCrtDel.DEL_CHARACTER(nearPlayer->uiID, object->uiID);
						assert(proxyRet);
						proxyRet = g_ProxyCrtDel.DEL_CHARACTER(object->uiID, nearPlayer->uiID);
						assert(proxyRet);
					}
				}
			}
		}
	}
	else if (delCol.first) {	// | 삭제
		//for (uint16 cy = topCellBefore; cy <= bottomCellBefore; cy++) {
		//	uint16 cx = delCol.second;
		//	for (stObjectInfo* nearPlayer = grid->cells[cy][cx]; nearPlayer != nullptr; nearPlayer = nearPlayer->nextGridObj) {
		//		if (nearPlayer->uiID != object->uiID) {		// 삭제 대상에는 포워딩 대상 제외
		//			g_ProxyCrtDel.DEL_CHARACTER(nearPlayer->uiID, object->uiID);
		//			g_ProxyCrtDel.DEL_CHARACTER(object->uiID, nearPlayer->uiID);
		//		}
		//	}
		//}
		for (uint16 cy = topCellBefore; cy <= bottomCellBefore; cy++) {
			for (uint16 cx = delCol.second.first; cx <= delCol.second.second; cx++) {
				for (stObjectInfo* nearPlayer = grid->cells[cy][cx]; nearPlayer != nullptr; nearPlayer = nearPlayer->nextGridObj) {
					if (nearPlayer->uiID != object->uiID) {		// 삭제 대상에는 포워딩 대상 제외
						proxyRet = g_ProxyCrtDel.DEL_CHARACTER(nearPlayer->uiID, object->uiID);
						assert(proxyRet);
						proxyRet = g_ProxyCrtDel.DEL_CHARACTER(object->uiID, nearPlayer->uiID);
						assert(proxyRet);
					}
				}
			}
		}
	}

	if (crtRow.first && crtCol.first) {	// ㄴ ㄱ . . 생성
		// (1) 꼭지점 생성
		//for (stObjectInfo* nearPlayer = grid->cells[crtRow.second][crtCol.second]; nearPlayer != nullptr; nearPlayer = nearPlayer->nextGridObj) {
		//	if (nearPlayer->uiID != object->uiID) {
		//		g_ProxyCrtDel.CRT_OTHER_CHARACTER(nearPlayer->uiID, object->uiID, object->byDir, object->stPos.usX, object->stPos.usY, object->byHP);
		//		g_ProxyMove.MOVE_START(nearPlayer->uiID, object->uiID, object->byDir, object->stPos.usX, object->stPos.usY);
		//		g_ProxyCrtDel.CRT_OTHER_CHARACTER(object->uiID, nearPlayer->uiID, nearPlayer->byDir, nearPlayer->stPos.usX, nearPlayer->stPos.usY, nearPlayer->byHP);
		//	}
		//}
		for (uint16 cx = crtCol.second.first; cx <= crtCol.second.second; cx++) {
			for (uint16 cy = crtRow.second.first; cy <= crtRow.second.second; cy++) {
				for (stObjectInfo* nearPlayer = grid->cells[cy][cx]; nearPlayer != nullptr; nearPlayer = nearPlayer->nextGridObj) {
					proxyRet = g_ProxyCrtDel.CRT_OTHER_CHARACTER(nearPlayer->uiID, object->uiID, object->byDir, object->stPos.usX, object->stPos.usY, object->byHP);
					assert(proxyRet);
					proxyRet = g_ProxyMove.MOVE_START(nearPlayer->uiID, object->uiID, object->byDir, object->stPos.usX, object->stPos.usY);
					assert(proxyRet);
					proxyRet = g_ProxyCrtDel.CRT_OTHER_CHARACTER(object->uiID, nearPlayer->uiID, nearPlayer->byDir, nearPlayer->stPos.usX, nearPlayer->stPos.usY, nearPlayer->byHP);
					assert(proxyRet);
				}
			}
		}

		// (2-1) ㅡ 변 생성
		for (uint16 cx = leftCell; cx <= rightCell; cx++) {
			//if (cx != crtCol.second) {
			//	uint16 cy = crtRow.second;
			//	for (stObjectInfo* nearPlayer = grid->cells[cy][cx]; nearPlayer != nullptr; nearPlayer = nearPlayer->nextGridObj) {
			//		if (nearPlayer->uiID != object->uiID) {
			//			g_ProxyCrtDel.CRT_OTHER_CHARACTER(nearPlayer->uiID, object->uiID, object->byDir, object->stPos.usX, object->stPos.usY, object->byHP);
			//			g_ProxyMove.MOVE_START(nearPlayer->uiID, object->uiID, object->byDir, object->stPos.usX, object->stPos.usY);
			//			g_ProxyCrtDel.CRT_OTHER_CHARACTER(object->uiID, nearPlayer->uiID, nearPlayer->byDir, nearPlayer->stPos.usX, nearPlayer->stPos.usY, nearPlayer->byHP);
			//		}
			//	}
			//}
			if (crtCol.second.first <= cx && cx <= crtCol.second.second) {
				continue;
			}
			else {
				for (uint16 cy = crtRow.second.first; cy <= crtRow.second.second; cy++) {
					for (stObjectInfo* nearPlayer = grid->cells[cy][cx]; nearPlayer != nullptr; nearPlayer = nearPlayer->nextGridObj) {
						if (nearPlayer->uiID != object->uiID) {		// 삭제 대상에는 포워딩 대상 제외
							proxyRet = g_ProxyCrtDel.CRT_OTHER_CHARACTER(nearPlayer->uiID, object->uiID, object->byDir, object->stPos.usX, object->stPos.usY, object->byHP);
							assert(proxyRet);
							proxyRet = g_ProxyMove.MOVE_START(nearPlayer->uiID, object->uiID, object->byDir, object->stPos.usX, object->stPos.usY);
							assert(proxyRet);
							proxyRet = g_ProxyCrtDel.CRT_OTHER_CHARACTER(object->uiID, nearPlayer->uiID, nearPlayer->byDir, nearPlayer->stPos.usX, nearPlayer->stPos.usY, nearPlayer->byHP);
							assert(proxyRet);
						}
					}
				}
			}
		}
		// (2-1) | 변 생성
		for (uint16 cy = topCell; cy <= bottomCell; cy++) {
			//if (cy != crtRow.second) {
			//	uint16 cx = crtCol.second;
			//	for (stObjectInfo* nearPlayer = grid->cells[cy][cx]; nearPlayer != nullptr; nearPlayer = nearPlayer->nextGridObj) {
			//		if (nearPlayer->uiID != object->uiID) {		// 삭제 대상에는 포워딩 대상 제외
			//			g_ProxyCrtDel.CRT_OTHER_CHARACTER(nearPlayer->uiID, object->uiID, object->byDir, object->stPos.usX, object->stPos.usY, object->byHP);
			//			g_ProxyMove.MOVE_START(nearPlayer->uiID, object->uiID, object->byDir, object->stPos.usX, object->stPos.usY);
			//			g_ProxyCrtDel.CRT_OTHER_CHARACTER(object->uiID, nearPlayer->uiID, nearPlayer->byDir, nearPlayer->stPos.usX, nearPlayer->stPos.usY, nearPlayer->byHP);
			//		}
			//	}
			//}
			if (crtRow.second.first <= cy && cy <= crtRow.second.second) {
				continue;
			}
			else {
				for (uint16 cx = crtCol.second.first; cx <= crtCol.second.second; cx++) {
					for (stObjectInfo* nearPlayer = grid->cells[cy][cx]; nearPlayer != nullptr; nearPlayer = nearPlayer->nextGridObj) {
						if (nearPlayer->uiID != object->uiID) {		// 삭제 대상에는 포워딩 대상 제외
							proxyRet = g_ProxyCrtDel.CRT_OTHER_CHARACTER(nearPlayer->uiID, object->uiID, object->byDir, object->stPos.usX, object->stPos.usY, object->byHP);
							proxyRet = g_ProxyMove.MOVE_START(nearPlayer->uiID, object->uiID, object->byDir, object->stPos.usX, object->stPos.usY);
							proxyRet = g_ProxyCrtDel.CRT_OTHER_CHARACTER(object->uiID, nearPlayer->uiID, nearPlayer->byDir, nearPlayer->stPos.usX, nearPlayer->stPos.usY, nearPlayer->byHP);
						}
					}
				}
			}
		}
	}
	else if (crtRow.first) {	
		for (uint16 cx = leftCell; cx <= rightCell; cx++) {
			for (uint16 cy = crtRow.second.first; cy <= crtRow.second.second; cy++) {
				for (stObjectInfo* nearPlayer = grid->cells[cy][cx]; nearPlayer != nullptr; nearPlayer = nearPlayer->nextGridObj) {
					if (nearPlayer->uiID != object->uiID) {		// 삭제 대상에는 포워딩 대상 제외
						proxyRet = g_ProxyCrtDel.CRT_OTHER_CHARACTER(nearPlayer->uiID, object->uiID, object->byDir, object->stPos.usX, object->stPos.usY, object->byHP);
						proxyRet = g_ProxyMove.MOVE_START(nearPlayer->uiID, object->uiID, object->byDir, object->stPos.usX, object->stPos.usY);
						proxyRet = g_ProxyCrtDel.CRT_OTHER_CHARACTER(object->uiID, nearPlayer->uiID, nearPlayer->byDir, nearPlayer->stPos.usX, nearPlayer->stPos.usY, nearPlayer->byHP);
					}
				}
			}
		}
	}
	else if (crtCol.first) {	// | 생성
		//for (uint16 cy = topCell; cy <= bottomCell; cy++) {
		//	uint16 cx = crtCol.second;
		//	for (stObjectInfo* nearPlayer = grid->cells[cy][cx]; nearPlayer != nullptr; nearPlayer = nearPlayer->nextGridObj) {
		//		if (nearPlayer->uiID != object->uiID) {		// 삭제 대상에는 포워딩 대상 제외
		//			g_ProxyCrtDel.CRT_OTHER_CHARACTER(nearPlayer->uiID, object->uiID, object->byDir, object->stPos.usX, object->stPos.usY, object->byHP);
		//			g_ProxyMove.MOVE_START(nearPlayer->uiID, object->uiID, object->byDir, object->stPos.usX, object->stPos.usY);
		//			g_ProxyCrtDel.CRT_OTHER_CHARACTER(object->uiID, nearPlayer->uiID, nearPlayer->byDir, nearPlayer->stPos.usX, nearPlayer->stPos.usY, nearPlayer->byHP);
		//		}
		//	}
		//}
		for (uint16 cy = topCell; cy <= bottomCell; cy++) {
			for (uint16 cx = crtCol.second.first; cx <= crtCol.second.second; cx++) {
				for (stObjectInfo* nearPlayer = grid->cells[cy][cx]; nearPlayer != nullptr; nearPlayer = nearPlayer->nextGridObj) {
					if (nearPlayer->uiID != object->uiID) {		// 삭제 대상에는 포워딩 대상 제외
						proxyRet = g_ProxyCrtDel.CRT_OTHER_CHARACTER(nearPlayer->uiID, object->uiID, object->byDir, object->stPos.usX, object->stPos.usY, object->byHP);
						assert(proxyRet);
						proxyRet = g_ProxyMove.MOVE_START(nearPlayer->uiID, object->uiID, object->byDir, object->stPos.usX, object->stPos.usY);
						assert(proxyRet);
						proxyRet = g_ProxyCrtDel.CRT_OTHER_CHARACTER(object->uiID, nearPlayer->uiID, nearPlayer->byDir, nearPlayer->stPos.usX, nearPlayer->stPos.usY, nearPlayer->byHP);
						assert(proxyRet);
					}
				}
			}
		}
	}
}
void ForwardMsgToNear(stObjectInfo* object, Grid* grid, RpcID msgID) {
	bool proxyRet;
	uint16 topCell;
	uint16 bottomCell;
	uint16 leftCell;
	uint16 rightCell;
	GetRangeCell(object->stPos, topCell, bottomCell, leftCell, rightCell);

	switch (msgID) {
	case FightGameCrtDel_S2C::RPC_DEL_CHARACTER: {
		//BYTE bodyLen = sizeof(object->uiID);
		for (uint16_t cy = topCell; cy <= bottomCell; cy++) {
			for (uint16_t cx = leftCell; cx <= rightCell; cx++) {
				for (stObjectInfo* nearPlayer = grid->cells[cy][cx]; nearPlayer != nullptr; nearPlayer = nearPlayer->nextGridObj) {
					//if (nearPlayer->uiID != object->uiID) {		// 삭제 대상에는 포워딩 대상 제외
						proxyRet = g_ProxyCrtDel.DEL_CHARACTER(nearPlayer->uiID, object->uiID);
						assert(proxyRet);
					//}
					// => 클라이언트 동작 분석 결과 삭제 대상에도 delete 메시지를 보내주었어야 했음
				}
			}
		}
		break;
	}
	case FightGameMove_S2C::RPC_MOVE_START: {
		//BYTE bodyLen = sizeof(object->uiID) + sizeof(object->byDir) + sizeof(object->stPos.usX) + sizeof(object->stPos.usY);
		for (uint16_t cy = topCell; cy <= bottomCell; cy++) {
			for (uint16_t cx = leftCell; cx <= rightCell; cx++) {
				for (stObjectInfo* nearPlayer = grid->cells[cy][cx]; nearPlayer != nullptr; nearPlayer = nearPlayer->nextGridObj) {
					if (nearPlayer->uiID != object->uiID) {		// 삭제 대상에는 포워딩 대상 제외
						//std::cout << "[Forward Start] X: " << x << ", Y: " << y << std::endl;
						proxyRet = g_ProxyMove.MOVE_START(nearPlayer->uiID, object->uiID, object->byDir, object->stPos.usX, object->stPos.usY);
						assert(proxyRet);
					}
				}
			}
		}
		break;
	}
	case FightGameMove_S2C::RPC_MOVE_STOP: {
		//BYTE bodyLen = sizeof(object->uiID) + sizeof(object->byDir) + sizeof(object->stPos.usX) + sizeof(object->stPos.usY);
		for (uint16_t cy = topCell; cy <= bottomCell; cy++) {
			for (uint16_t cx = leftCell; cx <= rightCell; cx++) {
				for (stObjectInfo* nearPlayer = grid->cells[cy][cx]; nearPlayer != nullptr; nearPlayer = nearPlayer->nextGridObj) {
					if (nearPlayer->uiID != object->uiID) {		// 삭제 대상에는 포워딩 대상 제외
						//std::cout << "[Forward Stop] X: " << x << ", Y: " << y << std::endl;
						proxyRet = g_ProxyMove.MOVE_STOP(nearPlayer->uiID, object->uiID, object->byDir, object->stPos.usX, object->stPos.usY);
						assert(proxyRet);
					}
				}
			}
		}
		break;
	}
	case FightGameAttack_S2C::RPC_ATTACK1: {
		//BYTE bodyLen = sizeof(object->uiID) + sizeof(object->byDir) + sizeof(object->stPos.usX) + sizeof(object->stPos.usY);
		for (uint16_t cy = topCell; cy <= bottomCell; cy++) {
			for (uint16_t cx = leftCell; cx <= rightCell; cx++) {
				for (stObjectInfo* nearPlayer = grid->cells[cy][cx]; nearPlayer != nullptr; nearPlayer = nearPlayer->nextGridObj) {
					if (nearPlayer->uiID != object->uiID) {		// 삭제 대상에는 포워딩 대상 제외
						proxyRet = g_ProxyAttack.ATTACK1(nearPlayer->uiID, object->uiID, object->byDir, object->stPos.usX, object->stPos.usY);
						assert(proxyRet);
					}
				}
			}
		}
		break;
	}
	case FightGameAttack_S2C::RPC_ATTACK2: {
		//BYTE bodyLen = sizeof(object->uiID) + sizeof(object->byDir) + sizeof(object->stPos.usX) + sizeof(object->stPos.usY);
		for (uint16_t cy = topCell; cy <= bottomCell; cy++) {
			for (uint16_t cx = leftCell; cx <= rightCell; cx++) {
				for (stObjectInfo* nearPlayer = grid->cells[cy][cx]; nearPlayer != nullptr; nearPlayer = nearPlayer->nextGridObj) {
					if (nearPlayer->uiID != object->uiID) {		// 삭제 대상에는 포워딩 대상 제외
						proxyRet = g_ProxyAttack.ATTACK2(nearPlayer->uiID, object->uiID, object->byDir, object->stPos.usX, object->stPos.usY);
						assert(proxyRet);
					}
				}
			}
		}
		break;
	}
	case FightGameAttack_S2C::RPC_ATTACK3: {
		//BYTE bodyLen = sizeof(object->uiID) + sizeof(object->byDir) + sizeof(object->stPos.usX) + sizeof(object->stPos.usY);
		for (uint16_t cy = topCell; cy <= bottomCell; cy++) {
			for (uint16_t cx = leftCell; cx <= rightCell; cx++) {
				for (stObjectInfo* nearPlayer = grid->cells[cy][cx]; nearPlayer != nullptr; nearPlayer = nearPlayer->nextGridObj) {
					if (nearPlayer->uiID != object->uiID) {		// 삭제 대상에는 포워딩 대상 제외
						proxyRet = g_ProxyAttack.ATTACK3(nearPlayer->uiID, object->uiID, object->byDir, object->stPos.usX, object->stPos.usY);
						assert(proxyRet);
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
	bool proxyRet;
	uint16 topCell;
	uint16 bottomCell;
	uint16 leftCell;
	uint16 rightCell;
	GetRangeCell(object->stPos, topCell, bottomCell, leftCell, rightCell);

	//BYTE bodyLen = sizeof(object->uiID) + sizeof(object->byDir) + sizeof(object->stPos) + sizeof(object->byHP);

	for (uint16 cy = topCell; cy <= bottomCell; cy++) {
		for (uint16 cx = leftCell; cx <= rightCell; cx++) {
			stObjectInfo* nearobj = grid->cells[cy][cx];
			for (stObjectInfo* nearPlayer = grid->cells[cy][cx]; nearPlayer != nullptr; nearPlayer = nearPlayer->nextGridObj) {
				proxyRet = g_ProxyCrtDel.CRT_OTHER_CHARACTER(nearPlayer->uiID, object->uiID, object->byDir, object->stPos.usX, object->stPos.usY, object->byHP);
				assert(proxyRet);
				proxyRet = g_ProxyCrtDel.CRT_OTHER_CHARACTER(object->uiID, nearPlayer->uiID, nearPlayer->byDir, nearPlayer->stPos.usX, nearPlayer->stPos.usY, nearPlayer->byHP);
				assert(proxyRet);
			}
		}
	}
}
void ForwardDmgMsg(stObjectInfo* attacker, stObjectInfo* target, Grid* grid) {
	bool proxyRet;
	uint16 topCellAttaker = 0;
	uint16 bottomCellAttaker = 0;
	uint16 leftCellAttaker = 0;
	uint16 rightCellAttaker = 0;

	GetRangeCell(attacker->stPos, topCellAttaker, bottomCellAttaker, leftCellAttaker, rightCellAttaker);

	//uint16 topCellTarget;
	//uint16 bottomCellTarget;
	//uint16 leftCellTarget;
	//uint16 rightCellTarget;

	//BYTE bodyLen = sizeof(attacker->uiID) + sizeof(target->uiID) + sizeof(BYTE);

	for (uint16_t cy = topCellAttaker; cy <= bottomCellAttaker; cy++) {
		for (uint16_t cx = leftCellAttaker; cx <= rightCellAttaker; cx++) {
			for (stObjectInfo* nearPlayer = grid->cells[cy][cx]; nearPlayer != nullptr; nearPlayer = nearPlayer->nextGridObj) {
				proxyRet = g_ProxyDamage.DAMAGE(nearPlayer->uiID, attacker->uiID, target->uiID, target->byHP);
				assert(proxyRet);
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
	bool proxyRet;
	//BYTE bodyLen = sizeof(uint32_t) + sizeof(uint16_t) + sizeof(uint16_t);
	proxyRet = g_ProxyComm.SYNC(player->uiID, player->uiID, player->stPos.usX, player->stPos.usY);
	assert(proxyRet);

	g_SyncPerLoop++;
}

//////////////////////////////
// g_TimeSet 갱신
//////////////////////////////
void ResetTime(stObjectInfo* player) {
#if defined(TIME_SET)
	auto playerTimeIter = g_TimeSet.find({ player->lastEchoTime, player->uiID });
	if (playerTimeIter != g_TimeSet.end()) {
		g_TimeSet.erase(playerTimeIter);
	}
	else {
		assert(playerTimeIter != g_TimeSet.end());
	}

	g_TimeSet.insert({ g_Time, player->uiID });
	player->lastEchoTime = g_Time;
#else
	player->lastEchoTime = g_Time;
#endif
}

//////////////////////////////
// 캐릭터 생성
//////////////////////////////
stPoint GetRandomPosition() {
	stPoint pos;
	//srand(time(NULL));
	pos.usX = (rand() % (dfRANGE_MOVE_RIGHT - dfRANGE_MOVE_LEFT - 1)) + dfRANGE_MOVE_LEFT + 1;
	pos.usY = (rand() % (dfRANGE_MOVE_BOTTOM - dfRANGE_MOVE_TOP - 1)) + dfRANGE_MOVE_TOP + 1;
	return pos;
}
void CreateFighter(HostID hostID) {
#ifdef  DEFAULT_POOL
	stObjectInfo* newObject = new stObjectInfo();
#endif //  DEFAULT_POOL
#ifdef JINI_POOL
	stObjectInfo* newObject = reinterpret_cast<stObjectInfo*>(g_ObjectPool.AllocMem());
#endif // JINI_POOL
	newObject->uiID = hostID;
#if defined(CLIENT_DEBUG)
	newObject->stPos.usX = 0;
	newObject->stPos.usY = 0;
#else
	newObject->stPos = GetRandomPosition();
#endif
	newObject->byHP = dfDEAFAULT_INIT_HP;
	newObject->byDir = rand() % 2 == 0 ? dfPACKET_MOVE_DIR_LL : dfPACKET_MOVE_DIR_RR;

#ifdef SYNC_TEST
	newObject->beforePos = newObject->stPos;
#endif // SYNC_TEST

	
	/***** 중요 결함 *****/
	// 객체 풀을 통해서 객체 메모리를 할당받고 있다.
	// 따라서 별도의 생성자가 호출되지 않는다.
	// 서버가 이용하는 데이터
	//bool bMoveFlag = false;
	//bool bFirstMoveFlag = false;
	//stObjectInfo* nextGridObj = nullptr;
	//stObjectInfo* prevGridObj = nullptr;
	// 위와 같은 초기화가 이루어지지 않는다.
	// 결국 따로 초기화하든, placement_new를 호출해주어야 한다.
	newObject->bMoveFlag = false;
	//newObject->bFirstMoveFlag = false;
	newObject->nextGridObj = nullptr;
	newObject->prevGridObj = nullptr;

	//std::cout << "[CREATE] S(" << newObject->stPos.usX << ", " << newObject->stPos.usY << ")" << std::endl;

	// 전역 타이머로 초기화
	newObject->lastEchoTime = g_Time;
	//g_TimeMap.insert({ g_Time, hostID });
	g_TimeSet.insert({ g_Time, hostID });	// 부호를 변경하여 삽입, 시간 기준 오름차순 정렬

	//BYTE bodyLen = sizeof(newObject->uiID) + sizeof(newObject->byDir) + sizeof(newObject->stPos) + sizeof(newObject->byHP);
	bool proxyRet = g_ProxyCrtDel.CRT_CHARACTER(newObject->uiID, newObject->uiID, newObject->byDir, newObject->stPos.usX, newObject->stPos.usY, newObject->byHP);
	assert(proxyRet);

	if (g_ClientMap.find(hostID) == g_ClientMap.end()) {
		g_ClientMap.insert({ hostID, newObject });
		ForwardCRTMsg(newObject, &g_Grid);
		g_Grid.Add(newObject);
	}
	else {
		// 코어로부터 생성 지시가 들어왔지만, 생성하고자 하는 HostID가 이미 클라언트 맵에 존재하는 상황이 발생하였음.
		// 아래 상황이 의심됨
		// (1) 코어(JNet)에러: 해당 ID는 유효한(살아있는) 플레이어의 ID임에도 불구하고 코어 쪽에서의 ID 할당 오류로 인해 삭제된 ID라 생각하여 새로운 생성 ID로써 전달
		// (2) 컨텐츠 에러: 코어에서 전달된 삭제 지시를 컨텐츠 단에서 수행하지 못하여 존재하였던 ID
		//ERROR_EXCEPTION_WINDOW(L"CreateFighter", L"gClientMap.find(hostID) != gClientMap.end()");
	}
}
void DeleteFighter(HostID hostID, bool netcoreSide) {
	if (g_DeleteClientSet.find(hostID) == g_DeleteClientSet.end()) {
		//size_t setSize = g_DeleteClientSet.size();
		//cout << "g_DeleteClientSet.size(): " << g_DeleteClientSet.size() << endl;
		g_DeleteClientSet.insert({ hostID, netcoreSide });
	}
}

//////////////////////////////
// 캐릭터 이동 및 중지
//////////////////////////////
void MoveFigter(HostID hostID, BYTE Direction, uint16_t X, uint16_t Y) {
	// 캐릭터가 죽기 전 move 메시지가 전송됨을 확인(추측)

	auto playerIter = g_ClientMap.find(hostID);
	if (playerIter == g_ClientMap.end()) {
		assert(playerIter != g_ClientMap.end());
		return;
	}
	stObjectInfo* player = playerIter->second;
	ResetTime(player);

	player->byDir = Direction;
	player->bMoveFlag = true;
	//player->bFirstMoveFlag = true;

	// 좌표가 틀어지면 클라이언트를 끊는 것이 아니라 Sync를 맞춘다.
	if (abs(X - player->stPos.usX) > dfERROR_RANGE || abs(Y - player->stPos.usY) > dfERROR_RANGE) {
		// 좌표 오차가 dfERROR_RANGE보다 크면 SYNC 메시지 전송
#ifdef SYNC_TEST
		std::cout << "[SYNC_START] C(" << X << ", " << Y << ") S(" << player->stPos.usX << ", " << player->stPos.usY << ")" << std::endl;
		std::cout << "Sbefore(" << player->beforePos.usX << ", " << player->beforePos.usY << ")" << std::endl;
#endif // SYNC_TEST		
		SyncPosition(player);
	}
	else {
		// 아니라면 클라이언트 좌표로 미세한 오차를 맞춘다.
		if (player->stPos.usY != Y || player->stPos.usX != X) {
			g_Grid.Delete(player);
			stPoint beforePos = player->stPos;
			player->stPos.usY = Y;
			player->stPos.usX = X;
			GridResetInterestSpace(beforePos, player, &g_Grid, false);
			g_Grid.Add(player);
		}
	}
	
#ifdef SYNC_TEST
	player->beforePos = player->stPos;
#endif // SYNC_TEST
	ForwardMsgToNear(player, &g_Grid, FightGameMove_S2C::RPC_MOVE_START);
}
void StopFigther(HostID hostID, BYTE Direction, uint16_t X, uint16_t Y) {
	// 캐릭터가 죽기 전 move 메시지가 전송됨을 확인(추측)
	
	auto playerIter = g_ClientMap.find(hostID);
	if (playerIter == g_ClientMap.end()) {
		assert(playerIter != g_ClientMap.end());
		return;
	}
	stObjectInfo* player = playerIter->second;
	ResetTime(player);

	player->byDir = Direction;
	player->bMoveFlag = false;

	// 좌표가 틀어지면 클라이언트를 끊는 것이 아니라 Sync를 맞춘다.
	if (abs(X - player->stPos.usX) > dfERROR_RANGE || abs(Y - player->stPos.usY) > dfERROR_RANGE) {
#ifdef SYNC_TEST
		// 좌표 오차가 dfERROR_RANGE보다 크면 SYNC 메시지 전송
		std::cout << "[SYNC_STOP] C(" << X << ", " << Y << ") S(" << player->stPos.usX << ", " << player->stPos.usY << ")" << std::endl;
		std::cout << "Sbefore(" << player->beforePos.usX << ", " << player->beforePos.usY << ")" << std::endl;
#endif // SYNC_TEST		
		SyncPosition(player);
	}
	else {
		// 아니라면 클라이언트 좌표로 미세한 오차를 맞춘다.
		if (player->stPos.usY != Y || player->stPos.usX != X) {
			g_Grid.Delete(player);
			stPoint beforePos = player->stPos;
			player->stPos.usY = Y;
			player->stPos.usX = X;
			GridResetInterestSpace(beforePos, player, &g_Grid, false);
			g_Grid.Add(player);
		}
	}

#ifdef SYNC_TEST
	player->beforePos = player->stPos;
#endif // SYNC_TEST
	ForwardMsgToNear(player, &g_Grid, FightGameMove_S2C::RPC_MOVE_STOP);
}

//////////////////////////////
// 캐릭터 공격
//////////////////////////////
void AttackFighter(HostID hostID, BYTE Direction, uint16_t X, uint16_t Y, enAttackType atkType) {
	// 캐릭터가 죽기 전 move 메시지가 전송됨을 확인(추측)
	
	auto playerIter = g_ClientMap.find(hostID);
	if (playerIter == g_ClientMap.end()) {
		assert(playerIter != g_ClientMap.end());
		return;
	}
	stObjectInfo* player = playerIter->second;
	ResetTime(player);

	player->byDir = Direction;

	// 좌표가 틀어지면 클라이언트를 끊는 것이 아니라 Sync를 맞춘다.
	if (abs(X - player->stPos.usX) > dfERROR_RANGE || abs(Y - player->stPos.usY) > dfERROR_RANGE) {
		// 좌표 오차가 dfERROR_RANGE보다 크면 SYNC 메시지 전송
		SyncPosition(player);
	}
	else {
		// 아니라면 클라이언트 좌표로 미세한 오차를 맞춘다.
		if (player->stPos.usY != Y || player->stPos.usX != X) {
			g_Grid.Delete(player);
			stPoint beforePos = player->stPos;
			player->stPos.usY = Y;
			player->stPos.usX = X;
			GridResetInterestSpace(beforePos, player, &g_Grid, false);
			g_Grid.Add(player);
		}
	}

#ifdef SYNC_TEST
	player->beforePos = player->stPos;
#endif // SYNC_TEST

	// 작업 큐 큐잉
	stAttackWork atkWork;
	atkWork.uiID = player->uiID;
	atkWork.byDir = player->byDir;
	atkWork.usX = player->stPos.usX;
	atkWork.usY = player->stPos.usY;
	atkWork.byType = atkType;
	g_AtkWorkQueue.push(atkWork);

	// 주변 클라이언트에 ATTACK 메시지 포워딩
	switch (atkType) {
	case enAttackType::ATTACK1: {
		ForwardMsgToNear(player, &g_Grid, FightGameAttack_S2C::RPC_ATTACK1);
		break;
	}
	case enAttackType::ATTACK2: {
		ForwardMsgToNear(player, &g_Grid, FightGameAttack_S2C::RPC_ATTACK2);
		break;
	}
	case enAttackType::ATTACK3: {
		ForwardMsgToNear(player, &g_Grid, FightGameAttack_S2C::RPC_ATTACK3);
		break;
	}
	}
}

void ReceiveEcho(HostID hostID, uint32_t time)
{
	//BYTE bodyLen = sizeof(time);
	bool proxyRet = g_ProxyComm.ECHO(hostID, time);
	assert(proxyRet);

	auto playerIter = g_ClientMap.find(hostID);
	if (playerIter != g_ClientMap.end()) {
		ResetTime(playerIter->second);
	}
	else {
		assert(playerIter != g_ClientMap.end());
	}
}

//////////////////////////////
// Batch Process
//////////////////////////////
void BatchSyncLog() {
	if (g_SyncPerLoop > 0) {
		cout << "[SYNC] sync count: " << g_SyncPerLoop << endl;
		cout << "TimeOutCheck: " << g_Duration_TimeOutCheck << endl;
		cout << "AttackWork: " << g_Duration_AttackWork << endl;
		cout << "MoveWork: " << g_Duration_MoveWork << endl;
		cout << "DeleteClient: " << g_Duration_DeleteClient << endl;
		cout << "DelectClientCnt: " << g_DeleteClientCnt << endl;
		cout << "DisconnectedClientCnt: " << g_DisconnectedClient << endl;

		g_SyncPerLoop = 0;
	}
}

void BatchDeleteClientWork() {

	g_DeleteClientCnt = 0;
	g_DisconnectedClient = 0;
	g_Duration_DeleteClient = clock();

	bool proxyRet;

	for (auto delCleint : g_DeleteClientSet) {
		HostID hostID = delCleint.first;
		bool netcoreSide = delCleint.second;

		if (g_ClientMap.find(hostID) != g_ClientMap.end()) {
			stObjectInfo* client = g_ClientMap[hostID];

			// 코어에 연결 종료 요청
			if (!netcoreSide) {
				g_ProxyCrtDel.Disconnect(hostID);
			}
			ForwardMsgToNear(client, &g_Grid, FightGameCrtDel_S2C::RPC_DEL_CHARACTER);
			g_Grid.Delete(client);

#ifdef DEFAULT_POOL
			delete client;
#endif // DEFAULT_POOL
#ifdef JINI_POOL
			g_ObjectPool.ReturnMem(reinterpret_cast<BYTE*>(client));
#endif // JINI_POOL

			g_ClientMap.erase(hostID);
			g_DeleteClientCnt++;
		}
		//else {
		//	ERROR_EXCEPTION_WINDOW(L"BatchDeleteClientWork", L"gClientMap.find(hostID) == gClientMap.end()");
		//}
		// => 위 분기가 발생하는 것이 잘못된 흐름인가?
	}
	g_DeleteClientSet.clear();

	g_Duration_DeleteClient = clock() - g_Duration_DeleteClient;
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

	if (g_ClientMap.find(targetID) != g_ClientMap.end() && g_ClientMap.find(atkerID) != g_ClientMap.end()) {
		stObjectInfo* attacker = g_ClientMap[atkerID];
		stObjectInfo* target = g_ClientMap[targetID];
		if (target->byHP <= damage) {
			// 죽음
			target->byHP = 0;
			DeleteFighter(targetID);
		}
		else {
			target->byHP -= damage;
		}
		ForwardDmgMsg(attacker, target, &g_Grid);
	}
	else {
		//ERROR_EXCEPTION_WINDOW(L"AttackWork(..)", L"공격자 또는 타겟 대상 없음");
	}
}
void BatchAttackWork() {

	g_Duration_AttackWork = clock();

	while (!g_AtkWorkQueue.empty()) {
		stAttackWork& atkWork = g_AtkWorkQueue.front();
		g_AtkWorkQueue.pop();

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
			//ERROR_EXCEPTION_WINDOW(L"MAIN(공격 처리)", L"UNVALID ATK TYPE");
		}

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
				for (stObjectInfo* target = g_Grid.cells[cy][cx]; target != nullptr; target = target->nextGridObj) {
					if (atkWork.uiID != target->uiID) {
						if (atkRangeTop <= target->stPos.usY && target->stPos.usY <= atkRangeBottom && atkRangeLeft <= target->stPos.usX && target->stPos.usX <= atkRangeRight) {
							AttackWork(atkWork.uiID, target->uiID, atkWork.byType);
						}
					}
				}
			}
		}
	}

	g_Duration_AttackWork = clock() - g_Duration_AttackWork;
}
void BatchMoveWork(uint16 loopDelta) {
	if (loopDelta <= 0) {
		g_Duration_MoveWork = clock();
		return;
	}

	g_Duration_MoveWork = clock();

	for (auto iter : g_ClientMap) {
		stObjectInfo* object = iter.second;
		if (object->bMoveFlag) {
			g_Grid.Delete(object);

			stPoint beforePos = object->stPos;
			switch (object->byDir) {
			case dfPACKET_MOVE_DIR_LL:
				if (object->stPos.usX < DELTA_X * (loopDelta)) {			// 오버 프래임 로직 추가
					object->stPos.usX = dfRANGE_MOVE_LEFT;
					object->bMoveFlag = false;
				}
				else {
					object->stPos.usX -= DELTA_X * (loopDelta);
				}
				break;

			case dfPACKET_MOVE_DIR_LU:
				if (object->stPos.usX < DELTA_X * (loopDelta)) {
					object->stPos.usX = dfRANGE_MOVE_LEFT;
					object->bMoveFlag = false;
				}
				else {
					object->stPos.usX -= DELTA_X * (loopDelta);
				}
				if (object->stPos.usY < DELTA_Y * (loopDelta)) {
					object->stPos.usY = dfRANGE_MOVE_TOP;
					object->bMoveFlag = false;
				}
				else {
					object->stPos.usY -= DELTA_Y * (loopDelta);
				}
				break;

			case dfPACKET_MOVE_DIR_UU:
				if (object->stPos.usY < DELTA_Y * (loopDelta)) {
					object->stPos.usY = dfRANGE_MOVE_TOP;
					object->bMoveFlag = false;
				}
				else {
					object->stPos.usY -= DELTA_Y * (loopDelta);
				}
				break;

			case dfPACKET_MOVE_DIR_RU:
				if (object->stPos.usX + DELTA_X * (loopDelta) > dfRANGE_MOVE_RIGHT) {
					object->stPos.usX = dfRANGE_MOVE_RIGHT;
					object->bMoveFlag = false;
				}
				else {
					object->stPos.usX += DELTA_X * (loopDelta);
				}
				if (object->stPos.usY < DELTA_Y * (loopDelta)) {
					object->stPos.usY = dfRANGE_MOVE_TOP;
					object->bMoveFlag = false;
				}
				else {
					object->stPos.usY -= DELTA_Y * (loopDelta);
				}
				break;

			case dfPACKET_MOVE_DIR_RR:
				if (object->stPos.usX + DELTA_X * (loopDelta) > dfRANGE_MOVE_RIGHT) {
					object->stPos.usX = dfRANGE_MOVE_RIGHT;
					object->bMoveFlag = false;
				}
				else {
					object->stPos.usX += DELTA_X * (loopDelta);
				}
				break;

			case dfPACKET_MOVE_DIR_RD:
				if (object->stPos.usX + DELTA_X * (loopDelta) > dfRANGE_MOVE_RIGHT) {
					object->stPos.usX = dfRANGE_MOVE_RIGHT;
					object->bMoveFlag = false;
				}
				else {
					object->stPos.usX += DELTA_X * (loopDelta);
				}
				if (object->stPos.usY + DELTA_Y * (loopDelta) > dfRANGE_MOVE_BOTTOM) {
					object->stPos.usY = dfRANGE_MOVE_BOTTOM;
					object->bMoveFlag = false;
				}
				else {
					object->stPos.usY += DELTA_Y * (loopDelta);
				}
				break;

			case dfPACKET_MOVE_DIR_DD:
				if (object->stPos.usY + DELTA_Y * (loopDelta) > dfRANGE_MOVE_BOTTOM) {
					object->stPos.usY = dfRANGE_MOVE_BOTTOM;
					object->bMoveFlag = false;
				}
				else {
					object->stPos.usY += DELTA_Y * (loopDelta);
				}
				break;

			case dfPACKET_MOVE_DIR_LD:
				if (object->stPos.usX < DELTA_X * (loopDelta)) {
					object->stPos.usX = dfRANGE_MOVE_LEFT;
					object->bMoveFlag = false;
				}
				else {
					object->stPos.usX -= DELTA_X * (loopDelta);
				}
				if (object->stPos.usY + DELTA_Y * (loopDelta) > dfRANGE_MOVE_BOTTOM) {
					object->stPos.usY = dfRANGE_MOVE_BOTTOM;
					object->bMoveFlag = false;
				}
				else {
					object->stPos.usY += DELTA_Y * (loopDelta);
				}
				break;
			default:
				break;
			}

			GridResetInterestSpace(beforePos, object, &g_Grid, true);
			g_Grid.Add(object);
		}
	}

	g_Duration_MoveWork = clock() - g_Duration_MoveWork;
}

void BatchTimeOutCheck() {
	//SLEEP_TIME_MS;
	// 1000ms / SLEEP_TIME_MS = N
	// N 번에 한번 씩 갱신

	static uint16 s_TimeCheckDuration = 1000 / SLEEP_TIME_MS;
	static time_t s_LoopCnt = 0;

	g_Duration_TimeOutCheck = clock();

	g_Time = time(NULL);
	s_LoopCnt++;

	if (s_LoopCnt % s_TimeCheckDuration == 0) {
#if defined(TIME_SET)
		for (auto iter = g_TimeSet.begin(); iter != g_TimeSet.end();) {
			if (g_Time - iter->first > 30) {
				DeleteFighter(iter->second);
				iter = g_TimeSet.erase(iter);
			}
			else {
				break;
			}
		}

#else
		static time_t s_MinLastTime = 0;

		//cout << g_Time - s_MinLastTime << endl;
		if (g_Time - s_MinLastTime > 30) {
			s_MinLastTime = MAXLONGLONG;

			// 탐색 시 30 초 미만의 시간 중 최대를 저장하고, 이를 초마다 카운팅 -> 30초가 넘어가면 for문 순회로, for문 전체 순회를 매초마다 하지 않도록 함.
			for (auto iter = g_ClientMap.begin(); iter != g_ClientMap.end(); iter++) {
				if (g_Time - iter->second->lastEchoTime > 30) {
					cout << "Time Out!! ID: " << iter->second->uiID << endl;
					DeleteFighter(iter->second->uiID);
				}
				else {
					s_MinLastTime = min(s_MinLastTime, iter->second->lastEchoTime);
				}
			}

			if (s_MinLastTime == MAXLONGLONG) {
				s_MinLastTime = 0;
			}
		}
#endif
	}

	g_Duration_TimeOutCheck = clock() - g_Duration_TimeOutCheck;
}

void BatchPrintLog() {
	static uint32 logTime = 0;
	
	if (logTime % 100 == 0) {
		cout << "===================================" << endl;
		cout << "캐릭터 수: " << g_ClientMap.size() << endl;
		for (auto iter = g_TimeSet.begin(); iter != g_TimeSet.end(); iter++) {
			if (logTime % 10 == 0) {
				cout << "lastEchoTime: " << iter->first << ", hostID: " << iter->second << endl;
			}
		}
		cout << "===================================" << endl;
	}

	logTime++;
}