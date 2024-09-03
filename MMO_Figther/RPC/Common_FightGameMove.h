#pragma once

namespace FightGameMove_S2C {
	static const RpcID RPC_MOVE_START = 11;
	static const RpcID RPC_MOVE_STOP = 13;

	extern RpcID gRpcList[];
	extern int gRpcListCount;
}

namespace FightGameMove_C2S {
	static const RpcID RPC_MOVE_START = 10;
	static const RpcID RPC_MOVE_STOP = 12;

	extern RpcID gRpcList[];
	extern int gRpcListCount;
}

