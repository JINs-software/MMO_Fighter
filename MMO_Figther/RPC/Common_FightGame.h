#pragma once

namespace FightGameS2C {

	static const RpcID RPC_CRT_CHARACTER = 0;
	static const RpcID RPC_CRT_OTHER_CHARACTER = 1;
	static const RpcID RPC_DEL_CHARACTER = 2;
	static const RpcID RPC_MOVE_START = 11;
	static const RpcID RPC_MOVE_STOP = 13;
	static const RpcID RPC_ATTACK1 = 21;
	static const RpcID RPC_ATTACK2 = 23;
	static const RpcID RPC_ATTACK3 = 25;
	static const RpcID RPC_DAMAGE = 30;
	static const RpcID RPC_SYNC = 251;
	static const RpcID RPC_ECHO = 253;

	extern RpcID gRpcList[];
	extern int gRpcListCount;

}

namespace FightGameC2S {

	static const RpcID RPC_MOVE_START = 10;
	static const RpcID RPC_MOVE_STOP = 12;
	static const RpcID RPC_ATTACK1 = 20;
	static const RpcID RPC_ATTACK2 = 22;
	static const RpcID RPC_ATTACK3 = 24;
	static const RpcID RPC_ECHO = 252;

	extern RpcID gRpcList[];
	extern int gRpcListCount;

}

