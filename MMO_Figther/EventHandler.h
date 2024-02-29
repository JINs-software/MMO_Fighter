#pragma once
#include "JNetServer.h"

class EventHandler : public JNetServerEventHandler {
public:
	void OnReadyToAccept() override {
		return;
	}
	bool OnConnectRequest() override {
		return true;
	}
	void OnClientJoin(HostID remote) override {
		CreateFighter(remote);
		//std::cout << "[OnClientJoin] host id: " << remote << ", CreateFighter completed!" << std::endl;
	}
	bool OnClientDisconnect(HostID remote) override {
		DeleteFighter(remote, true);
		//std::cout << "[OnClientDisconnect] host id: " << remote << ", Client Disconnected Event!" << std::endl;
		return true;
	}
};