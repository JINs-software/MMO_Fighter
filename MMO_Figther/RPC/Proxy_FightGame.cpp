#include "Proxy_FightGame.h"

namespace FightGameS2C {

	bool Proxy::CRT_CHARACTER(HostID remote, BYTE byCode, BYTE bySize, BYTE byType, uint32_t ID, BYTE Direction, uint16_t X, uint16_t Y, BYTE HP) {
		uint32_t msgLen = sizeof(byCode) + sizeof(bySize) + sizeof(byType) + sizeof(ID) + sizeof(Direction) + sizeof(X) + sizeof(Y) + sizeof(HP);
#ifdef DIRECT_ACCESS_TO_JNETSESSION
		stJNetSession* jnetSession = GetJNetSession(remote);
		if (jnetSession != nullptr) {
			JBuffer& buff = (*jnetSession->sendBuff);
			if (buff.GetFreeSize() >= msgLen) {
				buff << byCode;
				buff << bySize;
				buff << byType;
				buff << ID;
				buff << Direction;
				buff << X;
				buff << Y;
				buff << HP;
			}
			else {
				std::cout << "!------------------------------!" << std::endl;
				std::cout << "!- buff.GetFreeSize() < msgLen-!" << std::endl;
				std::cout << "!------------------------------!" << std::endl;
				return false;
			}
		}
		else {
			//std::cout << "!------------------------------!" << std::endl;
			//std::cout << "!--GetJNetSession return NULL--!" << std::endl;
			//std::cout << "!------------------------------!" << std::endl;
			return false;
		}
#else
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
#endif // DIRECT_ACCESS_TO_JNETSESSION
		//std::cout << "[CRT_CHARACTER] remote: " << remote << ", id: " << ID << ", dir: " << Direction << ", X: " << X << ", Y: " << Y << std::endl;

		return true;
	}
	bool Proxy::CRT_OTHER_CHARACTER(HostID remote, BYTE byCode, BYTE bySize, BYTE byType, uint32_t ID, BYTE Direction, uint16_t X, uint16_t Y, BYTE HP) {
		uint32_t msgLen = sizeof(byCode) + sizeof(bySize) + sizeof(byType) + sizeof(ID) + sizeof(Direction) + sizeof(X) + sizeof(Y) + sizeof(HP);
#ifdef DIRECT_ACCESS_TO_JNETSESSION
		stJNetSession* jnetSession = GetJNetSession(remote);
		if (jnetSession != nullptr) {
			JBuffer& buff = (*jnetSession->sendBuff);
			if (buff.GetFreeSize() >= msgLen) {
				buff << byCode;
				buff << bySize;
				buff << byType;
				buff << ID;
				buff << Direction;
				buff << X;
				buff << Y;
				buff << HP;
			}
			else {
				std::cout << "!------------------------------!" << std::endl;
				std::cout << "!- buff.GetFreeSize() < msgLen-!" << std::endl;
				std::cout << "!------------------------------!" << std::endl;
				return false;
			}
		}
		else {
			//std::cout << "!------------------------------!" << std::endl;
			//std::cout << "!--GetJNetSession return NULL--!" << std::endl;
			//std::cout << "!------------------------------!" << std::endl;
			return false;
		}
#else
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
#endif
		//std::cout << "[CRT_OTHER_CHARACTER] remote: " << remote << ", id: " << ID << ", dir: " << Direction << ", X: " << X << ", Y: " << Y << std::endl;

		return true;
	}
	bool Proxy::DEL_CHARACTER(HostID remote, BYTE byCode, BYTE bySize, BYTE byType, uint32_t ID) {
		uint32_t msgLen = sizeof(byCode) + sizeof(bySize) + sizeof(byType) + sizeof(ID);
#ifdef DIRECT_ACCESS_TO_JNETSESSION
		stJNetSession* jnetSession = GetJNetSession(remote);
		if (jnetSession != nullptr) {
			JBuffer& buff = (*jnetSession->sendBuff);
			if (buff.GetFreeSize() >= msgLen) {
				buff << byCode;
				buff << bySize;
				buff << byType;
				buff << ID;
			}
			else {
				std::cout << "!------------------------------!" << std::endl;
				std::cout << "!- buff.GetFreeSize() < msgLen-!" << std::endl;
				std::cout << "!------------------------------!" << std::endl;
				return false;
			}
		}
		else {
			//std::cout << "!------------------------------!" << std::endl;
			//std::cout << "!--GetJNetSession return NULL--!" << std::endl;
			//std::cout << "!------------------------------!" << std::endl;
			return false;
		}
#else
		JBuffer buff(msgLen);
		buff << byCode;
		buff << bySize;
		buff << byType;
		buff << ID;
		Send(remote, buff);
#endif
		//std::cout << "[CRT_DEL_CHARACTER] remote: " << remote << ", id: " << ID << std::endl;

		return true;
	}
	bool Proxy::MOVE_START(HostID remote, BYTE byCode, BYTE bySize, BYTE byType, uint32_t ID, BYTE Direction, uint16_t X, uint16_t Y) {
		uint32_t msgLen = sizeof(byCode) + sizeof(bySize) + sizeof(byType) + sizeof(ID) + sizeof(Direction) + sizeof(X) + sizeof(Y);
#ifdef DIRECT_ACCESS_TO_JNETSESSION
		stJNetSession* jnetSession = GetJNetSession(remote);
		if (jnetSession != nullptr) {
			JBuffer& buff = (*jnetSession->sendBuff);
			if (buff.GetFreeSize() >= msgLen) {
				buff << byCode;
				buff << bySize;
				buff << byType;
				buff << ID;
				buff << Direction;
				buff << X;
				buff << Y;
			}
			else {
				std::cout << "!------------------------------!" << std::endl;
				std::cout << "!- buff.GetFreeSize() < msgLen-!" << std::endl;
				std::cout << "!------------------------------!" << std::endl;
				return false;
			}
		}
		else {
			//std::cout << "!------------------------------!" << std::endl;
			//std::cout << "!--GetJNetSession return NULL--!" << std::endl;
			//std::cout << "!------------------------------!" << std::endl;
			return false;
		}
#else
		JBuffer buff(msgLen);
		buff << byCode;
		buff << bySize;
		buff << byType;
		buff << ID;
		buff << Direction;
		buff << X;
		buff << Y;
		Send(remote, buff);
#endif
		//std::cout << "[MOVE_START] remote: " << remote << ", id: " << ID << ", dir: " << Direction << ", X: " << X << ", Y: " << Y << std::endl;

		return true;
	}
	bool Proxy::MOVE_STOP(HostID remote, BYTE byCode, BYTE bySize, BYTE byType, uint32_t ID, BYTE Direction, uint16_t X, uint16_t Y) {
		uint32_t msgLen = sizeof(byCode) + sizeof(bySize) + sizeof(byType) + sizeof(ID) + sizeof(Direction) + sizeof(X) + sizeof(Y);
#ifdef DIRECT_ACCESS_TO_JNETSESSION
		stJNetSession* jnetSession = GetJNetSession(remote);
		if (jnetSession != nullptr) {
			JBuffer& buff = (*jnetSession->sendBuff);
			if (buff.GetFreeSize() >= msgLen) {
				buff << byCode;
				buff << bySize;
				buff << byType;
				buff << ID;
				buff << Direction;
				buff << X;
				buff << Y;
			}
			else {
				std::cout << "!------------------------------!" << std::endl;
				std::cout << "!- buff.GetFreeSize() < msgLen-!" << std::endl;
				std::cout << "!------------------------------!" << std::endl;
				return false;
			}
		}
		else {
			//std::cout << "!------------------------------!" << std::endl;
			//std::cout << "!--GetJNetSession return NULL--!" << std::endl;
			//std::cout << "!------------------------------!" << std::endl;
			return false;
		}
#else
		JBuffer buff(msgLen);
		buff << byCode;
		buff << bySize;
		buff << byType;
		buff << ID;
		buff << Direction;
		buff << X;
		buff << Y;
		Send(remote, buff);
#endif
		//std::cout << "[MOVE_STOP] remote: " << remote << ", id: " << ID << ", dir: " << Direction << ", X: " << X << ", Y: " << Y << std::endl;

		return true;
	}
	bool Proxy::ATTACK1(HostID remote, BYTE byCode, BYTE bySize, BYTE byType, uint32_t ID, BYTE Direction, uint16_t X, uint16_t Y) {
		uint32_t msgLen = sizeof(byCode) + sizeof(bySize) + sizeof(byType) + sizeof(ID) + sizeof(Direction) + sizeof(X) + sizeof(Y);
#ifdef DIRECT_ACCESS_TO_JNETSESSION
		stJNetSession* jnetSession = GetJNetSession(remote);
		if (jnetSession != nullptr) {
			JBuffer& buff = (*jnetSession->sendBuff);
			if (buff.GetFreeSize() >= msgLen) {
				buff << byCode;
				buff << bySize;
				buff << byType;
				buff << ID;
				buff << Direction;
				buff << X;
				buff << Y;
			}
			else {
				std::cout << "!------------------------------!" << std::endl;
				std::cout << "!- buff.GetFreeSize() < msgLen-!" << std::endl;
				std::cout << "!------------------------------!" << std::endl;
				return false;
			}
		}
		else {
			//std::cout << "!------------------------------!" << std::endl;
			//std::cout << "!--GetJNetSession return NULL--!" << std::endl;
			//std::cout << "!------------------------------!" << std::endl;
			return false;
		}
#else		
		JBuffer buff(msgLen);
		buff << byCode;
		buff << bySize;
		buff << byType;
		buff << ID;
		buff << Direction;
		buff << X;
		buff << Y;
		Send(remote, buff);
#endif
		return true;
	}
	bool Proxy::ATTACK2(HostID remote, BYTE byCode, BYTE bySize, BYTE byType, uint32_t ID, BYTE Direction, uint16_t X, uint16_t Y) {
		uint32_t msgLen = sizeof(byCode) + sizeof(bySize) + sizeof(byType) + sizeof(ID) + sizeof(Direction) + sizeof(X) + sizeof(Y);
#ifdef DIRECT_ACCESS_TO_JNETSESSION
		stJNetSession* jnetSession = GetJNetSession(remote);
		if (jnetSession != nullptr) {
			JBuffer& buff = (*jnetSession->sendBuff);
			if (buff.GetFreeSize() >= msgLen) {
				buff << byCode;
				buff << bySize;
				buff << byType;
				buff << ID;
				buff << Direction;
				buff << X;
				buff << Y;
			}
			else {
				std::cout << "!------------------------------!" << std::endl;
				std::cout << "!- buff.GetFreeSize() < msgLen-!" << std::endl;
				std::cout << "!------------------------------!" << std::endl;
				return false;
			}
		}
		else {
			//std::cout << "!------------------------------!" << std::endl;
			//std::cout << "!--GetJNetSession return NULL--!" << std::endl;
			//std::cout << "!------------------------------!" << std::endl;
			return false;
		}
#else
		JBuffer buff(msgLen);
		buff << byCode;
		buff << bySize;
		buff << byType;
		buff << ID;
		buff << Direction;
		buff << X;
		buff << Y;
		Send(remote, buff);
#endif
		return true;
	}
	bool Proxy::ATTACK3(HostID remote, BYTE byCode, BYTE bySize, BYTE byType, uint32_t ID, BYTE Direction, uint16_t X, uint16_t Y) {
		uint32_t msgLen = sizeof(byCode) + sizeof(bySize) + sizeof(byType) + sizeof(ID) + sizeof(Direction) + sizeof(X) + sizeof(Y);
#ifdef DIRECT_ACCESS_TO_JNETSESSION
		stJNetSession* jnetSession = GetJNetSession(remote);
		if (jnetSession != nullptr) {
			JBuffer& buff = (*jnetSession->sendBuff);
			if (buff.GetFreeSize() >= msgLen) {
				buff << byCode;
				buff << bySize;
				buff << byType;
				buff << ID;
				buff << Direction;
				buff << X;
				buff << Y;
			}
			else {
				std::cout << "!------------------------------!" << std::endl;
				std::cout << "!- buff.GetFreeSize() < msgLen-!" << std::endl;
				std::cout << "!------------------------------!" << std::endl;
				return false;
			}
		}
		else {
			//std::cout << "!------------------------------!" << std::endl;
			//std::cout << "!--GetJNetSession return NULL--!" << std::endl;
			//std::cout << "!------------------------------!" << std::endl;
			return false;
		}
#else
		JBuffer buff(msgLen);
		buff << byCode;
		buff << bySize;
		buff << byType;
		buff << ID;
		buff << Direction;
		buff << X;
		buff << Y;
		Send(remote, buff);
#endif
		return true;
	}
	bool Proxy::DAMAGE(HostID remote, BYTE byCode, BYTE bySize, BYTE byType, uint32_t attker, uint32_t target, BYTE targetHP) {
		uint32_t msgLen = sizeof(byCode) + sizeof(bySize) + sizeof(byType) + sizeof(attker) + sizeof(target) + sizeof(targetHP);
#ifdef DIRECT_ACCESS_TO_JNETSESSION
		stJNetSession* jnetSession = GetJNetSession(remote);
		if (jnetSession != nullptr) {
			JBuffer& buff = (*jnetSession->sendBuff);
			if (buff.GetFreeSize() >= msgLen) {
				buff << byCode;
				buff << bySize;
				buff << byType;
				buff << attker;
				buff << target;
				buff << targetHP;
			}
			else {
				std::cout << "!------------------------------!" << std::endl;
				std::cout << "!- buff.GetFreeSize() < msgLen-!" << std::endl;
				std::cout << "!------------------------------!" << std::endl;
				return false;
			}
		}
		else {
			//std::cout << "!------------------------------!" << std::endl;
			//std::cout << "!--GetJNetSession return NULL--!" << std::endl;
			//std::cout << "!------------------------------!" << std::endl;
			return false;
		}
#else
		JBuffer buff(msgLen);
		buff << byCode;
		buff << bySize;
		buff << byType;
		buff << attker;
		buff << target;
		buff << targetHP;
		Send(remote, buff);
#endif
		return true;
	}
	bool Proxy::SYNC(HostID remote, BYTE byCode, BYTE bySize, BYTE byType, uint32_t ID, uint16_t X, uint16_t Y) {
		uint32_t msgLen = sizeof(byCode) + sizeof(bySize) + sizeof(byType) + sizeof(ID) + sizeof(X) + sizeof(Y);
#ifdef DIRECT_ACCESS_TO_JNETSESSION
		stJNetSession* jnetSession = GetJNetSession(remote);
		if (jnetSession != nullptr) {
			JBuffer& buff = (*jnetSession->sendBuff);
			if (buff.GetFreeSize() >= msgLen) {
				buff << byCode;
				buff << bySize;
				buff << byType;
				buff << ID;
				buff << X;
				buff << Y;
			}
			else {
				std::cout << "!------------------------------!" << std::endl;
				std::cout << "!- buff.GetFreeSize() < msgLen-!" << std::endl;
				std::cout << "!------------------------------!" << std::endl;
				return false;
			}
		}
		else {
			//std::cout << "!------------------------------!" << std::endl;
			//std::cout << "!--GetJNetSession return NULL--!" << std::endl;
			//std::cout << "!------------------------------!" << std::endl;
			return false;
		}
#else
		JBuffer buff(msgLen);
		buff << byCode;
		buff << bySize;
		buff << byType;
		buff << ID;
		buff << X;
		buff << Y;
		Send(remote, buff);
#endif
		return true;
	}
	bool Proxy::ECHO(HostID remote, BYTE byCode, BYTE bySize, BYTE byType, uint32_t Time) {
		uint32_t msgLen = sizeof(byCode) + sizeof(bySize) + sizeof(byType) + sizeof(Time);
#ifdef DIRECT_ACCESS_TO_JNETSESSION
		stJNetSession* jnetSession = GetJNetSession(remote);
		if (jnetSession != nullptr) {
			JBuffer& buff = (*jnetSession->sendBuff);
			if (buff.GetFreeSize() >= msgLen) {
				buff << byCode;
				buff << bySize;
				buff << byType;
				buff << Time;
			}
			else {
				std::cout << "!------------------------------!" << std::endl;
				std::cout << "!- buff.GetFreeSize() < msgLen-!" << std::endl;
				std::cout << "!------------------------------!" << std::endl;
				return false;
			}
		}
		else {
			//std::cout << "!------------------------------!" << std::endl;
			//std::cout << "!--GetJNetSession return NULL--!" << std::endl;
			//std::cout << "!------------------------------!" << std::endl;
			return false;
		}
#else
		JBuffer buff(msgLen);
		buff << byCode;
		buff << bySize;
		buff << byType;
		buff << Time;
		Send(remote, buff);
#endif
		return true;
	}
}

namespace FightGameC2S {

	bool Proxy::MOVE_START(HostID remote, BYTE byCode, BYTE bySize, BYTE byType, BYTE Direction, uint16_t X, uint16_t Y) {
		uint32_t msgLen = sizeof(byCode) + sizeof(bySize) + sizeof(byType) + sizeof(Direction) + sizeof(X) + sizeof(Y);
#ifdef DIRECT_ACCESS_TO_JNETSESSION
		stJNetSession* jnetSession = GetJNetSession(remote);
		if (jnetSession != nullptr) {
			JBuffer& buff = (*jnetSession->sendBuff);
			if (buff.GetFreeSize() >= msgLen) {
				buff << byCode;
				buff << bySize;
				buff << byType;
				buff << Direction;
				buff << X;
				buff << Y;
			}
			else {
				std::cout << "!------------------------------!" << std::endl;
				std::cout << "!- buff.GetFreeSize() < msgLen-!" << std::endl;
				std::cout << "!------------------------------!" << std::endl;
				return false;
			}
		}
		else {
			//std::cout << "!------------------------------!" << std::endl;
			//std::cout << "!--GetJNetSession return NULL--!" << std::endl;
			//std::cout << "!------------------------------!" << std::endl;
			return false;
		}
#else
		JBuffer buff(msgLen);
		buff << byCode;
		buff << bySize;
		buff << byType;
		buff << Direction;
		buff << X;
		buff << Y;
		Send(remote, buff);
#endif
		return true;
	}
	bool Proxy::MOVE_STOP(HostID remote, BYTE byCode, BYTE bySize, BYTE byType, BYTE Direction, uint16_t X, uint16_t Y) {
		uint32_t msgLen = sizeof(byCode) + sizeof(bySize) + sizeof(byType) + sizeof(Direction) + sizeof(X) + sizeof(Y);
#ifdef DIRECT_ACCESS_TO_JNETSESSION
		stJNetSession* jnetSession = GetJNetSession(remote);
		if (jnetSession != nullptr) {
			JBuffer& buff = (*jnetSession->sendBuff);
			if (buff.GetFreeSize() >= msgLen) {
				buff << byCode;
				buff << bySize;
				buff << byType;
				buff << Direction;
				buff << X;
				buff << Y;
			}
			else {
				std::cout << "!------------------------------!" << std::endl;
				std::cout << "!- buff.GetFreeSize() < msgLen-!" << std::endl;
				std::cout << "!------------------------------!" << std::endl;
				return false;
			}
		}
		else {
			//std::cout << "!------------------------------!" << std::endl;
			//std::cout << "!--GetJNetSession return NULL--!" << std::endl;
			//std::cout << "!------------------------------!" << std::endl;
			return false;
		}
#else
		JBuffer buff(msgLen);
		buff << byCode;
		buff << bySize;
		buff << byType;
		buff << Direction;
		buff << X;
		buff << Y;
		Send(remote, buff);
#endif
		return true;
	}
	bool Proxy::ATTACK1(HostID remote, BYTE byCode, BYTE bySize, BYTE byType, BYTE Direction, uint16_t X, uint16_t Y) {
		uint32_t msgLen = sizeof(byCode) + sizeof(bySize) + sizeof(byType) + sizeof(Direction) + sizeof(X) + sizeof(Y);
#ifdef DIRECT_ACCESS_TO_JNETSESSION
		stJNetSession* jnetSession = GetJNetSession(remote);
		if (jnetSession != nullptr) {
			JBuffer& buff = (*jnetSession->sendBuff);
			if (buff.GetFreeSize() >= msgLen) {
				buff << byCode;
				buff << bySize;
				buff << byType;
				buff << Direction;
				buff << X;
				buff << Y;
			}
			else {
				std::cout << "!------------------------------!" << std::endl;
				std::cout << "!- buff.GetFreeSize() < msgLen-!" << std::endl;
				std::cout << "!------------------------------!" << std::endl;
				return false;
			}
		}
		else {
			//std::cout << "!------------------------------!" << std::endl;
			//std::cout << "!--GetJNetSession return NULL--!" << std::endl;
			//std::cout << "!------------------------------!" << std::endl;
			return false;
		}
#else
		JBuffer buff(msgLen);
		buff << byCode;
		buff << bySize;
		buff << byType;
		buff << Direction;
		buff << X;
		buff << Y;
		Send(remote, buff);
#endif
		return true;
	}
	bool Proxy::ATTACK2(HostID remote, BYTE byCode, BYTE bySize, BYTE byType, BYTE Direction, uint16_t X, uint16_t Y) {
		uint32_t msgLen = sizeof(byCode) + sizeof(bySize) + sizeof(byType) + sizeof(Direction) + sizeof(X) + sizeof(Y);
#ifdef DIRECT_ACCESS_TO_JNETSESSION
		stJNetSession* jnetSession = GetJNetSession(remote);
		if (jnetSession != nullptr) {
			JBuffer& buff = (*jnetSession->sendBuff);
			if (buff.GetFreeSize() >= msgLen) {
				buff << byCode;
				buff << bySize;
				buff << byType;
				buff << Direction;
				buff << X;
				buff << Y;
			}
			else {
				std::cout << "!------------------------------!" << std::endl;
				std::cout << "!- buff.GetFreeSize() < msgLen-!" << std::endl;
				std::cout << "!------------------------------!" << std::endl;
				return false;
			}
		}
		else {
			//std::cout << "!------------------------------!" << std::endl;
			//std::cout << "!--GetJNetSession return NULL--!" << std::endl;
			//std::cout << "!------------------------------!" << std::endl;
			return false;
		}
#else
		JBuffer buff(msgLen);
		buff << byCode;
		buff << bySize;
		buff << byType;
		buff << Direction;
		buff << X;
		buff << Y;
		Send(remote, buff);
#endif
		return true;
	}
	bool Proxy::ATTACK3(HostID remote, BYTE byCode, BYTE bySize, BYTE byType, BYTE Direction, uint16_t X, uint16_t Y) {
		uint32_t msgLen = sizeof(byCode) + sizeof(bySize) + sizeof(byType) + sizeof(Direction) + sizeof(X) + sizeof(Y);
#ifdef DIRECT_ACCESS_TO_JNETSESSION
		stJNetSession* jnetSession = GetJNetSession(remote);
		if (jnetSession != nullptr) {
			JBuffer& buff = (*jnetSession->sendBuff);
			if (buff.GetFreeSize() >= msgLen) {
				buff << byCode;
				buff << bySize;
				buff << byType;
				buff << Direction;
				buff << X;
				buff << Y;
			}
			else {
				std::cout << "!------------------------------!" << std::endl;
				std::cout << "!- buff.GetFreeSize() < msgLen-!" << std::endl;
				std::cout << "!------------------------------!" << std::endl;
				return false;
			}
		}
		else {
			//std::cout << "!------------------------------!" << std::endl;
			//std::cout << "!--GetJNetSession return NULL--!" << std::endl;
			//std::cout << "!------------------------------!" << std::endl;
			return false;
		}
#else
		JBuffer buff(msgLen);
		buff << byCode;
		buff << bySize;
		buff << byType;
		buff << Direction;
		buff << X;
		buff << Y;
		Send(remote, buff);
#endif
		return true;
	}
	bool Proxy::ECHO(HostID remote, BYTE byCode, BYTE bySize, BYTE byType, uint32_t Time) {
		uint32_t msgLen = sizeof(byCode) + sizeof(bySize) + sizeof(byType) + sizeof(Time);
#ifdef DIRECT_ACCESS_TO_JNETSESSION
		stJNetSession* jnetSession = GetJNetSession(remote);
		if (jnetSession != nullptr) {
			JBuffer& buff = (*jnetSession->sendBuff);
			if (buff.GetFreeSize() >= msgLen) {
				buff << byCode;
				buff << bySize;
				buff << byType;
				buff << Time;
			}
			else {
				std::cout << "!------------------------------!" << std::endl;
				std::cout << "!- buff.GetFreeSize() < msgLen-!" << std::endl;
				std::cout << "!------------------------------!" << std::endl;
				return false;
			}
		}
		else {
			//std::cout << "!------------------------------!" << std::endl;
			//std::cout << "!--GetJNetSession return NULL--!" << std::endl;
			//std::cout << "!------------------------------!" << std::endl;
			return false;
		}
#else
		JBuffer buff(msgLen);
		buff << byCode;
		buff << bySize;
		buff << byType;
		buff << Time;
		Send(remote, buff);
#endif
		return true;
	}
}

