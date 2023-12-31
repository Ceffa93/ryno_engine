#include "Client.h"
#include "NetStructs.h"
#include "Scene.h"
#include "Game.h"
#include "NetworkScene.h"


namespace Ryno {


	void Client::start() {

		net_scene = (NetworkScene*)Game::get_instance()->get_scene();

		//Initialize and bind socket
		if (!sock.init(true)) {
			close();
			return;
		}
		sock.set_blocking(false);
		sock.bind(local_address);
		NetUtil::print(local_address.to_string());

	}

	bool Client::update() {
		//check if socket is created
		if (!sock.create_state.up())
			return false;

		SmallAddress addr;
		NetMessage mess;
		I32 res = 0;

		net_time.update_time();

		//Try to send periodic time update
		send_client_update();

		//**RECEIVE MESSAGES**//

		//Receive and dispatch messages.
		//It handles directly a few of them,
		//and dispatch the others to the network scene
		while ((res = sock.recv_struct(&mess, addr)) != 0) {
			if (res < 0) {
				close();
				return false;
			}

			mess.header.to_hardware_order();

			//Update network time after update from network.
			//If the client is not connected, then connect it
			//and call on_client_started callback on the network scene
			if (mess.header.code == NetCode::SERVER_TIME) {
				mess.server_update.to_hardware_order();
				F32 new_time = mess.server_update.get_time();

				if (!connected) {
					client_id = mess.server_update.client_id;
					connected = true;
					net_scene->on_client_started();
				}
				net_time.recv_time(new_time);
			}
			//Handle update of scores from the network.
			//It doesn't do it here, but delegate the job to the net scene
			else if (mess.header.code == NetCode::UPDATE) {
				mess.net_array.to_hardware_order();
				net_scene->on_periodic_update_recv(&mess);

			}
			//Handle update of a network object. Delegated to the scene
			else if (connected && mess.header.code == NetCode::OBJECT){
				mess.object_update.to_hardware_order();
				net_scene->on_network_recv(&mess);
			}
		}

		//**SEND MESSAGES**//

		//If connected starts to send updates about net_objects
		if (connected) {
			for (NetObject* net_obj : NetObject::net_objects) {
				bool need_update = net_obj->last_sent + net_obj->send_delay <= TimeManager::time;
				bool need_disconnect = net_obj->last_received + net_obj->disconnect_delay <= TimeManager::time;

				//Send update only if necessary
				if (need_update && net_obj->owned) {
					net_obj->last_sent = TimeManager::time;
					NetMessage m;
					net_scene->on_network_send(net_obj, &m);
					m.header.id.client_id = client_id;
					m.header.set_timestamp(net_time.get_time());
					m.header.to_network_order();
					m.object_update.to_network_order();

					sock.send_struct(&m, server_address);
				}
				//Handle destruction of objects inactive for too long
				else if (need_disconnect && !net_obj->owned) {
					net_obj->mark_for_destruction = true;
				}
			}
		}
		//Destroy all objects marked for distruction
		net_scene->remove_unused_net_objects();
		return true;
	}
	
	void Client::send_client_update() {
		//Return if just sent a message
		if (net_time.last_sent + net_time.update_frequence > TimeManager::time)
			return;

		//Send local time to server
		NetMessage message;
		message.header.code = NetCode::CLIENT_TIME;
		message.header.set_timestamp(net_time.get_time());
		message.header.to_network_order();
		message.client_update.to_network_order();
		net_time.last_sent = TimeManager::time;
		sock.send_struct(&message, server_address);

	}

	//Close client and wipe every net object from the scene
	void Client::close() {
		client_id = 0;
		connected = false;
		sock.close();
	}
}
