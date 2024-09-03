#pragma once

namespace FightGameComm_S2C {
	static const RpcID RPC_SYNC = 251;
	static const RpcID RPC_ECHO = 253;

	extern RpcID gRpcList[];
	extern int gRpcListCount;
}

namespace FightGameComm_C2S {
	static const RpcID RPC_ECHO = 252;

	extern RpcID gRpcList[];
	extern int gRpcListCount;
}

