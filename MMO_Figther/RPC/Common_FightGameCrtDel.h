#pragma once

namespace FightGameCrtDel_S2C {
	static const RpcID RPC_CRT_CHARACTER = 0;
	static const RpcID RPC_CRT_OTHER_CHARACTER = 1;
	static const RpcID RPC_DEL_CHARACTER = 2;

	extern RpcID gRpcList[];
	extern int gRpcListCount;
}

namespace FightGameCrtDel_C2S {

	extern RpcID gRpcList[];
	extern int gRpcListCount;
}

