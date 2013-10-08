#ifndef WRAP_H
#define WRAP_H

#include "clib7zip.h"

#ifdef __cplusplus 
extern "C" { 
#endif

bool lib7zip_open_archive(void *rp, c7z_Library *l, c7z_InStream **cr, c7z_Archive **a);
bool lib7zip_close_archive(c7z_InStream *cr, c7z_Archive *a);
bool lib7zip_archive_extract(c7z_Archive *a, unsigned int i, void *wp);
bool lib7zip_archive_extract_password(c7z_Archive *a, unsigned int i, void *wp, wchar_t *ws);
bool lib7zip_item_extract(c7z_Archive *a, c7z_ArchiveItem *i, void *wp);

char * lib7zip_toString(wchar_t *s);
wchar_t * lib7zip_wchar_array(wchar_t **wss, int i);

#ifdef __cplusplus 
} /* extern "C" */ 
#endif

#endif
