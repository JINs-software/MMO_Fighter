#include "Stub_FightGameAttack.h"

namespace FightGameAttack_S2C {

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
		case RPC_ATTACK1:
		{
			uint32_t ID;
			jbuff >> ID;
			BYTE Dir;
			jbuff >> Dir;
			uint16_t X;
			jbuff >> X;
			uint16_t Y;
			jbuff >> Y;
			ATTACK1(remote, ID, Dir, X, Y);
		}
		break;
		case RPC_ATTACK2:
		{
			uint32_t ID;
			jbuff >> ID;
			BYTE Dir;
			jbuff >> Dir;
			uint16_t X;
			jbuff >> X;
			uint16_t Y;
			jbuff >> Y;
			ATTACK2(remote, ID, Dir, X, Y);
		}
		break;
		case RPC_ATTACK3:
		{
			uint32_t ID;
			jbuff >> ID;
			BYTE Dir;
			jbuff >> Dir;
			uint16_t X;
			jbuff >> X;
			uint16_t Y;
			jbuff >> Y;
			ATTACK3(remote, ID, Dir, X, Y);
		}
		break;
		}
	}
}

namespace FightGameAttack_C2S {

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
		case RPC_ATTACK1:
		{
			BYTE Dir;
			jbuff >> Dir;
			uint16_t X;
			jbuff >> X;
			uint16_t Y;
			jbuff >> Y;
			ATTACK1(remote, Dir, X, Y);
		}
		break;
		case RPC_ATTACK2:
		{
			BYTE Dir;
			jbuff >> Dir;
			uint16_t X;
			jbuff >> X;
			uint16_t Y;
			jbuff >> Y;
			ATTACK2(remote, Dir, X, Y);
		}
		break;
		case RPC_ATTACK3:
		{
			BYTE Dir;
			jbuff >> Dir;
			uint16_t X;
			jbuff >> X;
			uint16_t Y;
			jbuff >> Y;
			ATTACK3(remote, Dir, X, Y);
		}
		break;
		}
	}
}

