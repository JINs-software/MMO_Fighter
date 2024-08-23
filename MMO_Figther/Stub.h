#pragma once
#include "JNetCoreServer.h"
//#include "RPC/Stub_FightGame.h"
//
//class Stub : public FightGameC2S::Stub {
//	JPDEC_FightGameC2S_MOVE_START;
//	JPDEC_FightGameC2S_MOVE_STOP;
//	JPDEC_FightGameC2S_ATTACK1;
//	JPDEC_FightGameC2S_ATTACK2;
//	JPDEC_FightGameC2S_ATTACK3;
//	JPDEC_FightGameC2S_ECHO;
//};


#include "RPC/Stub_FightGameCrtDel.h"
#include "RPC/Stub_FightGameMove.h"
#include "RPC/Stub_FightGameAttack.h"
#include "RPC/Stub_FightGameDamage.h"
#include "RPC/Stub_FightGameComm.h"

class Stub : public FightGameMove_C2S::Stub, public FightGameAttack_C2S::Stub, public FightGameComm_C2S::Stub {
	JPDEC_FightGameMove_C2S_MOVE_START;
	JPDEC_FightGameMove_C2S_MOVE_STOP;

	JPDEC_FightGameAttack_C2S_ATTACK1;
	JPDEC_FightGameAttack_C2S_ATTACK2;
	JPDEC_FightGameAttack_C2S_ATTACK3;

	JPDEC_FightGameComm_C2S_ECHO;
};
