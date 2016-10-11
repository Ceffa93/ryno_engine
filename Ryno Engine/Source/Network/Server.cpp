#include "Server.h"
#include "NetUtil.h"

namespace Ryno {

	void Server::start() {

		if (!sock.init()) {
			close();
			return;
		}

		sock.set_blocking(true);
		sock.bind(server_ip,server_port);
		sock.listen();
	}
	
	void Server::update()
	{
		fd_set readable;
		fd_set writeable;

		FD_ZERO(&readable);
		FD_ZERO(&writeable);

		FD_SET(sock.get(), &readable);


		for (auto conn : conns)
		{
			if (conn->want_read())
				FD_SET(conn->sock->get(), &readable);
			if (conn->want_write())
				FD_SET(conn->sock->get(), &writeable);
		}

		int count = select(0, &readable, &writeable, NULL, &timeout);
		if (count == SOCKET_ERROR)
		{
			NetUtil::print_error("Select error: ");
		}

		//Handle server socket
		if (FD_ISSET(sock.get(), &readable))
		{
			Socket* client_sock = sock.accept();
			client_sock->set_blocking(true);

			if (conns.size() >= max_conns - 1)
				client_sock->close();
			else
				conns.push_back(new Connection(client_sock));
		}

		//Handle clients
		for (auto it = conns.begin(); it != conns.end(); )  //No increment
		{
			Connection *conn = *it;
			bool alive = true;

			if (FD_ISSET(conn->sock->get(), &readable)) {
				conn->message.clear();
				alive &= conn->do_read();
				if (alive) {
					std::string new_mex = conn->message;
					for (auto new_it = conns.begin(); new_it != conns.end(); new_it++) {
						Connection* temp_c = *new_it;
						if (FD_ISSET(temp_c->sock->get(), &writeable)) {
							temp_c->message = new_mex;
							alive &= temp_c->do_write(new_mex);
						}
					}
				}
			}
			
		

			if (!alive)
			{
				delete conn;
				it = conns.erase(it);
			}
			else
				++it;
		}
	}

	void Server::close() {
		sock.close();
	}

	void Server::set_timeout(U32 microseconds) {
		timeout.tv_sec = microseconds/1000000;
		timeout.tv_usec = microseconds%1000000;
	}
}