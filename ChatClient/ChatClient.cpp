#define NOMINMAX

#include <enet/enet.h>
#include <iostream>
#include <string>
#include <thread>
#include "C:\Users\jquintana\source\repos\Enet\ChatServer\ChatServer.h"

using namespace std;

ENetAddress address;
ENetHost* server = nullptr;
ENetHost* client = nullptr;
ENetHost* serverName;

string clientName;

std::thread getMessageThread;

void sendPacket(string message);

bool CreateClient()
{
    client = enet_host_create(nullptr /* create a client host */,
        1 /* only allow 1 outgoing connection */,
        2 /* allow up 2 channels to be used, 0 and 1 */,
        0 /* assume any amount of incoming bandwidth */,
        0 /* assume any amount of outgoing bandwidth */);

    return client != nullptr;
}

void getMessage()
{
    while (1) 
    {
        cout << "Enter Message: " << endl;
        string message;
        //cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::getline(std::cin, message);
        sendPacket(message);
    }
}

void sendPacket(string message)
{

    ENetPacket* packet = enet_packet_create(message.c_str(),
        message.length() + 1,
        ENET_PACKET_FLAG_RELIABLE);

    enet_host_broadcast(client, 0, packet);

    enet_host_flush(client);
}


int main()
{
    // Initializes network code
    if (enet_initialize() != 0)
    {
        fprintf(stderr, "An error occurred while initializing ENet.\n");
        cout << "An error occurred while initializing ENet." << endl;
        return EXIT_FAILURE;
    }
    atexit(enet_deinitialize);

    if (!CreateClient())
    {
        fprintf(stderr,
            "An error occurred while trying to create an ENet client host.\n");
        exit(EXIT_FAILURE);
    }

    cout << "Please create client name: " << endl;
    cin >> clientName;

    // CONNECTING TO SERVER
    ENetAddress address;
    ENetEvent event;
    ENetPeer* peer;
    /* Connect to some.server.net:1234. */
    enet_address_set_host(&address, "localhost");
    address.port = 1234;
    /* Initiate the connection, allocating the two channels 0 and 1. */
    peer = enet_host_connect(client, &address, 2, 0);
    if (peer == nullptr)
    {
        fprintf(stderr,
            "No available peers for initiating an ENet connection.\n");
        exit(EXIT_FAILURE);
    }
    /* Wait up to 5 seconds for the connection attempt to succeed. */
    if (enet_host_service(client, &event, 5000) > 0 &&
        event.type == ENET_EVENT_TYPE_CONNECT)
    {
        cout << "Connection to "<< serverName << " succeeded." << endl;
        string clientNameString = clientName + " has joined the server.";

        // Creates a packet with the client name
        ENetPacket* packet = enet_packet_create(clientNameString.c_str(),
            clientNameString.length() + 1,
            ENET_PACKET_FLAG_RELIABLE);

        enet_host_broadcast(client, 0, packet);
        //enet_peer_send(client, 0, packet);

        /* One could just use enet_host_service() instead. */
        //enet_host_service();
        enet_host_flush(client);

        getMessageThread = thread(getMessage);
    }
    else
    {
        /* Either the 5 seconds are up or a disconnect event was */
        /* received. Reset the peer in the event the 5 seconds   */
        /* had run out without any significant event.            */
        enet_peer_reset(peer);
        cout << "Connection to localhost failed." << endl;
    }

    while (1)
    {
        ENetEvent event;
        // Wait up to 1000 ms for an event
        while (enet_host_service(client, &event, 1000) > 0)
        {
            switch (event.type)
            {
            case ENET_EVENT_TYPE_RECEIVE:
                cout << serverName << " said: " << (char*)event.packet->data << endl;
                {
                    //TODO: Update sendPacket()
                    //sendPacket(event);

                }
            }
        }
    }

    if (client != nullptr)
    {

        enet_host_destroy(client);
    }

    return EXIT_SUCCESS;

}
