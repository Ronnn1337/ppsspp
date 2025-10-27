#include <cstddef>

#include "ext/libkirk/amctrl.h"
#include "ext/libkirk/kirk_engine.h"
#include "libchdr/chd.h"

extern "C" {

int kirk_init(KirkState *) { return 0; }
int sceDrmBBMacInit(MAC_KEY *, int) { return 0; }
int sceDrmBBMacUpdate(KirkState *, MAC_KEY *, u8 *, int) { return 0; }
int sceDrmBBMacFinal(KirkState *, MAC_KEY *, u8 *, u8 *) { return 0; }
int sceDrmBBMacFinal2(KirkState *, MAC_KEY *, u8 *, u8 *) { return 0; }
int bbmac_getkey(KirkState *, MAC_KEY *, u8 *, u8 *) { return 0; }
int sceDrmBBCipherInit(KirkState *, CIPHER_KEY *, int, int, u8 *, u8 *, u32) { return 0; }
int sceDrmBBCipherUpdate(KirkState *, CIPHER_KEY *, u8 *, int) { return 0; }
int sceDrmBBCipherFinal(CIPHER_KEY *) { return 0; }

int sceNpDrmGetFixedKey(KirkState *, u8 *, char *, int) { return 0; }

PGD_DESC *pgd_open(KirkState *, u8 *, int, u8 *) { return nullptr; }
int pgd_decrypt_block(KirkState *, PGD_DESC *, int) { return 0; }
int pgd_close(PGD_DESC *) { return 0; }

const char *chd_error_string(chd_error) { return "stub"; }
chd_error chd_read_header(const char *, chd_header *) { return CHDERR_FILE_NOT_FOUND; }
chd_error chd_open_core_file(core_file *, uint32_t, void *, chd_file **) { return CHDERR_FILE_NOT_FOUND; }
void chd_close(chd_file *) {}
const chd_header *chd_get_header(chd_file *) { return nullptr; }
chd_error chd_read(chd_file *, uint32_t, void *) { return CHDERR_FILE_NOT_FOUND; }

}
