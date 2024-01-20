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

	void BatchProcess(uint16 loopDelta) override {
#ifdef NON_SLEEP_LOOP
		if (loopDelta <= 0) {
			return;
		}
#endif
		// 전역 타이머 갱신
		gTime = time(NULL);

		//ConsoleLog();
		BatchAttackWork();
		BatchDeleteClientWork();
		BatchMoveWork(loopDelta);
	}
};
FighterGameBatch fightGameBatch;
FightGameS2C::Proxy g_Proxy;
Stub g_Stub;
EventHandler g_eventHnd;

int main() {
	JNetServer* jnetServer = new JNetServer(false);

	jnetServer->AttachEventHandler(&g_eventHnd);
	jnetServer->AttachProxy(&g_Proxy, VALID_PACKET_NUM);
	jnetServer->AttachStub(&g_Stub, VALID_PACKET_NUM);
	jnetServer->AttachBatchProcess(&fightGameBatch);

	stServerStartParam startParam;
	startParam.IP = "172.30.1.100";
	startParam.Port = 20000;

	jnetServer->Start(startParam);
	std::cout << "Server Start!" << std::endl;

	// 전역 타이머 작동 개시
	gTime = time(NULL);

	timeBeginPeriod(1);
	double loopStart = clock();
	double loopEnd;
	double loopDuration;
	uint16 loopCnt = 0;
	uint16 loopDelta = 0;
	double overTime = 0;
	char ctrInput;
	while (true) {
		//if (_kbhit()) {
		//	ctrInput = _getch();
		//	if (ctrInput == 'q' || ctrInput == 'Q') {
		//		break;
		//	}
		//}

#ifdef SLEEP_LOOP
		jnetServer->FrameMove(1 + loopDelta);
		++loopCnt;

		loopEnd = clock();
		loopDuration = (loopEnd - loopStart);

		if (loopDuration > SLEEP_TIME_MS) {
			//ERROR_EXCEPTION_WINDOW(L"Main", L"loopDuration > SLEEP_TIME_MS");
			overTime += (loopDuration - SLEEP_TIME_MS);
			loopDelta = overTime / SLEEP_TIME_MS;

			std::cout << "[프레임 초과]" << endl;
			std::cout << "loopDelta: " << loopDelta << std::endl;
			std::cout << "loopCnt: " << loopCnt << std::endl;

			overTime -= (loopDelta * SLEEP_TIME_MS);
		}
		else {
			loopDelta = 0;
			Sleep((double)SLEEP_TIME_MS - loopDuration);
		}

		loopStart = clock();
#endif // SLEEP_LOOP
#ifdef NON_SLEEP_LOOP
		jnetServer->FrameMove(loopDelta);
		++loopCnt;

		loopEnd = clock();
		loopDuration = (loopEnd - loopStart);
		loopStart = clock();

		if (loopDuration > SLEEP_TIME_MS) {
			std::cout << "[프레임 초과]" << endl;
			std::cout << "loopDelta: " << loopDelta << std::endl;
			std::cout << "loopCnt: " << loopCnt << std::endl;
		}

		overTime += loopDuration;
		loopDelta = overTime / SLEEP_TIME_MS;
		overTime -= loopDelta * SLEEP_TIME_MS;

		//loopStart = clock();
#endif // NON_SLEEP_LOOP

	}
	timeEndPeriod(1);
}