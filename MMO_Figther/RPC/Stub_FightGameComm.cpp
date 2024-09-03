#include "Stub_FightGameComm.h"

namespace FightGameComm_S2C {

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
		case RPC_SYNC:
		{
			uint32_t ID;
			jbuff >> ID;
			uint16_t X;
			jbuff >> X;
			uint16_t Y;
			jbuff >> Y;
			SYNC(remote, ID, X, Y);
		}
		break;
		case RPC_ECHO:
		{
			uint32_t Time;
			jbuff >> Time;
			ECHO(remote, Time);
		}
		break;
		}
	}
}

namespace FightGameComm_C2S {

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
		case RPC_ECHO:
		{
			uint32_t Time;
			jbuff >> Time;
			ECHO(remote, Time);
		}
		break;
		}
	}
}

