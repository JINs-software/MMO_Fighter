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

extern std::map<HostID, stObjectInfo*> gClientMap;
//extern std::vector<std::vector<std::set<HostID>>> gClientGrid;
extern std::vector<std::vector<stObjectInfo*>> gClientGrid;
extern std::set<HostID> gDeleteClientSet;
extern std::queue<stAttackWork> AtkWorkQueue;


//////////////////////////////
// ĳ���� ����
//////////////////////////////
stPoint GetRandomPosition();
void CreateFighter(HostID hostID);
void DeleteFighter(HostID hostID);

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
void BatchMoveWork();