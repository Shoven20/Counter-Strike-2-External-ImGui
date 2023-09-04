#include "Driver.h"
#include "Defs.h"

using namespace std;

#pragma comment(lib, "Ws2_32")

static bool send_packet(const SOCKET a_socket, const Packet& packet, uint64_t& out_result)
{
	Packet completion_packet{ };

	if (send(a_socket, (const char*)&packet, sizeof(Packet), 0) == SOCKET_ERROR)
		return false;

	const auto result = recv(a_socket, (char*)&completion_packet, sizeof(Packet), 0);
	if (result < sizeof(PacketHeader) ||
		completion_packet.header.magic != packet_magic ||
		completion_packet.header.type != PacketType::packet_completed)
		return false;

	out_result = completion_packet.data.completed.result;
	return true;
}

static uint32_t copy_memory(const SOCKET a_socket, const uint32_t src_process_id, const uintptr_t src_address, const uint32_t dest_process_id, const uintptr_t dest_address, const size_t size)
{
	Packet packet{ };

	packet.header.magic = packet_magic;
	packet.header.type = PacketType::packet_copy_memory;

	auto& data = packet.data.copy_memory;
	data.src_process_id = src_process_id;
	data.src_address = uint64_t(src_address);
	data.dest_process_id = dest_process_id;
	data.dest_address = uint64_t(dest_address);
	data.size = uint64_t(size);

	uint64_t result = 0;
	if (send_packet(a_socket, packet, result))
		return uint32_t(result);

	return 0;
}

void initdrv()
{
	WSADATA wsa_data;
	WSAStartup(MAKEWORD(2, 2), &wsa_data);
}

void clean_socket()
{
	WSACleanup();
}


SOCKET connect()
{
	SOCKADDR_IN address{ };

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = htonl(server_ip);
	address.sin_port = htons(server_port);

	const auto connection = socket(AF_INET, SOCK_STREAM, 0);
	if (connection == INVALID_SOCKET)
		return INVALID_SOCKET;

	if (connect(connection, (SOCKADDR*)&address, sizeof(address)) == SOCKET_ERROR)
	{
		closesocket(connection);
		return INVALID_SOCKET;
	}
	return connection;
}

void close_socket(const SOCKET a_socket)
{
	closesocket(a_socket);
}


uint32_t read_memory(const SOCKET a_socket, const uint32_t process_id, const uintptr_t address, const uintptr_t buffer, const size_t size)
{
	return copy_memory(a_socket, process_id, address, GetCurrentProcessId(), buffer, size);
}

uint32_t write_memory(const SOCKET a_socket, const uint32_t process_id, const uintptr_t address, const uintptr_t buffer, const size_t size)
{
	return copy_memory(a_socket, GetCurrentProcessId(), buffer, process_id, address, size);
}

uint64_t process_base_address(const SOCKET a_socket, const uint32_t exe_id)
{
	Packet packet{ };
	packet.header.magic = packet_magic;
	packet.header.type = PacketType::packet_get_base_address;

	auto& data = packet.data.get_base_address;
	data.exe_id = exe_id;

	uint64_t result = 0;
	if (send_packet(a_socket, packet, result))
		return result;

	return 0;
}

uint64_t dll_oku(const SOCKET a_socket, const uint32_t process_id, int module)
{
	Packet packet{ };
	packet.header.magic = packet_magic;
	packet.header.type = PacketType::packet_get_dll_address;
	auto& data = packet.data.get_dll_address;
	data.process_id = process_id;
	data.name = module;

	uint64_t result = 0;
	if (send_packet(a_socket, packet, result))
		return result;

	return 0;
}

std::uint32_t process_id(const std::string& name)
{
	const auto snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (snap == INVALID_HANDLE_VALUE) {
		return 0;
	}

	PROCESSENTRY32 proc_entry{};
	proc_entry.dwSize = sizeof proc_entry;

	auto found_process = false;
	if (!!Process32First(snap, &proc_entry)) {
		do {
			if (name == proc_entry.szExeFile) {
				found_process = true;
				break;
			}
		} while (!!Process32Next(snap, &proc_entry));
	}

	CloseHandle(snap);
	return found_process
		? proc_entry.th32ProcessID
		: 0;
}