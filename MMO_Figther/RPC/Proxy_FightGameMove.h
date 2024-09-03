#pragma once
#include "Common_FightGameMove.h"

namespace FightGameMove_S2C {

	class Proxy : public JNetProxy
	{
	public: 
		virtual bool MOVE_START(HostID remote, uint32_t ID, BYTE Dir, uint16_t X, uint16_t Y);
		virtual bool MOVE_STOP(HostID remote, uint32_t ID, BYTE Dir, uint16_t X, uint16_t Y);

		virtual RpcID* GetRpcList() override { return gRpcList; }
		virtual int GetRpcListCount() override { return gRpcListCount; }
	};
}

namespace FightGameMove_C2S {

	class Proxy : public JNetProxy
	{
	public: 
		virtual bool MOVE_START(HostID remote, BYTE Dir, uint16_t X, uint16_t Y);
		virtual bool MOVE_STOP(HostID remote, BYTE Dir, uint16_t X, uint16_t Y);

		virtual RpcID* GetRpcList() override { return gRpcList; }
		virtual int GetRpcListCount() override { return gRpcListCount; }
	};
}

