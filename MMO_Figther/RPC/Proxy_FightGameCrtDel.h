#pragma once
#include "Common_FightGameCrtDel.h"

namespace FightGameCrtDel_S2C {

	class Proxy : public JNetProxy
	{
	public: 
		virtual bool CRT_CHARACTER(HostID remote, uint32_t ID, BYTE Dir, uint16_t X, uint16_t Y, BYTE HP);
		virtual bool CRT_OTHER_CHARACTER(HostID remote, uint32_t ID, BYTE Dir, uint16_t X, uint16_t Y, BYTE HP);
		virtual bool DEL_CHARACTER(HostID remote, uint32_t ID);

		virtual RpcID* GetRpcList() override { return gRpcList; }
		virtual int GetRpcListCount() override { return gRpcListCount; }
	};
}

namespace FightGameCrtDel_C2S {

	class Proxy : public JNetProxy
	{
	public: 

		virtual RpcID* GetRpcList() override { return gRpcList; }
		virtual int GetRpcListCount() override { return gRpcListCount; }
	};
}

