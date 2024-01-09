#include "Stub.h"
#include "RPC/Stub_FightGame.cpp"

// даеыцВ дз╣Е ажют
JPDEF_FightGameC2S_MOVE_START(Stub) {
	//std::cout << "recv() MOVE_START msg from " << remote << std::endl;
	MoveFigter(remote, Direction, X, Y);
	return true;
}
JPDEF_FightGameC2S_MOVE_STOP(Stub) {
	//std::cout << "recv() MOVE_STOP msg from " << remote << std::endl;
	StopFigther(remote, Direction, X, Y);
	return true;
}
JPDEF_FightGameC2S_ATTACK1(Stub) {
	//std::cout << "recv() ATTACK1 msg from " << remote << std::endl;
	AttackFighter(remote, Direction, X, Y, enAttackType::ATTACK1);
	return true;
}
JPDEF_FightGameC2S_ATTACK2(Stub) {
	//std::cout << "recv() ATTACK2 msg from " << remote << std::endl;
	AttackFighter(remote, Direction, X, Y, enAttackType::ATTACK2);
	return true;
}
JPDEF_FightGameC2S_ATTACK3(Stub) {
	//std::cout << "recv() ATTACK3 msg from " << remote << std::endl;
	AttackFighter(remote, Direction, X, Y, enAttackType::ATTACK3);
	return true;
}
JPDEF_FightGameC2S_ECHO(Stub) {
	//std::cout << "recv() ECHO msg from " << remote << std::endl;
	ReceiveEcho(remote, Time);
	return true;
}
