#pragma once

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    CHDERR_NONE = 0,
    CHDERR_FILE_NOT_FOUND = 1,
    CHDERR_INVALID_DATA = 2,
} chd_error;

typedef struct chd_file chd_file;

typedef struct chd_header {
    uint32_t hunkbytes;
    uint32_t unitbytes;
    uint64_t unitcount;
} chd_header;

typedef struct core_file core_file;

struct core_file {
    void *argp;
    uint64_t (*fsize)(core_file *);
    int (*fseek)(core_file *, int64_t, int);
    size_t (*fread)(void *, size_t, size_t, core_file *);
    int (*fclose)(core_file *);
};

#define CHD_OPEN_READ 0

chd_error chd_read_header(const char *filename, chd_header *header);
const char *chd_error_string(chd_error err);
chd_error chd_open_core_file(core_file *file, uint32_t mode, void *parent, chd_file **out);
void chd_close(chd_file *file);
const chd_header *chd_get_header(chd_file *file);
chd_error chd_read(chd_file *file, uint32_t hunknum, void *buffer);

#ifdef __cplusplus
}
#endif
