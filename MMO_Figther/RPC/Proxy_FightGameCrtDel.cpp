#include "Proxy_FightGameCrtDel.h"

namespace FightGameCrtDel_S2C {
	bool Proxy::CRT_CHARACTER(HostID remote, uint32_t ID, BYTE Dir, uint16_t X, uint16_t Y, BYTE HP) {
		uint32_t msgLen = sizeof(ID) + sizeof(Dir) + sizeof(X) + sizeof(Y) + sizeof(HP);
		stJNetSession* jnetSession = GetJNetSession(remote);
		if (jnetSession != nullptr) {
			JBuffer& buff = jnetSession->sendBuff;
			if (buff.GetFreeSize() >= sizeof(stMSG_HDR) + msgLen) {
				stMSG_HDR hdr;
				hdr.byCode = 0x89;
				hdr.bySize = sizeof(ID) + sizeof(Dir) + sizeof(X) + sizeof(Y) + sizeof(HP);
				hdr.byType = RPC_CRT_CHARACTER;
				buff << hdr;
				buff << ID;
				buff << Dir;
				buff << X;
				buff << Y;
				buff << HP;
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
	bool Proxy::CRT_OTHER_CHARACTER(HostID remote, uint32_t ID, BYTE Dir, uint16_t X, uint16_t Y, BYTE HP) {
		uint32_t msgLen = sizeof(ID) + sizeof(Dir) + sizeof(X) + sizeof(Y) + sizeof(HP);
		stJNetSession* jnetSession = GetJNetSession(remote);
		if (jnetSession != nullptr) {
			JBuffer& buff = jnetSession->sendBuff;
			if (buff.GetFreeSize() >= sizeof(stMSG_HDR) + msgLen) {
				stMSG_HDR hdr;
				hdr.byCode = 0x89;
				hdr.bySize = sizeof(ID) + sizeof(Dir) + sizeof(X) + sizeof(Y) + sizeof(HP);
				hdr.byType = RPC_CRT_OTHER_CHARACTER;
				buff << hdr;
				buff << ID;
				buff << Dir;
				buff << X;
				buff << Y;
				buff << HP;
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
	bool Proxy::DEL_CHARACTER(HostID remote, uint32_t ID) {
		uint32_t msgLen = sizeof(ID);
		stJNetSession* jnetSession = GetJNetSession(remote);
		if (jnetSession != nullptr) {
			JBuffer& buff = jnetSession->sendBuff;
			if (buff.GetFreeSize() >= sizeof(stMSG_HDR) + msgLen) {
				stMSG_HDR hdr;
				hdr.byCode = 0x89;
				hdr.bySize = sizeof(ID);
				hdr.byType = RPC_DEL_CHARACTER;
				buff << hdr;
				buff << ID;
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
namespace FightGameCrtDel_C2S {
}
