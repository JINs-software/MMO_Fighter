#include "Stub.h"
//#include "RPC/Stub_FightGame.cpp"

#include "RPC/Stub_FightGameCrtDel.cpp"
#include "RPC/Stub_FightGameMove.cpp"
#include "RPC/Stub_FightGameAttack.cpp"
#include "RPC/Stub_FightGameDamage.cpp"

#include "Contents.h"

// ƒ¡≈Ÿ√˜ ƒ⁄µÂ ¡÷¿‘
JPDEF_FightGameMove_C2S_MOVE_START(Stub) {
	MoveFigter(remote, Dir, X, Y);
	return true;
}
JPDEF_FightGameMove_C2S_MOVE_STOP(Stub) {
	StopFigther(remote, Dir, X, Y);
	return true;
}
JPDEF_FightGameAttack_C2S_ATTACK1(Stub) {
	AttackFighter(remote, Dir, X, Y, enAttackType::ATTACK1);
	return true;
}
JPDEF_FightGameAttack_C2S_ATTACK2(Stub) {
	AttackFighter(remote, Dir, X, Y, enAttackType::ATTACK2);
	return true;
}
JPDEF_FightGameAttack_C2S_ATTACK3(Stub) {
	AttackFighter(remote, Dir, X, Y, enAttackType::ATTACK3);
	return true;
}
JPDEF_FightGameComm_C2S_ECHO(Stub) {
	ReceiveEcho(remote, Time);
	return true;
}
