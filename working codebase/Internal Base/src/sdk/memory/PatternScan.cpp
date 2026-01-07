#include "PatternScan.h"
#include <Windows.h>
#include <Psapi.h>
#include <vector>

uintptr_t Memory::GetModuleBase(const char* module)
{
    return reinterpret_cast<uintptr_t>(GetModuleHandleA(module));
}

static std::vector<int> PatternToBytes(const char* pattern)
{
    std::vector<int> bytes;

    for (const char* c = pattern; *c; ++c)
    {
        if (*c == '?')
        {
            ++c;
            if (*c == '?') ++c;
            bytes.push_back(-1);
        }
        else
        {
            bytes.push_back(strtoul(c, const_cast<char**>(&c), 16));
        }
    }

    return bytes;
}

uintptr_t Memory::PatternScan(const char* module, const char* signature)
{
    HMODULE mod = GetModuleHandleA(module);
    if (!mod) return 0;

    MODULEINFO info{};
    if (!GetModuleInformation(GetCurrentProcess(), mod, &info, sizeof(info)))
        return 0;

    auto base = reinterpret_cast<uint8_t*>(mod);
    auto size = info.SizeOfImage;

    auto pattern = PatternToBytes(signature);
    auto data = pattern.data();
    auto len = pattern.size();

    for (size_t i = 0; i < size - len; ++i)
    {
        bool found = true;
        for (size_t j = 0; j < len; ++j)
        {
            if (data[j] != -1 && data[j] != base[i + j])
            {
                found = false;
                break;
            }
        }
        if (found)
            return reinterpret_cast<uintptr_t>(&base[i]);
    }

    return 0;
}