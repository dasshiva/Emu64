#include "FileMapping.hpp"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <fileapi.h>
#include <winbase.h>
#include <memoryapi.h>
#include <winnt.h>

struct WindowsMap {
    HANDLE file;
    HANDLE fileview;
    LPVOID storage;
};

FileMapping::FileMapping(const char* name) {
    valid = false;
    mapdata = new WindowsMap;
    if (!mapdata) 
        return;
    
    struct WindowsMap* map = static_cast<WindowsMap*>(mapdata);
    map->file = CreateFileA(name, GENERIC_READ, 0, nullptr, OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL, nullptr);
    if (map->file == INVALID_HANDLE_VALUE) {
        err = GetLastError();
        return;
    }

    map->fileview = CreateFileMappingA(map->file, nullptr, PAGE_READONLY,
        0, 0, nullptr);
    if (map->fileview == nullptr) {
        err = GetLastError();
        return;
    }

    map->storage = MapViewOfFile(map->fileview, FILE_MAP_READ, 0, 0, 0);
    if (!map->storage) {
        err = GetLastError();
        return;
    }

    char* t = static_cast<char*>(map->storage);

    MEMORY_BASIC_INFORMATION info;
    VirtualQuery(map->storage, &info, sizeof(MEMORY_BASIC_INFORMATION));
    size = info.RegionSize;
    valid = true;
}

void* FileMapping::GetMappedMemory() {
    if (!valid)
        return nullptr;
    struct WindowsMap* map = static_cast<WindowsMap*>(mapdata);
    return map->storage;
}

FileMapping::~FileMapping() {
    if (!valid)
	return;

    if (mapdata) {
        struct WindowsMap* mp = static_cast<WindowsMap*>(mapdata);
        if (mp->storage)
            UnmapViewOfFile(mp->storage);
        if (mp->fileview)
            CloseHandle(mp->fileview);
        if (mp->file)
            CloseHandle(mp->file);
        //delete mp;
        mapdata = nullptr;
    }
    valid = false;
}
