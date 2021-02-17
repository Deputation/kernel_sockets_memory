#include "common.hpp"

int main(void)
{
	auto ksock = std::make_shared<connection::ksock_t>(55123);
	log("KSOCK connected on port %d\n", ksock->port);
	auto process = std::make_shared<memory::virtual_controller_t>(ksock, utils::get_process_id("client.exe"), L"client.exe");

	log("Testing ksock connection using the virtual controller.\n");

	auto secret = 0x1337u;

	log("Base address (main module): %p\n", process->get_base());
	log("Size (main module): %p\n", process->get_size());
	log("Base address (ntdll.dll): %p\n", process->get_base(L"ntdll.dll"));
	log("Size (ntdll.dll): %p\n", process->get_size(L"ntdll.dll"));
	log("Read (reading secret): %p\n", process->read<uint32_t>(reinterpret_cast<uint64_t>(&secret)));
	log("Write (changing secret): %p\n", process->write<uint32_t>(reinterpret_cast<uint64_t>(&secret), 0xCAFEBABE));
	log("Read 2 (reading secret): %p\n", process->read<uint32_t>(reinterpret_cast<uint64_t>(&secret)));

	auto region = process->allocate(PAGE_EXECUTE_READWRITE, MEM_COMMIT | MEM_RESERVE, 0x1000);
	log("Allocation: %p\n", region);

	// The two values should be the same, since the old protection was PAGE_EXECUTE_READWRITE
	log("Protection: %p - %p\n", process->protect(region, PAGE_READWRITE, 0x1000), PAGE_EXECUTE_READWRITE);
	
	process->free(region, 0x1000);
	ksock->disconnect();

	// So you can read results even if you double clicked.
	std::cin.get();

	return 0;
}