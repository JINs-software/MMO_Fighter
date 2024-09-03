#include "Stub_FightGameMove.h"

namespace FightGameMove_S2C {

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
		case RPC_MOVE_START:
		{
			uint32_t ID;
			jbuff >> ID;
			BYTE Dir;
			jbuff >> Dir;
			uint16_t X;
			jbuff >> X;
			uint16_t Y;
			jbuff >> Y;
			MOVE_START(remote, ID, Dir, X, Y);
		}
		break;
		case RPC_MOVE_STOP:
		{
			uint32_t ID;
			jbuff >> ID;
			BYTE Dir;
			jbuff >> Dir;
			uint16_t X;
			jbuff >> X;
			uint16_t Y;
			jbuff >> Y;
			MOVE_STOP(remote, ID, Dir, X, Y);
		}
		break;
		}
	}
}

namespace FightGameMove_C2S {

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
		case RPC_MOVE_START:
		{
			BYTE Dir;
			jbuff >> Dir;
			uint16_t X;
			jbuff >> X;
			uint16_t Y;
			jbuff >> Y;
			MOVE_START(remote, Dir, X, Y);
		}
		break;
		case RPC_MOVE_STOP:
		{
			BYTE Dir;
			jbuff >> Dir;
			uint16_t X;
			jbuff >> X;
			uint16_t Y;
			jbuff >> Y;
			MOVE_STOP(remote, Dir, X, Y);
		}
		break;
		}
	}
}

