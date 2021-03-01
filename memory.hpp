#pragma once
#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>
#include <string>
#include <sstream>
using namespace std;

struct Memory {
protected:
    HANDLE  m_process;
    DWORD   m_pid;

public:
    ~Memory() {
        CloseHandle(m_process);
    }

    template <typename T>
    T read(DWORD address) {
        T read_bytes;
        ReadProcessMemory(m_process, (LPVOID)address, &read_bytes, sizeof(T), 0);
        return read_bytes;
    }

    template <typename T>
    void read(DWORD address, T &buffer) {
        ReadProcessMemory(m_process, (LPVOID)address, &buffer, sizeof(T), 0);
    }

    template <typename T>
    T write(DWORD address, T buffer) {
        WriteProcessMemory(m_process, (LPVOID)address, &buffer, sizeof(buffer), 0);
        return buffer;
    }

    bool get_process(char* name) {
        HANDLE pid = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        PROCESSENTRY32 entry;
        entry.dwSize = sizeof(entry);

        do {
            if (entry.szExeFile == (WCHAR*)name) {
                m_pid = entry.th32ProcessID;
                CloseHandle(pid);
                m_process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, m_pid);
                return true;
            }
        } while (Process32Next(pid, &entry));

        return false;
    }

    bool get_process_by_pid(DWORD pid) {
        m_pid = pid;
        m_process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, m_pid);
        return m_process;
    }

    DWORD get_module(LPSTR name) {
        HANDLE module = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, m_pid);
        MODULEENTRY32 entry;
        entry.dwSize = sizeof(entry);

        do {
            if (entry.szModule == (WCHAR*)name) {
                CloseHandle(module);
                return (DWORD)entry.modBaseAddr;
            }
        } while(Module32Next(module, &entry));

        return 0;
    }

    DWORD get_module(LPSTR name, DWORD& buffer) {
        HANDLE module = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, m_pid);
        MODULEENTRY32 entry;
        entry.dwSize = sizeof(entry);

        do {
            if (entry.szModule == (WCHAR*)name) {
                CloseHandle(module);
                buffer = (DWORD)entry.modBaseAddr;
                return 1;
            }
        } while(Module32Next(module, &entry));

        return 0;
    }
} mem;
