#pragma once

namespace connection
{
	constexpr auto magic_header_value = 0x59da412859da4128;
	constexpr auto server_ip = 0x7F000001; // localhost

	enum class e_packet_type : uint32_t
	{
		packet_get_base,
		packet_copy_memory,
		packet_allocate,
		packet_free,
		packet_protect,
		completed,
		ping
	};

	struct packet_header_t
	{
		uint64_t magic_header;
		e_packet_type type;
	};

	struct packet_response_t
	{
		uint64_t data;
	};

	struct packet_ping_t
	{
		int32_t ping_magic;
	};

	struct packet_copy_memory_t
	{
		uint32_t source_pid;
		uint64_t source_address;
		uint32_t target_pid;
		uint64_t target_address;
		size_t size;
	};

	struct packet_allocate_memory_t
	{
		uint32_t target_pid;
		uint32_t protection;
		uint32_t allocation_type;
		size_t size;
	};

	struct packet_free_memory_t
	{
		uint32_t target_pid;
		uint64_t target_address;
		size_t size;
	};

	struct packet_get_base_t
	{
		uint32_t target_pid;
		wchar_t module_name[64];
	};

	struct packet_protect_memory_t
	{
		uint32_t target_pid;
		uint64_t target_address;
		uint32_t new_protection;
		size_t size;
	};

	struct data_packet_t
	{
		packet_header_t header;

		union
		{
			packet_response_t response;
			packet_ping_t ping;
			packet_copy_memory_t copy_memory;
			packet_allocate_memory_t allocate_memory;
			packet_free_memory_t free_memory;
			packet_protect_memory_t protect_memory;
			packet_get_base_t get_base;
		} data;
	};

	class ksock_t
	{
	protected:
		WSADATA wsa_data;
		SOCKET connection;

	public:
		uint16_t port;

		ksock_t(uint16_t port);
		~ksock_t();

		SOCKET connect(uint16_t port);
		void disconnect();

		bool send_data(data_packet_t& packet, uint64_t& result);

		uint64_t ping(int32_t value);
		
		uint64_t get_base(uint32_t target_pid, const wchar_t* module_name);

		uint64_t mm_copy_virtual_memory(uint32_t source_pid, uintptr_t source_address, uint32_t target_pid,
			uintptr_t target_address, size_t size);

		uint64_t zw_allocate_virtual_memory(uint32_t target_pid, uint32_t protection, uint32_t allocation_type, size_t size);
		uint64_t zw_free_vritual_memory(uint32_t target_pid, uint64_t target_address, size_t size);
		uint64_t zw_protect_virtual_memory(uint32_t target_pid, uint64_t target_address, uint32_t protection, size_t size);
	};
}