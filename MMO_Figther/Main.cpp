#include "JNetServer.h"
#include "Configuration.h"

#include "Stub.h"
#include "EventHandler.h"
#include "RPC/Common_FightGame.cpp"
#include "RPC/Proxy_FightGame.cpp"

#pragma comment (lib, "winmm")

#define SLEEP_TIME_MS 20

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
		//ConsoleLog();
		BatchAttackWork();
		BatchDeleteClientWork();
		BatchMoveWork();
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
	startParam.IP = "127.0.0.1";
	startParam.Port = 20000;

	jnetServer->Start(startParam);
	std::cout << "Server Start!" << std::endl;


	timeBeginPeriod(1);
	double loopStart = clock();
	while (true) {
		jnetServer->FrameMove();

		double loopEnd = clock();
		double loopDuration = (loopEnd - loopStart);
		if (loopDuration > SLEEP_TIME_MS) { 
			//ERROR_EXCEPTION_WINDOW(L"Main", L"loopDuration > SLEEP_TIME_MS");
			std::cout << "프레임 초과" << endl;
		}
		else {
			Sleep((double)SLEEP_TIME_MS - loopDuration);
		}
		loopStart = clock();
	}
	timeEndPeriod(1);
}