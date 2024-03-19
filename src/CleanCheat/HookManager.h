#pragma once
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN

#ifdef CLEANCHEAT_MINHOOK_PATH
#include CLEANCHEAT_MINHOOK_PATH
#else
#include "Libs/MinHook/MinHook.h"
#endif

#if defined _M_X64
#pragma comment(lib, __FILE__ "\\..\\Libs\\MinHook\\libMinHook-x64-v143-md.lib")
#elif defined _M_IX86
#pragma comment(lib, __FILE__ "\\..\\Libs\\MinHook\\libMinHook-x86-v143-md.lib")
#endif

#include <algorithm>
#include <unordered_map>
#include <Windows.h>

class HookManager final
{
private:
    std::unordered_map<LPVOID, LPVOID> _detours;
    std::unordered_map<void*, std::vector<std::pair<int32_t, void*>>> _vmtSwaps;

public:
    bool isInitialized;

public:
    void SwapVmt(void* instance, const int32_t vmtIndex, void* hkFunc, void** outOriginalFunc)
    {
        if (_vmtSwaps.find(instance) == _vmtSwaps.end())
            _vmtSwaps.emplace(instance, std::vector<std::pair<int32_t, void*>>());

        void** vmTable = *static_cast<void***>(instance);
        _vmtSwaps[instance].push_back(std::make_pair(vmtIndex, vmTable[vmtIndex]));

        if (outOriginalFunc)
            *outOriginalFunc = vmTable[vmtIndex];

        DWORD virtualProtect;
        VirtualProtect(&vmTable[vmtIndex], 0x8, PAGE_EXECUTE_READWRITE, &virtualProtect);
        vmTable[vmtIndex] = hkFunc;
        VirtualProtect(&vmTable[vmtIndex], 0x8, virtualProtect, &virtualProtect);
    }

    void UnSwapVmt(void* instance, const int32_t vmtIndex, void* originalFunc)
    {
        void** vmTable = *static_cast<void***>(instance);

        std::vector<std::pair<int, void*>>::iterator itToRemove = std::remove_if(
            _vmtSwaps[instance].begin(),
            _vmtSwaps[instance].end(),
            [=](const std::pair<int32_t, void*> v)
            {
                return v.first == vmtIndex;
            });
        _vmtSwaps[instance].erase(itToRemove, _vmtSwaps[instance].end());
        if (_vmtSwaps[instance].empty())
            _vmtSwaps.erase(instance);

        DWORD virtualProtect;
        VirtualProtect(&vmTable[vmtIndex], 0x8, PAGE_EXECUTE_READWRITE, &virtualProtect);
        vmTable[vmtIndex] = originalFunc;
        VirtualProtect(&vmTable[vmtIndex], 0x8, virtualProtect, &virtualProtect);
    }

    void UnSwapAll()
    {
        for (std::pair<void* const, std::vector<std::pair<int, void*>>>& swap : _vmtSwaps)
        {
            void* instance = swap.first;
            for (std::pair<int, void*>& instanceSwap : swap.second)
            {
                int32_t vmtIndex = instanceSwap.first;
                void* originalFunc = instanceSwap.second;

                UnSwapVmt(instance, vmtIndex, originalFunc);
            }
        }
    }

    MH_STATUS Init()
    {
        MH_STATUS ret = MH_Initialize();
        if (ret == MH_OK) isInitialized = true;

        return ret;
    }

    MH_STATUS Uninit()
    {
        MH_STATUS ret = MH_Uninitialize();
        if (ret == MH_OK) isInitialized = false;

        return ret;
    }

    MH_STATUS Detour(LPVOID pTarget, LPVOID pDetour, LPVOID* ppOriginal)
    {
        MH_STATUS ret = MH_CreateHook(pTarget, pDetour, ppOriginal);
        if (ret == MH_OK) _detours.emplace(pTarget, pDetour);

        return ret;
    }

    MH_STATUS UnDetour(LPVOID pTarget)
    {
        _detours.erase(pTarget);

        return MH_RemoveHook(pTarget);
    }

    void UnDetourAll()
    {
        for (auto& detour : _detours)
        {
            if (UnDetour(detour.first) != MH_OK)
                CLEANCHEAT_LOG("ERROR: An error occurred while attempting to unhook");
        }
    }
};
