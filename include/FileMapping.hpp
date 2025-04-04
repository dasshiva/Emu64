#pragma once
#include <cstdint>

class FileMapping {
public:
    FileMapping(const char* name);
    ~FileMapping();
    bool     IsValid() { return valid; }
    uint32_t GetError() { return err; }
    void*    GetMappedMemory(); 
    uint64_t Size() { return size; }

private:
    bool valid;
    void* mapdata;
    uint64_t  size = 0;
    uint32_t  err = 0;
};