#include "Stub_FightGameDamage.h"

namespace FightGameDamage_S2C {

	void Stub::ProcessReceivedMessage(HostID remote, JBuffer& jbuff) {
		if (jbuff.GetUseSize() < sizeof(stMSG_HDR)) {
			return;
		}
		stMSG_HDR hdr;
		jbuff.Peek(&hdr);
		if (sizeof(hdr) + hdr.bySize > jbuff.GetUseSize()) {
			return;
		}
		jbuff.Dequeue((BYTE*)&hdr, sizeof(hdr));
		switch(static_cast<RpcID>(hdr.byType)) {
		case RPC_DAMAGE:
		{
			uint32_t attker;
			jbuff >> attker;
			uint32_t target;
			jbuff >> target;
			BYTE targetHP;
			jbuff >> targetHP;
			DAMAGE(remote, attker, target, targetHP);
		}
		break;
		}
	}
}

namespace FightGameDamage_C2S {

	void Stub::ProcessReceivedMessage(HostID remote, JBuffer& jbuff) {
		if (jbuff.GetUseSize() < sizeof(stMSG_HDR)) {
			return;
		}
		stMSG_HDR hdr;
		jbuff.Peek(&hdr);
		if (sizeof(hdr) + hdr.bySize > jbuff.GetUseSize()) {
			return;
		}
		jbuff.Dequeue((BYTE*)&hdr, sizeof(hdr));
		switch(static_cast<RpcID>(hdr.byType)) {
		}
	}
}

