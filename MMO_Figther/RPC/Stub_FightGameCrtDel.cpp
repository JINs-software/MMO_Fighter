#include "Stub_FightGameCrtDel.h"

namespace FightGameCrtDel_S2C {

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
		case RPC_CRT_CHARACTER:
		{
			uint32_t ID;
			jbuff >> ID;
			BYTE Dir;
			jbuff >> Dir;
			uint16_t X;
			jbuff >> X;
			uint16_t Y;
			jbuff >> Y;
			BYTE HP;
			jbuff >> HP;
			CRT_CHARACTER(remote, ID, Dir, X, Y, HP);
		}
		break;
		case RPC_CRT_OTHER_CHARACTER:
		{
			uint32_t ID;
			jbuff >> ID;
			BYTE Dir;
			jbuff >> Dir;
			uint16_t X;
			jbuff >> X;
			uint16_t Y;
			jbuff >> Y;
			BYTE HP;
			jbuff >> HP;
			CRT_OTHER_CHARACTER(remote, ID, Dir, X, Y, HP);
		}
		break;
		case RPC_DEL_CHARACTER:
		{
			uint32_t ID;
			jbuff >> ID;
			DEL_CHARACTER(remote, ID);
		}
		break;
		}
	}
}

namespace FightGameCrtDel_C2S {

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

