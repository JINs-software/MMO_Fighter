#include "Proxy_FightGameMove.h"

namespace FightGameMove_S2C {
	bool Proxy::MOVE_START(HostID remote, uint32_t ID, BYTE Dir, uint16_t X, uint16_t Y) {
		uint32_t msgLen = sizeof(ID) + sizeof(Dir) + sizeof(X) + sizeof(Y);
		stJNetSession* jnetSession = GetJNetSession(remote);
		if (jnetSession != nullptr) {
			JBuffer& buff = jnetSession->sendBuff;
			if (buff.GetFreeSize() >= sizeof(stMSG_HDR) + msgLen) {
				stMSG_HDR hdr;
				hdr.byCode = 0x89;
				hdr.bySize = sizeof(ID) + sizeof(Dir) + sizeof(X) + sizeof(Y);
				hdr.byType = RPC_MOVE_START;
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
	bool Proxy::MOVE_STOP(HostID remote, uint32_t ID, BYTE Dir, uint16_t X, uint16_t Y) {
		uint32_t msgLen = sizeof(ID) + sizeof(Dir) + sizeof(X) + sizeof(Y);
		stJNetSession* jnetSession = GetJNetSession(remote);
		if (jnetSession != nullptr) {
			JBuffer& buff = jnetSession->sendBuff;
			if (buff.GetFreeSize() >= sizeof(stMSG_HDR) + msgLen) {
				stMSG_HDR hdr;
				hdr.byCode = 0x89;
				hdr.bySize = sizeof(ID) + sizeof(Dir) + sizeof(X) + sizeof(Y);
				hdr.byType = RPC_MOVE_STOP;
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
namespace FightGameMove_C2S {
	bool Proxy::MOVE_START(HostID remote, BYTE Dir, uint16_t X, uint16_t Y) {
		uint32_t msgLen = sizeof(Dir) + sizeof(X) + sizeof(Y);
		stJNetSession* jnetSession = GetJNetSession(remote);
		if (jnetSession != nullptr) {
			JBuffer& buff = jnetSession->sendBuff;
			if (buff.GetFreeSize() >= sizeof(stMSG_HDR) + msgLen) {
				stMSG_HDR hdr;
				hdr.byCode = 0x89;
				hdr.bySize = sizeof(Dir) + sizeof(X) + sizeof(Y);
				hdr.byType = RPC_MOVE_START;
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
	bool Proxy::MOVE_STOP(HostID remote, BYTE Dir, uint16_t X, uint16_t Y) {
		uint32_t msgLen = sizeof(Dir) + sizeof(X) + sizeof(Y);
		stJNetSession* jnetSession = GetJNetSession(remote);
		if (jnetSession != nullptr) {
			JBuffer& buff = jnetSession->sendBuff;
			if (buff.GetFreeSize() >= sizeof(stMSG_HDR) + msgLen) {
				stMSG_HDR hdr;
				hdr.byCode = 0x89;
				hdr.bySize = sizeof(Dir) + sizeof(X) + sizeof(Y);
				hdr.byType = RPC_MOVE_STOP;
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
