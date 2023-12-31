#include "Socket.h"
#include "Network.h"

namespace Ryno {

	Socket::Socket()
		: sock(INVALID_SOCKET) {
		reset_states();
	}

	Socket::Socket(SOCKET _sock) {
		sock = _sock;
		verify_socket();
	}


	const SOCKET Socket::get() {
		return sock;
	}

	bool Socket::init(bool datagram) {
		sock = socket(AF_INET, datagram ? SOCK_DGRAM : SOCK_STREAM, 0);
		verify_socket();
		return create_state.up();
	}

	void Socket::verify_socket() {
		if (sock == INVALID_SOCKET) {
			NetUtil::print_error("Socket create error: ");
			create_state.set_down();
		}
		else {
			NetUtil::print("Socket create success.");
			create_state.set_up();
		}
	}

	I32 Socket::send(const std::string* message) {
		I32 size = ::send(sock, message->c_str(),message->size()+1, 0);
		if (size == SOCKET_ERROR)
		{
			I32 error = WSAGetLastError();
			if (error == WSAEWOULDBLOCK) {
				return 0;
			}
			NetUtil::print_error("Send error: ");
			return -1;
		}

		return size;
	}

	I8 Socket::recv_char(C* c) {
		if (::recv(sock, c, 1, 0) == SOCKET_ERROR) {
			I32 error = WSAGetLastError();
			if (error == WSAEWOULDBLOCK) {
				return 0;
			}
			else {
				NetUtil::print_error("Recv error: ");
				return -1;
			}
		}
		return 1;
	}

	bool Socket::bind(SmallAddress& address) {
		Address a = address.get_address();
		if (!create_state.up()) {
			NetUtil::print("Cannot connect if socket not created.");
			return nullptr;
		}
	
		if (::bind(sock, (const sockaddr *)&a, sizeof(a)) == SOCKET_ERROR) {
			bind_state.set_down();
			NetUtil::print_error("Bind failed: ");
			return false;
		}
		bind_state.set_up();
		NetUtil::print("Bind success.");
		address = get_socket_address();
		return true;
	}

	SmallAddress Socket::get_socket_address() {
		Address addr;
		I32 len = sizeof(sockaddr_in);
		if (getsockname(sock, (struct sockaddr *)&addr, &len) == SOCKET_ERROR)
			NetUtil::print_error("Get socket name error: ");

		return SmallAddress(addr);
	}

	I8 Socket::connect(const C* server_ip, U32 server_port) {
		if (!create_state.up()) {
			NetUtil::print("Cannot connect if socket not created.");
			return -1;
		}
		//Already connected
		if (connect_state.up())
			return 1;
		sockaddr_in addr;
		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = inet_addr(server_ip);
		addr.sin_port = htons(server_port);
		
		if (::connect(sock, (const sockaddr *)&addr, sizeof addr) == SOCKET_ERROR) {
			int err = WSAGetLastError();
			if (err == WSAEWOULDBLOCK || err == WSAEALREADY) {
				connect_state.set_loading();
				return 0;
			}
			else if (err == WSAEISCONN) {
					connect_state.set_up();
				NetUtil::print("Connect success.");
				return 1;
			}else{
				connect_state.set_down();
				NetUtil::print_error("Connect failed: ");
				return -1;
			}
		}
		connect_state.set_up();
		NetUtil::print("Connect success.");
		return 1;
	}

	bool Socket::listen() {
		if (!bind_state.up()) {
			NetUtil::print("Cannot listen if not bound.");
			return nullptr;
		}
		if (::listen(sock, 1) != 0) {
			listen_state.set_down();
			NetUtil::print_error("Listen failed: ");
			return false;
		}
		listen_state.set_up();
		NetUtil::print("Listen success.");
		return true;
	}

	Socket* Socket::accept() {
		if (!listen_state.up()) {
			NetUtil::print("Cannot accept if not listening.");
			return nullptr;
		}
		sockaddr_in client_addr;
		int addr_size = sizeof(client_addr);
		Socket* client_sock = new Socket(::accept(sock, (sockaddr *)&client_addr, &addr_size));
		if (client_sock->get() == INVALID_SOCKET) {
			int err = WSAGetLastError();
			if (err == WSAEWOULDBLOCK || err == WSAEALREADY) {
				accept_state.set_loading();
				return 0;
			}
			else {
				accept_state.down();
				NetUtil::print_error("Accept failed: ");
				delete client_sock;
				return nullptr;
			}
		}
		accept_state.up();
		NetUtil::print("Accept successful.");
		return client_sock;
	}


	void Socket::close() {
		closesocket(sock);
		sock = INVALID_SOCKET;
	}

	void Socket::set_blocking(bool b) {
		ULONG value = b ? 0 : 1;
		ioctlsocket(sock, FIONBIO, &value);
	}

	void Socket::reset_states() {
		create_state.set_down();
		bind_state.set_down();
		listen_state.set_down();
		connect_state.set_down();
		accept_state.set_down();
	}

	I32 Socket::send_struct(const NetMessage* message, const SmallAddress& to) {
		Address a = to.get_address();
		I32 size = ::sendto(sock, (C*)message, sizeof(NetMessage), 0, (sockaddr*)&a, sizeof(sockaddr_in));
		if (size == SOCKET_ERROR)
		{
			I32 error = WSAGetLastError();
			if (error == WSAEWOULDBLOCK) {
				return 0;
			}
			NetUtil::print_error("Send error: ");
			return -1;
		}

		return size;
	}

	I32 Socket::recv_struct(NetMessage* message, SmallAddress& from) {
		Address a;
		I32 length = sizeof(sockaddr_in);
		I32 size = ::recvfrom(sock, (C*)message, sizeof(NetMessage), 0, (sockaddr*)&a, &length);
		from.set(a);
		if (size == SOCKET_ERROR) {
			I32 error = WSAGetLastError();
			if (error == WSAEWOULDBLOCK) {
				return 0;
			}
			else {
				NetUtil::print_error("Recv error: ");
				return -1;
			}
		}
		return size;
	}

}