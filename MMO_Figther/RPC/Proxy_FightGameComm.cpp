#include "Proxy_FightGameComm.h"

namespace FightGameComm_S2C {
	bool Proxy::SYNC(HostID remote, uint32_t ID, uint16_t X, uint16_t Y) {
		uint32_t msgLen = sizeof(ID) + sizeof(X) + sizeof(Y);
		stJNetSession* jnetSession = GetJNetSession(remote);
		if (jnetSession != nullptr) {
			JBuffer& buff = jnetSession->sendBuff;
			if (buff.GetFreeSize() >= sizeof(stMSG_HDR) + msgLen) {
				stMSG_HDR hdr;
				hdr.byCode = 0x89;
				hdr.bySize = sizeof(ID) + sizeof(X) + sizeof(Y);
				hdr.byType = RPC_SYNC;
				buff << hdr;
				buff << ID;
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
	bool Proxy::ECHO(HostID remote, uint32_t Time) {
		uint32_t msgLen = sizeof(Time);
		stJNetSession* jnetSession = GetJNetSession(remote);
		if (jnetSession != nullptr) {
			JBuffer& buff = jnetSession->sendBuff;
			if (buff.GetFreeSize() >= sizeof(stMSG_HDR) + msgLen) {
				stMSG_HDR hdr;
				hdr.byCode = 0x89;
				hdr.bySize = sizeof(Time);
				hdr.byType = RPC_ECHO;
				buff << hdr;
				buff << Time;
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
namespace FightGameComm_C2S {
	bool Proxy::ECHO(HostID remote, uint32_t Time) {
		uint32_t msgLen = sizeof(Time);
		stJNetSession* jnetSession = GetJNetSession(remote);
		if (jnetSession != nullptr) {
			JBuffer& buff = jnetSession->sendBuff;
			if (buff.GetFreeSize() >= sizeof(stMSG_HDR) + msgLen) {
				stMSG_HDR hdr;
				hdr.byCode = 0x89;
				hdr.bySize = sizeof(Time);
				hdr.byType = RPC_ECHO;
				buff << hdr;
				buff << Time;
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
