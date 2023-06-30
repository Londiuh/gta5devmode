#pragma once

#include <windows.h>
#include <tlhelp32.h>
#include <psapi.h>
#include <vector>
#include <span>
#include <memory>

class Process
{
private:
	DWORD pid;
	HANDLE handle;
	uintptr_t module_base_address;
	DWORD module_size;

	DWORD find_pid_by_name(LPCWSTR process_name) const;

public:
	Process(LPCWSTR process_name);
	~Process();
	DWORD get_pid();
	uintptr_t find_pattern(std::vector<uint8_t> pattern);

	template <class T>
	bool write(uintptr_t address, const T& data)
	{
		const void* data_ptr = reinterpret_cast<const void*>(std::data(data));
		SIZE_T data_size = std::size(data) * sizeof(typename std::remove_pointer_t<decltype(std::data(data))>);

		return WriteProcessMemory(handle, reinterpret_cast<void*>(address), data_ptr, data_size, nullptr);
	}
};
