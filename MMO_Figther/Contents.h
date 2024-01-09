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
// 캐릭터 생성
//////////////////////////////
stPoint GetRandomPosition();
void CreateFighter(HostID hostID);
void DeleteFighter(HostID hostID);

//////////////////////////////
// 캐릭터 이동 및 중지
//////////////////////////////
void MoveFigter(HostID hostID, BYTE Direction, uint16_t X, uint16_t Y);
void StopFigther(HostID hostID, BYTE Direction, uint16_t X, uint16_t Y);

//////////////////////////////
// 캐릭터 공격
//////////////////////////////
void AttackFighter(HostID hostID, BYTE Direction, uint16_t X, uint16_t Y, enAttackType atkType);

//////////////////////////////
// 에코
//////////////////////////////
void ReceiveEcho(HostID hostID, uint32_t time);


//////////////////////////////
// Batch Process
//////////////////////////////
void BatchDeleteClientWork();
void AttackWork(HostID atkerID, HostID targetID, enAttackType atkType);
void BatchAttackWork();
void BatchMoveWork();