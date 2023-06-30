#include "process.hpp"

DWORD Process::find_pid_by_name(LPCWSTR process_name) const
{
	PROCESSENTRY32W entry;
	entry.dwSize = sizeof(PROCESSENTRY32W);
	DWORD pid = 0;
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (Process32FirstW(snapshot, &entry))
	{
		while (Process32NextW(snapshot, &entry))
		{
			if (wcscmp(entry.szExeFile, process_name) == 0)
			{
				pid = entry.th32ProcessID;
				break;
			}
		}
	}

	CloseHandle(snapshot);
	return pid;
}

Process::Process(LPCWSTR process_name)
{
	pid = find_pid_by_name(process_name);

	if (pid)
	{
		handle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE, FALSE, pid);

		HMODULE modules[255];
		MODULEINFO module_info;

		EnumProcessModules(handle, modules, sizeof(modules), nullptr);
		GetModuleInformation(handle, modules[0], &module_info, sizeof(module_info));

		module_base_address = reinterpret_cast<uintptr_t>(module_info.lpBaseOfDll);
		module_size = module_info.SizeOfImage;
	}
}

Process::~Process()
{
	CloseHandle(handle);
}

DWORD Process::get_pid()
{
	return pid;
}

uintptr_t Process::find_pattern(std::vector<uint8_t> byte_pattern)
{
	std::unique_ptr<uint8_t[]> buffer = std::make_unique<uint8_t[]>(module_size);

	if (!ReadProcessMemory(handle, reinterpret_cast<void*>(module_base_address), buffer.get(), module_size, nullptr))
	{
		return 0;
	}

	for (uintptr_t i = 0; i < module_size; i++)
	{
		for (uintptr_t j = 0; j < byte_pattern.size(); j++)
		{
			if (byte_pattern.at(j) == 0x00)
			{
				continue;
			}
			if (*reinterpret_cast<uint8_t*>(reinterpret_cast<uintptr_t>(&buffer[i + j])) == byte_pattern.at(j))
			{
				if (j == byte_pattern.size() - 1)
				{
					return module_base_address + i;
				}
				continue;
			}
			break;
		}
	}

	return 0;
}
