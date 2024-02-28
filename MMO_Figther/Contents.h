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
// 무식한 방식의 공간 분할
//-------------------------
void InsertToGrid(uint16_t Y, uint16_t X, HostID id);
void DeleteFromGrid(uint16_t Y, uint16_t X, HostID id);
// 캐릭터 생성, 삭제 그리고 MOVE/ATTACK 시 주변 공간의 플레이어에 메시지 포워딩
void FowardCRTMsg(stObjectInfo* newObject);
void ForwardDmgMsg(stObjectInfo* attacker, stObjectInfo* target);
void ForwardMsgToNear(stObjectInfo* player, RpcID msgID);
//-------------------------
// 캐릭터 이동 중간 중간 관심 영역 변경
// 새로운 관심 영역에는 CRT/MOVE 메시지 전송
// 관심 대상에서 제외된 영역에는 DEL 메시지 전송
void FowardCrtDelMsgByMove(stObjectInfo* player, const stPoint& beforePos);
//=========================

//=========================
// 고정 크기 격자 공간 분할
//-------------------------
// 고정 크기 격자 공간 관리
void GetRangeCell(const stPoint& objPos, uint16& topCell, uint16& bottomCell, uint16& leftCell, uint16& rightCell);
void GridResetInterestSpace(stPoint beforePos, stObjectInfo* object, Grid* grid, bool crtFlag = true);
void ForwardMsgToNear(stObjectInfo* object, Grid* grid, RpcID msgID);
void ForwardCRTMsg(stObjectInfo* object, Grid* grid);
void ForwardDmgMsg(stObjectInfo* attacker, stObjectInfo* target, Grid* grid);
void ForwardDELMsg(stObjectInfo* object, Grid* grid);



//////////////////////////////
// 캐릭터 생성
//////////////////////////////
stPoint GetRandomPosition();
void CreateFighter(HostID hostID);
void DeleteFighter(HostID hostID, bool netcoreSide = false);

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
void BatchMoveWork(uint16 calibration = 0);
void BatchTimeOutCheck();