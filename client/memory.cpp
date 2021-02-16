#include "common.hpp"

memory::virtual_controller_t::virtual_controller_t(std::shared_ptr<connection::ksock_t>& server, uint32_t pid, const wchar_t* name)
{
	this->server = server;
	this->pid = pid;
	this->name = std::wstring(name);
}

memory::virtual_controller_t::~virtual_controller_t()
{
}

uint64_t memory::virtual_controller_t::get_base()
{
	return server->get_base(this->pid, this->name.data());
}

uint64_t memory::virtual_controller_t::get_base(const wchar_t* module_name)
{
	return server->get_base(this->pid, module_name);
}

uint64_t memory::virtual_controller_t::allocate(uint32_t protection, uint32_t allocation_type, size_t size)
{
	return this->server->zw_allocate_virtual_memory(this->pid, protection, allocation_type, size);
}

void memory::virtual_controller_t::free(uint64_t target_address, size_t size)
{
	this->server->zw_free_vritual_memory(this->pid, target_address, size);
}

uint64_t memory::virtual_controller_t::protect(uint64_t target_address, uint32_t protection, size_t size)
{
	return this->server->zw_protect_virtual_memory(this->pid, target_address, protection, size);
}
