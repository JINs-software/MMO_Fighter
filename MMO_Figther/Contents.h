#pragma once
#include <map>
#include <set>
#include <vector>
#include <queue>
#include <mutex>
#include <time.h>
#include "Configuration.h"
#include "GameObject.h"

#include "JNetProxy.h"
#include "RPC/Proxy_FightGame.h"

#include "ExceptionHandler.h"

#include "JNetPool.h"

extern std::map<HostID, stObjectInfo*> gClientMap;
extern std::vector<std::vector<stObjectInfo*>> gClientGrid;
extern std::map<HostID, bool> gDeleteClientSet;
extern std::queue<stAttackWork> AtkWorkQueue;

extern time_t gTime;

//=========================
// ������ ����� ���� ����
//-------------------------
void InsertToGrid(uint16_t Y, uint16_t X, HostID id);
void DeleteFromGrid(uint16_t Y, uint16_t X, HostID id);
// ĳ���� ����, ���� �׸��� MOVE/ATTACK �� �ֺ� ������ �÷��̾ �޽��� ������
void FowardCRTMsg(stObjectInfo* newObject);
void ForwardDmgMsg(stObjectInfo* attacker, stObjectInfo* target);
void ForwardMsgToNear(stObjectInfo* player, RpcID msgID);
//-------------------------
// ĳ���� �̵� �߰� �߰� ���� ���� ����
// ���ο� ���� �������� CRT/MOVE �޽��� ����
// ���� ��󿡼� ���ܵ� �������� DEL �޽��� ����
void FowardCrtDelMsgByMove(stObjectInfo* player, const stPoint& beforePos);
//=========================

//=========================
// ���� ũ�� ���� ���� ����
//-------------------------
// ���� ũ�� ���� ���� ����
void GetRangeCell(const stPoint& objPos, uint16& topCell, uint16& bottomCell, uint16& leftCell, uint16& rightCell);
void GridResetInterestSpace(stPoint beforePos, stObjectInfo* object, Grid* grid, bool crtFlag = true);
void ForwardMsgToNear(stObjectInfo* object, Grid* grid, RpcID msgID);
void ForwardCRTMsg(stObjectInfo* object, Grid* grid);
void ForwardDmgMsg(stObjectInfo* attacker, stObjectInfo* target, Grid* grid);
void ForwardDELMsg(stObjectInfo* object, Grid* grid);



//////////////////////////////
// ĳ���� ����
//////////////////////////////
stPoint GetRandomPosition();
void CreateFighter(HostID hostID);
void DeleteFighter(HostID hostID, bool netcoreSide = false);

//////////////////////////////
// ĳ���� �̵� �� ����
//////////////////////////////
void MoveFigter(HostID hostID, BYTE Direction, uint16_t X, uint16_t Y);
void StopFigther(HostID hostID, BYTE Direction, uint16_t X, uint16_t Y);

//////////////////////////////
// ĳ���� ����
//////////////////////////////
void AttackFighter(HostID hostID, BYTE Direction, uint16_t X, uint16_t Y, enAttackType atkType);

//////////////////////////////
// ����
//////////////////////////////
void ReceiveEcho(HostID hostID, uint32_t time);


//////////////////////////////
// Batch Process
//////////////////////////////
void BatchDeleteClientWork();
void AttackWork(HostID atkerID, HostID targetID, enAttackType atkType);
void BatchAttackWork();
void BatchMoveWork(uint16 calibration = 0);
void BatchTimeOutCheck();