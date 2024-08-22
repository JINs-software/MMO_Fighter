#pragma once
#include "JNetCoreServer.h"
#include "RPC/Stub_FightGame.h"
#include "Contents.h"

class Stub : public FightGameC2S::Stub {
	JPDEC_FightGameC2S_MOVE_START;
	JPDEC_FightGameC2S_MOVE_STOP;
	JPDEC_FightGameC2S_ATTACK1;
	JPDEC_FightGameC2S_ATTACK2;
	JPDEC_FightGameC2S_ATTACK3;
	JPDEC_FightGameC2S_ECHO;
};