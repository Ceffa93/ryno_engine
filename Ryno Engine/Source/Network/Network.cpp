#include "Network.h"
#include "Server.h"
#include "Client.h"


namespace Ryno {

	Network* Network::get_instance() {
		static Network instance;//only at the beginning
		return &instance;
	}

	Network::~Network() {
		stop_client();
		stop_server();
	}

	void Network::init() {
		// Initialise the WinSock library -- we want version 2.2.
		WSADATA w;
		int error = WSAStartup(0x0202, &w);
		if (error != 0)
			Log::println("WSAStartup failed");
	
		if (w.wVersion != 0x0202)
			Log::println("Wrong WinSock version");
	}
	
	void Network::start_server() {
		stop_server();
		has_server = true;
		server = new Server("127.0.0.1", 5555);
		server->start();
	}

	void Network::start_client() {
		stop_client();
		client = new Client("127.0.0.1", 5555,"127.0.0.1");
		has_client = true;
		client->start();
	}

	void Network::update() {
		if (has_server && server) server->update();
		if (has_client && client) client->update();
	}


	void Network::stop_client() {
		if (client) {
			delete client;
			client = nullptr;
		}
		has_client = false;
	}
	void Network::stop_server() {
		if (server) {
			delete server;
			server = nullptr;
		}
		has_server = false;
	}
}