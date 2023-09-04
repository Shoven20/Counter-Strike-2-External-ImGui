#pragma once
#include <WinSock.h>
#include <cstdint>
#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>
#include <memory>
#include <string_view>
#include <cstdint>
#include <chrono>
#include <vector>

static SOCKET r_socket;
static DWORD r_pid;
static uintptr_t client;

void initdrv();
void clean_socket();
SOCKET connect();
void close_socket(const SOCKET a_socket);
uint32_t read_memory(const SOCKET a_socket, uint32_t process_id, uintptr_t address, uintptr_t buffer, size_t size);
uint32_t write_memory(const SOCKET a_socket, uint32_t process_id, uintptr_t address, uintptr_t buffer, size_t size);
uint64_t process_base_address(const SOCKET a_socket, uint32_t exe_id);
uint64_t dll_oku(const SOCKET a_socket, uint32_t process_id, int module);
std::uint32_t process_id(const std::string& name);

template <typename T>
T read(const std::uintptr_t address)
{
	T buffer{ };
	read_memory(r_socket, r_pid, address, uintptr_t(&buffer), sizeof(T));

	return buffer;
}

template <typename T>
T read_chain(const std::uintptr_t address, std::vector<uintptr_t> chain)
{
	uintptr_t cur_read = address;

	for (int i = 0; i < chain.size() - 1; ++i)
		cur_read = read<uintptr_t>(cur_read + chain[i]);

	return read<T>(cur_read + chain[chain.size() - 1]);
}

template <typename T>
void write(const std::uintptr_t address, const T& buffer)
{
	write_memory(r_socket, r_pid, address, uintptr_t(&buffer), sizeof(T));
}

static void readsize(const uintptr_t address, const void* buffer, const size_t size)
{
	read_memory(r_socket, r_pid, address, uintptr_t(buffer), size);
}


static std::string read_str(std::uintptr_t dst)
{
	char buf[256];
	readsize(dst, &buf, sizeof(buf));
	return buf;
}

#define safe_read(Addr, Type) read<Type>(Addr)
#define safe_write(Addr, Data, Type) write<Type>(Addr, Data)