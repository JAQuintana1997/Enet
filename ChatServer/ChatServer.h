#pragma once

#include <enet/enet.h>
#include <iostream>
#include <thread>
#include <string>
using namespace std;


class ChatServer
{
public:

	ENetAddress address;
	ENetHost* server;
	ENetHost* client;

	bool CreateServer();
	void sendPacket();
	void getMessage();
};