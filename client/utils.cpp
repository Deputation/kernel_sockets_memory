#include "common.hpp"

uint32_t utils::get_process_id(const char* exe_name)
{
	auto snapshot_handle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (snapshot_handle == INVALID_HANDLE_VALUE)
	{
		return 0;
	}

	PROCESSENTRY32 pe = { 0 };
	pe.dwSize = sizeof(PROCESSENTRY32);

	for (auto success = Process32First(snapshot_handle, &pe); success == true; success = Process32Next(snapshot_handle, &pe))
	{
		auto wide_name = std::wstring(pe.szExeFile);
		auto ascii_name = std::string(wide_name.begin(), wide_name.end());

		if (!ascii_name.compare(exe_name))
		{
			CloseHandle(snapshot_handle);
			return pe.th32ProcessID;
		}
	}

	CloseHandle(snapshot_handle);
	return NULL;
}
