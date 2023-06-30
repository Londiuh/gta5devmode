#include "memory/process.hpp"

#include <iostream>

int main()
{
	std::cout << "Finding GTA5.exe process..." << std::endl;
	Process gtav_process(L"GTA5.exe");

	DWORD pid = gtav_process.get_pid();

	if (!pid)
	{
		std::cerr << "Unable to find GTA5.exe process. Is the game running?" << std::endl;
		return 1;
	}

	std::cout << "Process found with PID " << pid << std::endl;
	std::cout << "Finding DLC::IS_DLC_PRESENT address..." << std::endl;
	// Find DLC::IS_DLC_PRESENT native address searching for it's pattern (48 89 5C 24 ? 57 48 83 EC 20 81 F9)
	uintptr_t address_dlc_present = gtav_process.find_pattern({0x48, 0x89, 0x5C, 0x24, 0x00, 0x57, 0x48, 0x83, 0xEC, 0x20, 0x81, 0xF9});

	if (!address_dlc_present)
	{
		std::cerr << "Pattern not found :(" << std::endl;
		return 1;
	}

	std::cout << "Pattern found at address " << address_dlc_present << std::endl;
	std::cout << "Overwritting native..." << std::endl;

	// Overwrite the DLC::IS_DLC_PRESENT native to always return true
	// mov al, 1
	// ret
	if (!gtav_process.write<std::vector<uint8_t>>(address_dlc_present, {0xB0, 0x01, 0xC3}))
	{
		std::cerr << "WriteProcessMemory failed" << std::endl;
	}

	std::cout << "Finished" << std::endl;
	return 0;
}
