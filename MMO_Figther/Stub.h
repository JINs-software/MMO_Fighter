#pragma once
#include "JNetCoreServer.h"

#include "RPC/Stub_FightGameCrtDel.h"
#include "RPC/Stub_FightGameMove.h"
#include "RPC/Stub_FightGameAttack.h"
#include "RPC/Stub_FightGameDamage.h"
#include "RPC/Stub_FightGameComm.h"

class StubMove : public FightGameMove_C2S::Stub {
	JPDEC_FightGameMove_C2S_MOVE_START;
	JPDEC_FightGameMove_C2S_MOVE_STOP;
};

class StubAttack : public FightGameAttack_C2S::Stub {
	JPDEC_FightGameAttack_C2S_ATTACK1;
	JPDEC_FightGameAttack_C2S_ATTACK2;
	JPDEC_FightGameAttack_C2S_ATTACK3;
};

class StubComm : public FightGameComm_C2S::Stub {
	JPDEC_FightGameComm_C2S_ECHO;
};


