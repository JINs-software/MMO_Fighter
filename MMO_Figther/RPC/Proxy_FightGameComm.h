#pragma once
#include "Common_FightGameComm.h"

namespace FightGameComm_S2C {

	class Proxy : public JNetProxy
	{
	public: 
		virtual bool SYNC(HostID remote, uint32_t ID, uint16_t X, uint16_t Y);
		virtual bool ECHO(HostID remote, uint32_t Time);

		virtual RpcID* GetRpcList() override { return gRpcList; }
		virtual int GetRpcListCount() override { return gRpcListCount; }
	};
}

namespace FightGameComm_C2S {

	class Proxy : public JNetProxy
	{
	public: 
		virtual bool ECHO(HostID remote, uint32_t Time);

		virtual RpcID* GetRpcList() override { return gRpcList; }
		virtual int GetRpcListCount() override { return gRpcListCount; }
	};
}

