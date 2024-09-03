#include "Proxy_FightGameDamage.h"

namespace FightGameDamage_S2C {
	bool Proxy::DAMAGE(HostID remote, uint32_t attker, uint32_t target, BYTE targetHP) {
		uint32_t msgLen = sizeof(attker) + sizeof(target) + sizeof(targetHP);
		stJNetSession* jnetSession = GetJNetSession(remote);
		if (jnetSession != nullptr) {
			JBuffer& buff = jnetSession->sendBuff;
			if (buff.GetFreeSize() >= sizeof(stMSG_HDR) + msgLen) {
				stMSG_HDR hdr;
				hdr.byCode = 0x89;
				hdr.bySize = sizeof(attker) + sizeof(target) + sizeof(targetHP);
				hdr.byType = RPC_DAMAGE;
				buff << hdr;
				buff << attker;
				buff << target;
				buff << targetHP;
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
namespace FightGameDamage_C2S {
}
