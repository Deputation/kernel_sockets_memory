#include "common.hpp"

connection::ksock_t::ksock_t(uint16_t port)
{
	WSAStartup(MAKEWORD(2, 2), &this->wsa_data);

	this->connection = this->connect(port);
	this->port = port;
}

connection::ksock_t::~ksock_t()
{
	WSACleanup();
}

SOCKET connection::ksock_t::connect(uint16_t port)
{
	SOCKADDR_IN address = {};

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = htonl(server_ip);
	address.sin_port = htons(port);

	const auto sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if (sockfd != INVALID_SOCKET)
	{
		if (::connect(sockfd, reinterpret_cast<SOCKADDR*>(&address), sizeof(address)) == SOCKET_ERROR)
		{
			closesocket(sockfd);
			return INVALID_SOCKET;
		}
		else
		{
			return sockfd;
		}
	}
	else
	{
		return sockfd;
	}
}

void connection::ksock_t::disconnect()
{
	closesocket(connection);
}

bool connection::ksock_t::send_data(data_packet_t& packet, uint64_t& result)
{
	data_packet_t response = {};

	if (send(this->connection, reinterpret_cast<const char*>(&packet), sizeof(packet), 0) == SOCKET_ERROR)
	{
		return false;
	}

	const auto recv_result = recv(this->connection, reinterpret_cast<char*>(&response), sizeof(packet), 0);

	if (recv_result < sizeof(packet_header_t) || response.header.magic_header != magic_header_value
		|| response.header.type != e_packet_type::completed)
	{
		return false;
	}

	result = response.data.response.data;

	return true;
}

uint64_t connection::ksock_t::ping(int32_t value)
{
	data_packet_t ping = {};

	ping.header.magic_header = magic_header_value;
	ping.header.type = e_packet_type::ping;
	ping.data.ping.ping_magic = value;

	auto result = 0ull;
	
	if (this->send_data(ping, result))
	{
		return result;
	}

	return 0;
}

uint64_t connection::ksock_t::get_base(uint32_t target_pid, const wchar_t* module_name)
{
	std::wstring buffer = module_name;

	data_packet_t get_base = {};

	get_base.header.magic_header = magic_header_value; 
	get_base.header.type = e_packet_type::packet_get_base;
	get_base.data.get_base.target_pid = target_pid;

	memset(&get_base.data.get_base.module_name[0], 0, 64 * sizeof(wchar_t));
	memcpy(&get_base.data.get_base.module_name[0], const_cast<wchar_t*>(buffer.data()), 
		(std::wcslen(buffer.data()) + 1) * sizeof(wchar_t));

	auto result = 0ull;

	if (this->send_data(get_base, result))
	{
		return result;
	}

	return 0;
}

uint64_t connection::ksock_t::mm_copy_virtual_memory(uint32_t source_pid, uintptr_t source_address, 
	uint32_t target_pid, uintptr_t target_address, size_t size)
{
	data_packet_t copy_memory = {};

	copy_memory.header.magic_header = magic_header_value;
	copy_memory.header.type = e_packet_type::packet_copy_memory;
	copy_memory.data.copy_memory.source_pid = source_pid;
	copy_memory.data.copy_memory.source_address = source_address;
	copy_memory.data.copy_memory.target_pid = target_pid;
	copy_memory.data.copy_memory.target_address = target_address;
	copy_memory.data.copy_memory.size = size;

	auto result = 0ull;

	if (this->send_data(copy_memory, result))
	{
		return result;
	}

	return 0;
}

uint64_t connection::ksock_t::zw_allocate_virtual_memory(uint32_t target_pid, uint32_t protection, 
	uint32_t allocation_type, size_t size)
{
	data_packet_t allocate_memory = {};

	allocate_memory.header.magic_header = magic_header_value;
	allocate_memory.header.type = e_packet_type::packet_allocate;
	allocate_memory.data.allocate_memory.target_pid = target_pid;
	allocate_memory.data.allocate_memory.protection = protection;
	allocate_memory.data.allocate_memory.allocation_type = allocation_type;
	allocate_memory.data.allocate_memory.size = size;

	auto result = 0ull;

	if (this->send_data(allocate_memory, result))
	{
		return result;
	}

	return 0;
}

uint64_t connection::ksock_t::zw_free_vritual_memory(uint32_t target_pid, uint64_t target_address, size_t size)
{
	data_packet_t free_memory = {};

	free_memory.header.magic_header = magic_header_value;
	free_memory.header.type = e_packet_type::packet_free;
	free_memory.data.free_memory.target_pid = target_pid;
	free_memory.data.free_memory.target_address = target_address;
	free_memory.data.free_memory.size = size;

	auto result = 0ull;

	if (this->send_data(free_memory, result))
	{
		return result;
	}

	return 0;
}

uint64_t connection::ksock_t::zw_protect_virtual_memory(uint32_t target_pid, uint64_t target_address, uint32_t protection, size_t size)
{
	data_packet_t protect_memory = {};

	protect_memory.header.magic_header = magic_header_value;
	protect_memory.header.type = e_packet_type::packet_protect;
	protect_memory.data.protect_memory.target_pid = target_pid;
	protect_memory.data.protect_memory.target_address = target_address;
	protect_memory.data.protect_memory.new_protection = protection;
	protect_memory.data.protect_memory.size = size;

	auto result = 0ull;

	if (this->send_data(protect_memory, result))
	{
		return result;
	}

	return 0;
}
