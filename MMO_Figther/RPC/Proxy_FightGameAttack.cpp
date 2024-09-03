#include "Proxy_FightGameAttack.h"

namespace FightGameAttack_S2C {
	bool Proxy::ATTACK1(HostID remote, uint32_t ID, BYTE Dir, uint16_t X, uint16_t Y) {
		uint32_t msgLen = sizeof(ID) + sizeof(Dir) + sizeof(X) + sizeof(Y);
		stJNetSession* jnetSession = GetJNetSession(remote);
		if (jnetSession != nullptr) {
			JBuffer& buff = jnetSession->sendBuff;
			if (buff.GetFreeSize() >= sizeof(stMSG_HDR) + msgLen) {
				stMSG_HDR hdr;
				hdr.byCode = 0x89;
				hdr.bySize = sizeof(ID) + sizeof(Dir) + sizeof(X) + sizeof(Y);
				hdr.byType = RPC_ATTACK1;
				buff << hdr;
				buff << ID;
				buff << Dir;
				buff << X;
				buff << Y;
			}
			else {
				return false;
			}
		}
		else {
			return false;
		}

		return true;
	}
	bool Proxy::ATTACK2(HostID remote, uint32_t ID, BYTE Dir, uint16_t X, uint16_t Y) {
		uint32_t msgLen = sizeof(ID) + sizeof(Dir) + sizeof(X) + sizeof(Y);
		stJNetSession* jnetSession = GetJNetSession(remote);
		if (jnetSession != nullptr) {
			JBuffer& buff = jnetSession->sendBuff;
			if (buff.GetFreeSize() >= sizeof(stMSG_HDR) + msgLen) {
				stMSG_HDR hdr;
				hdr.byCode = 0x89;
				hdr.bySize = sizeof(ID) + sizeof(Dir) + sizeof(X) + sizeof(Y);
				hdr.byType = RPC_ATTACK2;
				buff << hdr;
				buff << ID;
				buff << Dir;
				buff << X;
				buff << Y;
			}
			else {
				return false;
			}
		}
		else {
			return false;
		}

		return true;
	}
	bool Proxy::ATTACK3(HostID remote, uint32_t ID, BYTE Dir, uint16_t X, uint16_t Y) {
		uint32_t msgLen = sizeof(ID) + sizeof(Dir) + sizeof(X) + sizeof(Y);
		stJNetSession* jnetSession = GetJNetSession(remote);
		if (jnetSession != nullptr) {
			JBuffer& buff = jnetSession->sendBuff;
			if (buff.GetFreeSize() >= sizeof(stMSG_HDR) + msgLen) {
				stMSG_HDR hdr;
				hdr.byCode = 0x89;
				hdr.bySize = sizeof(ID) + sizeof(Dir) + sizeof(X) + sizeof(Y);
				hdr.byType = RPC_ATTACK3;
				buff << hdr;
				buff << ID;
				buff << Dir;
				buff << X;
				buff << Y;
			}
			else {
				return false;
			}
		}
		else {
			return false;
		}

		return true;
	}
}
namespace FightGameAttack_C2S {
	bool Proxy::ATTACK1(HostID remote, BYTE Dir, uint16_t X, uint16_t Y) {
		uint32_t msgLen = sizeof(Dir) + sizeof(X) + sizeof(Y);
		stJNetSession* jnetSession = GetJNetSession(remote);
		if (jnetSession != nullptr) {
			JBuffer& buff = jnetSession->sendBuff;
			if (buff.GetFreeSize() >= sizeof(stMSG_HDR) + msgLen) {
				stMSG_HDR hdr;
				hdr.byCode = 0x89;
				hdr.bySize = sizeof(Dir) + sizeof(X) + sizeof(Y);
				hdr.byType = RPC_ATTACK1;
				buff << hdr;
				buff << Dir;
				buff << X;
				buff << Y;
			}
			else {
				return false;
			}
		}
		else {
			return false;
		}

		return true;
	}
	bool Proxy::ATTACK2(HostID remote, BYTE Dir, uint16_t X, uint16_t Y) {
		uint32_t msgLen = sizeof(Dir) + sizeof(X) + sizeof(Y);
		stJNetSession* jnetSession = GetJNetSession(remote);
		if (jnetSession != nullptr) {
			JBuffer& buff = jnetSession->sendBuff;
			if (buff.GetFreeSize() >= sizeof(stMSG_HDR) + msgLen) {
				stMSG_HDR hdr;
				hdr.byCode = 0x89;
				hdr.bySize = sizeof(Dir) + sizeof(X) + sizeof(Y);
				hdr.byType = RPC_ATTACK2;
				buff << hdr;
				buff << Dir;
				buff << X;
				buff << Y;
			}
			else {
				return false;
			}
		}
		else {
			return false;
		}

		return true;
	}
	bool Proxy::ATTACK3(HostID remote, BYTE Dir, uint16_t X, uint16_t Y) {
		uint32_t msgLen = sizeof(Dir) + sizeof(X) + sizeof(Y);
		stJNetSession* jnetSession = GetJNetSession(remote);
		if (jnetSession != nullptr) {
			JBuffer& buff = jnetSession->sendBuff;
			if (buff.GetFreeSize() >= sizeof(stMSG_HDR) + msgLen) {
				stMSG_HDR hdr;
				hdr.byCode = 0x89;
				hdr.bySize = sizeof(Dir) + sizeof(X) + sizeof(Y);
				hdr.byType = RPC_ATTACK3;
				buff << hdr;
				buff << Dir;
				buff << X;
				buff << Y;
			}
			else {
				return false;
			}
		}
		else {
			return false;
		}

		return true;
	}
}
