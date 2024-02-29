#include <conio.h>
#include "JNetServer.h"
#include "Configuration.h"

#include "Stub.h"
#include "EventHandler.h"
#include "RPC/Common_FightGame.cpp"
#include "RPC/Proxy_FightGame.cpp"

#pragma comment (lib, "winmm")

void ConsoleLog() {
	bool flag = false;
	for (int y = 0; y <= dfRANGE_MOVE_BOTTOM; y++) {
		for (int x = 0; x <= dfRANGE_MOVE_RIGHT; x++) {
			if (gClientGrid[y][x] != nullptr) {
				flag = true;
				stObjectInfo* objptr = gClientGrid[y][x];
				while (objptr != nullptr) {
					std::cout << "y: " << y << ", x: " << x << ", id: " << objptr->uiID << std::endl;
					objptr = objptr->nextGridObj;
				}
			}
		}
	}
	if (flag) {
		std::cout << "--------------------------------" << std::endl;
	}
}

class FighterGameBatch : public JNetBatchProcess
{
	void BatchProcess() override {
		// 전역 타이머 갱신
		gTime = time(NULL);

		//ConsoleLog();
		BatchAttackWork();
		BatchDeleteClientWork();
		BatchMoveWork();
	}

	void BatchProcess(uint16 g_LoopDelta) override {
#ifdef NON_SLEEP_LOOP
		if (g_LoopDelta <= 0) {
			return;
		}
#endif
		// 전역 타이머 갱신
		gTime = time(NULL);

		//ConsoleLog();
		BatchAttackWork();
		BatchDeleteClientWork();
		BatchMoveWork(g_LoopDelta);
	}
};

FighterGameBatch fightGameBatch;
FightGameS2C::Proxy g_Proxy;
Stub g_Stub;
EventHandler g_eventHnd;
JNetServer* g_JNetServer;

double g_LoopStart, g_LoopEnd, g_LoopDuration, g_OverTime;
uint16 g_LoopCnt, g_LoopDelta;
char g_CtrlChar;

void Init();
void Clear();

int main() {
	
	Init();

	while (true) {
		if (_kbhit()) {
			g_CtrlChar = _getch();
			if (g_CtrlChar == 'q' || g_CtrlChar == 'Q') {
				break;
			}
		}

#ifdef SLEEP_LOOP
		g_JNetServer->FrameMove(1 + g_LoopDelta);
		++g_LoopCnt;

		g_LoopEnd = clock();
		g_LoopDuration = (g_LoopEnd - g_LoopStart);
		g_LoopStart = clock();

		if (g_LoopDuration > SLEEP_TIME_MS) {
			//ERROR_EXCEPTION_WINDOW(L"Main", L"g_LoopDuration > SLEEP_TIME_MS");
			g_OverTime += (g_LoopDuration - SLEEP_TIME_MS);
			g_LoopDelta = g_OverTime / SLEEP_TIME_MS;

#if defined(CONSOLE_LOG)
			std::cout << "[프레임 초과]" << endl;
			std::cout << "g_LoopDelta: " << g_LoopDelta << std::endl;
			std::cout << "g_LoopCnt: " << g_LoopCnt << std::endl;
#endif 
			g_OverTime -= (g_LoopDelta * SLEEP_TIME_MS);
		}
		else {
			g_LoopDelta = 0;
			Sleep((double)SLEEP_TIME_MS - g_LoopDuration);
		}
#elif defined(NON_SLEEP_LOOP)
		g_JNetServer->FrameMove(g_LoopDelta);
		++g_LoopCnt;

		g_LoopEnd = clock();
		g_LoopDuration = (g_LoopEnd - g_LoopStart);
		g_LoopStart = clock();

#if defined(CONSOLE_LOG)
		if (g_LoopDuration > SLEEP_TIME_MS) {
			std::cout << "[프레임 초과]" << endl;
			std::cout << "g_LoopDelta: " << g_LoopDelta << std::endl;
			std::cout << "g_LoopCnt: " << g_LoopCnt << std::endl;
		}
#endif
		g_OverTime += g_LoopDuration;
		g_LoopDelta = g_OverTime / SLEEP_TIME_MS;
		g_OverTime -= g_LoopDelta * SLEEP_TIME_MS;
#endif // NON_SLEEP_LOOP

	}

	Clear();
}


void Init() {
	g_JNetServer = new JNetServer(false);

	g_JNetServer->AttachEventHandler(&g_eventHnd);
	g_JNetServer->AttachProxy(&g_Proxy, VALID_PACKET_NUM);
	g_JNetServer->AttachStub(&g_Stub, VALID_PACKET_NUM);
	g_JNetServer->AttachBatchProcess(&fightGameBatch);

	stServerStartParam startParam;
	//startParam.IP = "172.29.16.221";
	//startParam.Port = 20000;
	startParam.IP = SERVER_IP;
	startParam.Port = SERVER_PORT;

	g_JNetServer->Start(startParam);
	std::cout << "Server Start!" << std::endl;

	// 전역 타이머 작동 개시
	gTime = time(NULL);

	timeBeginPeriod(1);
	g_LoopStart = clock();
}

void Clear() {
	timeEndPeriod(1);
}