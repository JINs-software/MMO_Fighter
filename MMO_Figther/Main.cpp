#include <conio.h>
#include "JNetCoreServer.h"
#include "EventHandler.h"
#include "Configuration.h"

//#include "RPC/Common_FightGame.cpp"
//#include "RPC/Proxy_FightGame.cpp"
#include "RPC/Common_FightGameCrtDel.cpp"
#include "RPC/Common_FightGameMove.cpp"
#include "RPC/Common_FightGameAttack.cpp"
#include "RPC/Common_FightGameDamage.cpp"
#include "RPC/Proxy_FightGameCrtDel.cpp"
#include "RPC/Proxy_FightGameMove.cpp"
#include "RPC/Proxy_FightGameAttack.cpp"
#include "RPC/Proxy_FightGameDamage.cpp"
#include "RPC/Proxy_FightGameComm.cpp"

#include "Stub.h"

#pragma comment (lib, "winmm")

class FighterGameBatch : public JNetBatchProcess
{
	void BatchProcess(uint16 g_LoopDelta) override {
#ifdef NON_SLEEP_LOOP
		if (g_LoopDelta <= 0) {
			return;
		}
#endif
		// 로직 수행 중 proxy send 시 존재하지 않은 remote(코어에서 이미 삭제 처리를 한) 세션에 전송을 막음
		BatchDeleteClientWork();

		BatchSyncLog();
		BatchTimeOutCheck();
		BatchAttackWork();
		//BatchDeleteClientWork();
		BatchMoveWork(g_LoopDelta);
#if defined(CONSOLE_LOG)
		BatchPrintLog();
#endif
	}
};

FighterGameBatch fightGameBatch;
//FightGameS2C::Proxy g_Proxy;
FightGameCrtDel_S2C::Proxy g_ProxyCrtDel;
FightGameMove_S2C::Proxy g_ProxyMove;
FightGameAttack_S2C::Proxy g_ProxyAttack;
FightGameDamage_S2C::Proxy g_ProxyDamage;
FightGameComm_S2C::Proxy g_ProxyComm;
Stub g_Stub;
EventHandler g_eventHnd;
JNetCoreServer* g_JNetServer;

void Init();

int main() {
	
	Init();
	g_JNetServer->Start(SLEEP_TIME_MS);
	std::cout << "Server Start!" << std::endl;
}


void Init() {
	g_JNetServer = new JNetCoreServer(true);

	g_JNetServer->AttachEventHandler(&g_eventHnd);
	g_JNetServer->AttachProxy(&g_ProxyCrtDel, VALID_PACKET_NUM);
	g_JNetServer->AttachProxy(&g_ProxyMove, VALID_PACKET_NUM);
	g_JNetServer->AttachProxy(&g_ProxyAttack, VALID_PACKET_NUM);
	g_JNetServer->AttachProxy(&g_ProxyDamage, VALID_PACKET_NUM);
	g_JNetServer->AttachStub(&g_Stub, VALID_PACKET_NUM);
	g_JNetServer->AttachBatchProcess(&fightGameBatch);

	g_JNetServer->Init(SERVER_PORT);

	// 전역 타이머 작동 개시
	g_Time = time(NULL);
}
