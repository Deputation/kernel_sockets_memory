#pragma once

namespace memory
{
	class virtual_controller_t
	{
	protected:
		std::shared_ptr<connection::ksock_t> server;

	public:
		uint32_t pid;
		std::wstring name;

		/// <summary>
		/// Instantiate the virtual memory controller.
		/// </summary>
		/// <param name="server"> A ksock_t shared instance. </param>
		/// <param name="pid"> The process id you want the virtual controller to bind to. </param>
		/// <param name="name"> Process executable's name. </param>
		virtual_controller_t(std::shared_ptr<connection::ksock_t>& server, uint32_t pid, const wchar_t* name);

		/// <summary>
		/// Destructor. Unimplemented as of now.
		/// </summary>
		~virtual_controller_t();

		/// <summary>
		/// Get the process' main module base.
		/// </summary>
		/// <returns> The main module's base. </returns>
		uint64_t get_base();

		/// <summary>
		/// Get a process module's base address.
		/// </summary>
		/// <param name="module_name"> The module name of which you want to retrieve the base address. </param>
		/// <returns> The base address of the specified module. </returns>
		uint64_t get_base(const wchar_t* module_name);

		/// <summary>
		/// Write to the process' virtual memory.
		/// </summary>
		/// <typeparam name="t"> The data type of the data you want to write. </typeparam>
		/// <param name="address"> The address you want the data to be written to. </param>
		/// <param name="value"> The data you want to write. </param>
		/// <returns> The amount of bytes written. </returns>
		template<typename t>
		inline size_t write(uint64_t address, t value)
		{
			auto buffer = value;

			return this->server->mm_copy_virtual_memory(GetCurrentProcessId(),
				reinterpret_cast<uint64_t>(&buffer), this->pid, address, sizeof(t));
		}

		/// <summary>
		/// Reads virtual memory from the process.
		/// </summary>
		/// <typeparam name="t"> The data type located at the address. </typeparam>
		/// <param name="address"> The address that you want to read. </param>
		/// <returns>The data read.</returns>
		template<typename t>
		inline t read(uint64_t address)
		{
			t buffer;

			this->server->mm_copy_virtual_memory(this->pid, address, GetCurrentProcessId(),
				reinterpret_cast<uint64_t>(&buffer), sizeof(t));

			return buffer;
		}

		/// <summary>
		/// Issue a kernel memory allocation. Pages will have the specified allocation type and protection.
		/// </summary>
		/// <param name="protection"> The pages' protection. </param>
		/// <param name="allocation_type"> The allocation's type. </param>
		/// <param name="size"> The allocation's size. </param>
		/// <returns> The base address of the allocated memory zone. </returns>
		uint64_t allocate(uint32_t protection, uint32_t allocation_type, size_t size);
		
		/// <summary>
		/// Frees memory with the MEM_RELEASE free type.
		/// </summary>
		/// <param name="target_address"> The base adddress of the allocation to free. </param>
		/// <param name="size"> The size of the memory allocation to free. </param>
		void free(uint64_t target_address, size_t size);
		
		/// <summary>
		/// Changes a memory zone's protection.
		/// </summary>
		/// <param name="target_address"> The address of which the protection will be changed. (Affects an entire
		/// page), standard PAGE_SIZE is 0x1000. </param>
		/// <param name="protection"> The new protection. </param>
		/// <param name="size"> The amount of bytes to be affected, starting from the specified target address. </param>
		/// <returns> The old protection. </returns>
		uint64_t protect(uint64_t target_address, uint32_t protection, size_t size);
	};
}

