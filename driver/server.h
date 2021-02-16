#pragma once

#define magic_header_value 0x59da412859da4128
#define server_ip 0x7F000001 // localhost

enum e_packet_type
{
	packet_get_base,
	packet_copy_memory,
	packet_allocate,
	packet_free,
	packet_protect,
	completed,
	ping
};

typedef struct _packet_header_t
{
	uint64_t magic_header;
	int type;
} packet_header_t, *lpacket_header_t;

typedef struct _packet_response_t
{
	uint64_t data;
} packet_response_t, *lpacket_response_t;

typedef struct _packet_ping_t
{
	int32_t ping_magic;
} packet_ping_t, *lpacket_ping_t;

typedef struct _packet_copy_memory_t
{
	uint32_t source_pid;
	uint64_t source_address;
	uint32_t target_pid;
	uint64_t target_address;
	size_t size;
} packet_copy_memory_t, *lpacket_copy_memory_t;

typedef struct _packet_allocate_memory_t
{
	uint32_t target_pid;
	uint32_t protection;
	uint32_t allocation_type;
	size_t size;
} packet_allocate_memory_t, *lpacket_allocate_memory_t;

typedef struct _packet_free_memory_t
{
	uint32_t target_pid;
	uint64_t target_address;
	size_t region_size;
} packet_free_memory_t, *lpacket_free_memory_t;

typedef struct _packet_protect_memory_t
{
	uint32_t target_pid;
	uint64_t target_address;
	uint32_t new_protection;
	size_t size;
} packet_protect_memory_t, *lpacket_protect_memory_t;

typedef struct _packet_get_base_t
{
	uint32_t target_pid;
	wchar_t module_name[64];
} packet_get_base_t, *lpacket_get_base_t;

typedef struct _data_packet_t
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
} data_packet_t, *lpdata_packet_t;

// server initialization
SOCKET create_server_socket(uint16_t port);
void initialize_server(uint16_t port);

// specific handlers
uint64_t handle_ping(data_packet_t packet);
uint64_t handle_copy_memory(data_packet_t packet);
uint64_t handle_allocate(data_packet_t packet);
uint64_t handle_free(data_packet_t packet);
uint64_t handle_protect(data_packet_t packet);
uint64_t handle_get_base(data_packet_t packet);

// master handler(s)
uint64_t handle_packet(data_packet_t packet);
bool answer_client(SOCKET sockfd, uint64_t result);
void server_thread(SOCKET* sockfd);