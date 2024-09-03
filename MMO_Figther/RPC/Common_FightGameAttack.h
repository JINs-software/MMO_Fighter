#pragma once

namespace FightGameAttack_S2C {
	static const RpcID RPC_ATTACK1 = 21;
	static const RpcID RPC_ATTACK2 = 23;
	static const RpcID RPC_ATTACK3 = 25;

	extern RpcID gRpcList[];
	extern int gRpcListCount;
}

namespace FightGameAttack_C2S {
	static const RpcID RPC_ATTACK1 = 20;
	static const RpcID RPC_ATTACK2 = 22;
	static const RpcID RPC_ATTACK3 = 24;

	extern RpcID gRpcList[];
	extern int gRpcListCount;
}

