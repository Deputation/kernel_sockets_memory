#include "common.h"

SOCKET create_server_socket(uint16_t port)
{
	SOCKADDR_IN address;

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = htonl(server_ip);
	address.sin_port = htons(port);

	SOCKET sockfd = socket_listen(AF_INET, SOCK_STREAM, 0);
	if (sockfd == INVALID_SOCKET)
	{
		log("Failed to create a valid server socket.\n");

		return INVALID_SOCKET;
	}

	if (bind(sockfd, (SOCKADDR*)&address, sizeof(address)) == SOCKET_ERROR)
	{
		log("Failed to bind the server socket.\n");

		closesocket(sockfd);
		return INVALID_SOCKET;
	}

	if (listen(sockfd, 10) == SOCKET_ERROR)
	{
		log("Failed to start listening in on the server socket.\n");

		closesocket(sockfd);
		return INVALID_SOCKET;
	}

	return sockfd;
}

void initialize_server(uint16_t port)
{
	NTSTATUS status = KsInitialize();

	if (!NT_SUCCESS(status))
	{
		log("Failed to initialize KSOCKET.\n");

		return;
	}

	SOCKET server_socket = create_server_socket(port);

	if (server_socket == INVALID_SOCKET)
	{
		log("Failed to initialize the server socket.\n");

		KsDestroy();
		return;
	}

	log("Listening on port %d\n", port);

	while (TRUE)
	{
		struct sockaddr socket_addr;
		socklen_t socket_length;

		SOCKET client_connection = accept(server_socket, &socket_addr, &socket_length);

		if (client_connection == INVALID_SOCKET)
		{
			log("Failed to accept client connection.\n", port);
		}
		else
		{
			HANDLE thread_handle;
			PsCreateSystemThread(&thread_handle, THREAD_ALL_ACCESS, 0, 0, 0, server_thread, &client_connection);
		}
	}

	log("The main thread has terminated...\n");

	closesocket(server_socket);
}

uint64_t handle_ping(data_packet_t packet)
{
	log("Received ping packet! %d\n", packet.data.ping.ping_magic);

	return packet.data.ping.ping_magic + 1;
}

uint64_t handle_copy_memory(data_packet_t packet)
{
	SIZE_T return_size = 0ull;
	PEPROCESS source, target;

	if (NT_SUCCESS(PsLookupProcessByProcessId(packet.data.copy_memory.source_pid, &source)) &&
		NT_SUCCESS(PsLookupProcessByProcessId(packet.data.copy_memory.target_pid, &target)))
	{
		bool source_valid = FALSE;
		bool target_valid = FALSE;

		KAPC_STATE kapc;
		KeStackAttachProcess(source, &kapc);
		{
			source_valid = MmIsAddressValid(packet.data.copy_memory.source_address);
		}
		KeUnstackDetachProcess(&kapc);

		KeStackAttachProcess(target, &kapc);
		{
			target_valid = MmIsAddressValid(packet.data.copy_memory.target_address);
		}
		KeUnstackDetachProcess(&kapc);

		if (target_valid && source_valid)
		{
			MmCopyVirtualMemory(source, (PVOID)packet.data.copy_memory.source_address, target,
				(PVOID)packet.data.copy_memory.target_address, packet.data.copy_memory.size, KernelMode, &return_size);
		}

		ObDereferenceObject(source);
		ObDereferenceObject(target);
	}

	return return_size;
}

uint64_t handle_allocate(data_packet_t packet)
{
	PVOID base_address = 0;
	PEPROCESS target;

	if (NT_SUCCESS(PsLookupProcessByProcessId(packet.data.allocate_memory.target_pid, &target)))
	{
		KAPC_STATE kapc;
		KeStackAttachProcess(target, &kapc);
		{
			ZwAllocateVirtualMemory(ZwCurrentProcess(), &base_address, 0, &packet.data.allocate_memory.size,
				packet.data.allocate_memory.allocation_type, packet.data.allocate_memory.protection);
		}
		KeUnstackDetachProcess(&kapc);

		ObDereferenceObject(target);
	}

	return base_address;
}

uint64_t handle_free(data_packet_t packet)
{
	NTSTATUS result;
	PEPROCESS target;

	if (NT_SUCCESS(PsLookupProcessByProcessId(packet.data.free_memory.target_pid, &target)))
	{
		KAPC_STATE kapc;
		KeStackAttachProcess(target, &kapc);
		{
			result = ZwFreeVirtualMemory(ZwCurrentProcess(), &packet.data.free_memory.target_address,
				&packet.data.free_memory.region_size, MEM_RELEASE);
		}
		KeUnstackDetachProcess(&kapc);

		ObDereferenceObject(target);
	}

	return result;
}

uint64_t handle_protect(data_packet_t packet)
{
	uint64_t result = 0ull;
	PEPROCESS target;

	if (NT_SUCCESS(PsLookupProcessByProcessId(packet.data.protect_memory.target_pid, &target)))
	{
		KAPC_STATE kapc;
		KeStackAttachProcess(target, &kapc);
		{
			ZwProtectVirtualMemory(ZwCurrentProcess(), &packet.data.protect_memory.target_address,
				&packet.data.protect_memory.size, packet.data.protect_memory.new_protection, &result);
		}
		KeUnstackDetachProcess(&kapc);

		ObDereferenceObject(target);
	}

	return result;
}

uint64_t handle_get_base(data_packet_t packet)
{
	uint64_t result = 0ull;
	PEPROCESS target;

	if (NT_SUCCESS(PsLookupProcessByProcessId(packet.data.get_base.target_pid, &target)))
	{
		KAPC_STATE kapc;
		KeStackAttachProcess(target, &kapc);
		{
			PLDR_DATA_TABLE_ENTRY entry = get_module_by_name_x64(target, packet.data.get_base.module_name);

			if (entry)
			{
				result = entry->DllBase;
			}
		}
		KeUnstackDetachProcess(&kapc);
		
		ObDereferenceObject(target);
	}

	return result;
}

uint64_t handle_get_size(data_packet_t packet)
{
	uint64_t result = 0ull;
	PEPROCESS target;

	if (NT_SUCCESS(PsLookupProcessByProcessId(packet.data.get_size.target_pid, &target)))
	{
		KAPC_STATE kapc;
		KeStackAttachProcess(target, &kapc);
		{
			PLDR_DATA_TABLE_ENTRY entry = get_module_by_name_x64(target, packet.data.get_size.module_name);

			if (entry)
			{
				result = entry->SizeOfImage;
			}
		}
		KeUnstackDetachProcess(&kapc);

		ObDereferenceObject(target);
	}

	return result;
}

uint64_t handle_packet(data_packet_t packet)
{
	uint64_t result = 0ull;
	
	switch (packet.header.type)
	{
	case ping:
		result = handle_ping(packet);
		break;

	case packet_copy_memory:
		result = handle_copy_memory(packet);
		break;

	case packet_allocate:
		result = handle_allocate(packet);
		break;

	case packet_free:
		result = handle_free(packet);
		break;

	case packet_protect:
		result = handle_protect(packet);
		break;

	case packet_get_base:
		result = handle_get_base(packet);
		break;

	case packet_get_size:
		result = handle_get_size(packet);
		break;

	default:
		log("Received unknown packet.\n");
		break;
	}

	return result;
}

bool answer_client(SOCKET sockfd, uint64_t result)
{
	data_packet_t packet;
	packet.header.magic_header = magic_header_value;
	packet.header.type = completed;
	packet.data.response.data = result;

	return send(sockfd, &packet, sizeof(packet), 0) != SOCKET_ERROR;
}

void server_thread(SOCKET* sockfd)
{
	log("Connection received, server thread spawned.\n");


	SOCKET connection = *sockfd;
	
	data_packet_t packet;

	while (TRUE)
	{
		int result = recv(connection, (void*)&packet, sizeof(packet), 0);

		if (result > 0 && result >= sizeof(packet.header) && packet.header.magic_header == magic_header_value)
		{
			uint64_t packet_result = handle_packet(packet);

			if (!answer_client(connection, packet_result))
			{
				break;
			}
		}
	}

	log("A server thread has terminated...\n");
}
