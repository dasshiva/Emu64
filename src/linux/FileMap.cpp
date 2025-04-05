#include "FileMapping.hpp"

#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <iostream>
struct LinuxMap {
    int   file;
    void* storage;
};

FileMapping::FileMapping(const char* name) {
    valid = false;
    mapdata = new LinuxMap;
    if (!mapdata) 
        return;
    
    struct stat st;
    if (stat(name, &st) < 0) {
	err = errno;
	return;
    }

    struct LinuxMap* map = static_cast<LinuxMap*>(mapdata);
    map->file = openat(AT_FDCWD, name, O_RDONLY | O_CLOEXEC);
    if (map->file < 0) {
        err = errno;
        return;
    }

    map->storage = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, 
		    map->file, 0);
    if (map->storage == MAP_FAILED) {
        err = errno;
        return;
    }

    if ((st.st_size % 4096) != 0) {
	    int rem = st.st_size % 4096;
	    st.st_size += 4096 - rem;
    }

    size = st.st_size;
    valid = true;
    close(map->file); // Linux doesn't need the file descriptor to be open
}

void* FileMapping::GetMappedMemory() {
    if (!valid)
        return nullptr;

    struct LinuxMap* map = static_cast<LinuxMap*>(mapdata);
    return map->storage;
}

FileMapping::~FileMapping() {
    // Don't call the destructor unless the bug is fixed
    return;
    /*
    if (!valid)
	return; */

    /*if (mapdata) {
        struct LinuxMap* mp = static_cast<LinuxMap*>(mapdata); */
        /* Some versions of gcc call this destructor prematurely (I have no idea why)
         * and hence munmap is called causing complete chaos as the underlying
         * file just "vanishes". Don't munmap() unless this is fixed */
        //if (mp->storage)
        //    munmap(mp->storage, size);
        // delete mp;
        // mapdata = nullptr;
    // }

    // valid = false;
}
