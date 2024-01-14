#include "Proxy_FightGame.h"

namespace FightGameS2C {

	bool Proxy::CRT_CHARACTER(HostID remote, BYTE byCode, BYTE bySize, BYTE byType, uint32_t ID, BYTE Direction, uint16_t X, uint16_t Y, BYTE HP) {
		uint32_t msgLen = sizeof(byCode) + sizeof(bySize) + sizeof(byType) + sizeof(ID) + sizeof(Direction) + sizeof(X) + sizeof(Y) + sizeof(HP);
		JBuffer buff(msgLen);
		buff << byCode;
		buff << bySize;
		buff << byType;
		buff << ID;
		buff << Direction;
		buff << X;
		buff << Y;
		buff << HP;
		Send(remote, buff);

		//std::cout << "[CRT_CHARACTER] remote: " << remote << ", id: " << ID << ", dir: " << Direction << ", X: " << X << ", Y: " << Y << std::endl;

		return true;
	}
	bool Proxy::CRT_OTHER_CHARACTER(HostID remote, BYTE byCode, BYTE bySize, BYTE byType, uint32_t ID, BYTE Direction, uint16_t X, uint16_t Y, BYTE HP) {
		uint32_t msgLen = sizeof(byCode) + sizeof(bySize) + sizeof(byType) + sizeof(ID) + sizeof(Direction) + sizeof(X) + sizeof(Y) + sizeof(HP);
		JBuffer buff(msgLen);
		buff << byCode;
		buff << bySize;
		buff << byType;
		buff << ID;
		buff << Direction;
		buff << X;
		buff << Y;
		buff << HP;
		Send(remote, buff);

		//std::cout << "[CRT_OTHER_CHARACTER] remote: " << remote << ", id: " << ID << ", dir: " << Direction << ", X: " << X << ", Y: " << Y << std::endl;

		return true;
	}
	bool Proxy::DEL_CHARACTER(HostID remote, BYTE byCode, BYTE bySize, BYTE byType, uint32_t ID) {
		uint32_t msgLen = sizeof(byCode) + sizeof(bySize) + sizeof(byType) + sizeof(ID);
		JBuffer buff(msgLen);
		buff << byCode;
		buff << bySize;
		buff << byType;
		buff << ID;
		Send(remote, buff);

		//std::cout << "[CRT_DEL_CHARACTER] remote: " << remote << ", id: " << ID << std::endl;

		return true;
	}
	bool Proxy::MOVE_START(HostID remote, BYTE byCode, BYTE bySize, BYTE byType, uint32_t ID, BYTE Direction, uint16_t X, uint16_t Y) {
		uint32_t msgLen = sizeof(byCode) + sizeof(bySize) + sizeof(byType) + sizeof(ID) + sizeof(Direction) + sizeof(X) + sizeof(Y);
		JBuffer buff(msgLen);
		buff << byCode;
		buff << bySize;
		buff << byType;
		buff << ID;
		buff << Direction;
		buff << X;
		buff << Y;
		Send(remote, buff);

		//std::cout << "[MOVE_START] remote: " << remote << ", id: " << ID << ", dir: " << Direction << ", X: " << X << ", Y: " << Y << std::endl;

		return true;
	}
	bool Proxy::MOVE_STOP(HostID remote, BYTE byCode, BYTE bySize, BYTE byType, uint32_t ID, BYTE Direction, uint16_t X, uint16_t Y) {
		uint32_t msgLen = sizeof(byCode) + sizeof(bySize) + sizeof(byType) + sizeof(ID) + sizeof(Direction) + sizeof(X) + sizeof(Y);
		JBuffer buff(msgLen);
		buff << byCode;
		buff << bySize;
		buff << byType;
		buff << ID;
		buff << Direction;
		buff << X;
		buff << Y;
		Send(remote, buff);

		//std::cout << "[MOVE_STOP] remote: " << remote << ", id: " << ID << ", dir: " << Direction << ", X: " << X << ", Y: " << Y << std::endl;

		return true;
	}
	bool Proxy::ATTACK1(HostID remote, BYTE byCode, BYTE bySize, BYTE byType, uint32_t ID, BYTE Direction, uint16_t X, uint16_t Y) {
		uint32_t msgLen = sizeof(byCode) + sizeof(bySize) + sizeof(byType) + sizeof(ID) + sizeof(Direction) + sizeof(X) + sizeof(Y);
		JBuffer buff(msgLen);
		buff << byCode;
		buff << bySize;
		buff << byType;
		buff << ID;
		buff << Direction;
		buff << X;
		buff << Y;
		Send(remote, buff);

		return true;
	}
	bool Proxy::ATTACK2(HostID remote, BYTE byCode, BYTE bySize, BYTE byType, uint32_t ID, BYTE Direction, uint16_t X, uint16_t Y) {
		uint32_t msgLen = sizeof(byCode) + sizeof(bySize) + sizeof(byType) + sizeof(ID) + sizeof(Direction) + sizeof(X) + sizeof(Y);
		JBuffer buff(msgLen);
		buff << byCode;
		buff << bySize;
		buff << byType;
		buff << ID;
		buff << Direction;
		buff << X;
		buff << Y;
		Send(remote, buff);

		return true;
	}
	bool Proxy::ATTACK3(HostID remote, BYTE byCode, BYTE bySize, BYTE byType, uint32_t ID, BYTE Direction, uint16_t X, uint16_t Y) {
		uint32_t msgLen = sizeof(byCode) + sizeof(bySize) + sizeof(byType) + sizeof(ID) + sizeof(Direction) + sizeof(X) + sizeof(Y);
		JBuffer buff(msgLen);
		buff << byCode;
		buff << bySize;
		buff << byType;
		buff << ID;
		buff << Direction;
		buff << X;
		buff << Y;
		Send(remote, buff);

		return true;
	}
	bool Proxy::DAMAGE(HostID remote, BYTE byCode, BYTE bySize, BYTE byType, uint32_t attker, uint32_t target, BYTE targetHP) {
		uint32_t msgLen = sizeof(byCode) + sizeof(bySize) + sizeof(byType) + sizeof(attker) + sizeof(target) + sizeof(targetHP);
		JBuffer buff(msgLen);
		buff << byCode;
		buff << bySize;
		buff << byType;
		buff << attker;
		buff << target;
		buff << targetHP;
		Send(remote, buff);

		return true;
	}
	bool Proxy::SYNC(HostID remote, BYTE byCode, BYTE bySize, BYTE byType, uint32_t ID, uint16_t X, uint16_t Y) {
		uint32_t msgLen = sizeof(byCode) + sizeof(bySize) + sizeof(byType) + sizeof(ID) + sizeof(X) + sizeof(Y);
		JBuffer buff(msgLen);
		buff << byCode;
		buff << bySize;
		buff << byType;
		buff << ID;
		buff << X;
		buff << Y;
		Send(remote, buff);

		return true;
	}
	bool Proxy::ECHO(HostID remote, BYTE byCode, BYTE bySize, BYTE byType, uint32_t Time) {
		uint32_t msgLen = sizeof(byCode) + sizeof(bySize) + sizeof(byType) + sizeof(Time);
		JBuffer buff(msgLen);
		buff << byCode;
		buff << bySize;
		buff << byType;
		buff << Time;
		Send(remote, buff);

		return true;
	}
}

namespace FightGameC2S {

	bool Proxy::MOVE_START(HostID remote, BYTE byCode, BYTE bySize, BYTE byType, BYTE Direction, uint16_t X, uint16_t Y) {
		uint32_t msgLen = sizeof(byCode) + sizeof(bySize) + sizeof(byType) + sizeof(Direction) + sizeof(X) + sizeof(Y);
		JBuffer buff(msgLen);
		buff << byCode;
		buff << bySize;
		buff << byType;
		buff << Direction;
		buff << X;
		buff << Y;
		Send(remote, buff);

		return true;
	}
	bool Proxy::MOVE_STOP(HostID remote, BYTE byCode, BYTE bySize, BYTE byType, BYTE Direction, uint16_t X, uint16_t Y) {
		uint32_t msgLen = sizeof(byCode) + sizeof(bySize) + sizeof(byType) + sizeof(Direction) + sizeof(X) + sizeof(Y);
		JBuffer buff(msgLen);
		buff << byCode;
		buff << bySize;
		buff << byType;
		buff << Direction;
		buff << X;
		buff << Y;
		Send(remote, buff);

		return true;
	}
	bool Proxy::ATTACK1(HostID remote, BYTE byCode, BYTE bySize, BYTE byType, BYTE Direction, uint16_t X, uint16_t Y) {
		uint32_t msgLen = sizeof(byCode) + sizeof(bySize) + sizeof(byType) + sizeof(Direction) + sizeof(X) + sizeof(Y);
		JBuffer buff(msgLen);
		buff << byCode;
		buff << bySize;
		buff << byType;
		buff << Direction;
		buff << X;
		buff << Y;
		Send(remote, buff);

		return true;
	}
	bool Proxy::ATTACK2(HostID remote, BYTE byCode, BYTE bySize, BYTE byType, BYTE Direction, uint16_t X, uint16_t Y) {
		uint32_t msgLen = sizeof(byCode) + sizeof(bySize) + sizeof(byType) + sizeof(Direction) + sizeof(X) + sizeof(Y);
		JBuffer buff(msgLen);
		buff << byCode;
		buff << bySize;
		buff << byType;
		buff << Direction;
		buff << X;
		buff << Y;
		Send(remote, buff);

		return true;
	}
	bool Proxy::ATTACK3(HostID remote, BYTE byCode, BYTE bySize, BYTE byType, BYTE Direction, uint16_t X, uint16_t Y) {
		uint32_t msgLen = sizeof(byCode) + sizeof(bySize) + sizeof(byType) + sizeof(Direction) + sizeof(X) + sizeof(Y);
		JBuffer buff(msgLen);
		buff << byCode;
		buff << bySize;
		buff << byType;
		buff << Direction;
		buff << X;
		buff << Y;
		Send(remote, buff);

		return true;
	}
	bool Proxy::ECHO(HostID remote, BYTE byCode, BYTE bySize, BYTE byType, uint32_t Time) {
		uint32_t msgLen = sizeof(byCode) + sizeof(bySize) + sizeof(byType) + sizeof(Time);
		JBuffer buff(msgLen);
		buff << byCode;
		buff << bySize;
		buff << byType;
		buff << Time;
		Send(remote, buff);

		return true;
	}
}

