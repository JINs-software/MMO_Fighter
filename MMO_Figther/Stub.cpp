#include "Stub.h"
#include "RPC/Stub_FightGame.cpp"

// ƒ¡≈Ÿ√˜ ƒ⁄µÂ ¡÷¿‘
JPDEF_FightGameC2S_MOVE_START(Stub) {
	MoveFigter(remote, Direction, X, Y);
	return true;
}
JPDEF_FightGameC2S_MOVE_STOP(Stub) {
	StopFigther(remote, Direction, X, Y);
	return true;
}
JPDEF_FightGameC2S_ATTACK1(Stub) {
	AttackFighter(remote, Direction, X, Y, enAttackType::ATTACK1);
	return true;
}
JPDEF_FightGameC2S_ATTACK2(Stub) {
	AttackFighter(remote, Direction, X, Y, enAttackType::ATTACK2);
	return true;
}
JPDEF_FightGameC2S_ATTACK3(Stub) {
	AttackFighter(remote, Direction, X, Y, enAttackType::ATTACK3);
	return true;
}
JPDEF_FightGameC2S_ECHO(Stub) {
	ReceiveEcho(remote, Time);
	return true;
}
