/**
 *
 * Copyright (c) 2012, Mark Harviston <mark.harviston@gmail.com>
 * This is free software, most forms of redistribution and derivitive works are permitted with the following restrictions.
 *
 * Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Conventions of this API:
 * ------------------------
 *
 * C-Wrapper for the C++ API lib7zip
 * which is in turn a wrapper over 7z.so/7z.dll which is a C API...
 * but it uses windows COM+ conventions and is awkward to use on non-windows systems,
 * actually also on windows.
 *
 * File extensions are given w/o the e.g. "7z" not ".7z".
 *
 * Functions that return a bool, return true for success and false for failure.
 * Functions that return an int, return 0 for success and non-zero for failure.
 *
 * Most pointers are "borrowed" so you don't need to free() them, those that do are marked..
 * Many lets you allocate an item on the stack and then pass in a pointer to that variable..
 *
 * This API is beta, be sure to check for memory leaks with valgrind.
 *
 */

#pragma once

#include "PropID.h"

#if !__cplusplus
typedef int bool;
#define false 0
#define true 1
#endif
#include <wchar.h>

#ifndef _WIN32

#define __int64 long long int
#endif

#define c7zip_EXPORT

/**
 * Items that return bool, true == success, false== failure
 * Items that return int (as opposed to unsigned int, or __int64), 0 == success, non-zero == failure
 */

typedef enum
{
	ErrorCode_Begin,

	NO_ERROR = ErrorCode_Begin,
	UNKNOWN_ERROR,
	NOT_INITIALIZE,
	NEED_PASSWORD,
	NOT_SUPPORTED_ARCHIVE,

	ErrorCode_End
} ErrorCodeEnum;

//Types
typedef void c7z_Object;
typedef void c7z_ObjPtrArr;// Object Pointer Array, aka a list of c7z_Objects/C7ZipObjects
typedef void c7z_ArchiveItem;
typedef void c7z_InStream;
typedef void c7z_MultiVolume;
typedef void c7z_OutStream;
typedef void c7z_Archive;
typedef void c7z_Library;

//Object

//ArchiveItem
///free a c7z_ArchiveItem pointer
c7zip_EXPORT void free_C7ZipArchiveItem(c7z_ArchiveItem* self);

///Borrowed Pointer, no need to deallocate
c7zip_EXPORT const wchar_t* c7zItm_GetFullPath(c7z_ArchiveItem* self);
c7zip_EXPORT unsigned __int64 c7zItm_GetSize(c7z_ArchiveItem* self);
c7zip_EXPORT bool c7zItm_IsDir(c7z_ArchiveItem* self);
c7zip_EXPORT bool c7zItm_IsEncrypted(c7z_ArchiveItem* self);
c7zip_EXPORT unsigned int c7zItm_GetArchiveIndex(c7z_ArchiveItem* self);
c7zip_EXPORT bool c7zItm_GetUInt64Property(c7z_ArchiveItem* self, int propertyIndex, unsigned __int64 * val);
c7zip_EXPORT bool c7zItm_GetFileTimeProperty(c7z_ArchiveItem* self, int propertyIndex, unsigned __int64 * val);
c7zip_EXPORT bool c7zItm_GetStringProperty(c7z_ArchiveItem* self, int propertyIndex, wchar_t ** val);
c7zip_EXPORT bool c7zItm_GetBoolProperty(c7z_ArchiveItem* self, int propertyIndex, bool * val);

//InStream
c7zip_EXPORT c7z_InStream* create_c7zInSt_Filename(const char* filename);
c7zip_EXPORT void free_C7ZipInStream(c7z_InStream* stream);
c7zip_EXPORT const wchar_t* c7zInSt_GetExt(c7z_InStream* self);

c7zip_EXPORT int c7zInSt_Read(c7z_InStream* self, void *data, unsigned int size, unsigned int *processedSize);
c7zip_EXPORT int c7zInSt_Seek(c7z_InStream* self, __int64 offset, unsigned int seekOrigin, unsigned __int64 *newPosition);
c7zip_EXPORT int c7zInSt_GetSize(c7z_InStream* self, unsigned __int64 * size);

//OutStream
c7zip_EXPORT int c7zOutSt_Write(c7z_OutStream* self, const void *data, unsigned int size, unsigned int *processedSize);
c7zip_EXPORT int c7zOutSt_Seek(c7z_OutStream* self, __int64 offset, unsigned int seekOrigin, unsigned __int64 *newPosition);
c7zip_EXPORT int c7zOutSt_SetSize(c7z_OutStream* self, unsigned __int64 size);

//Archive
c7zip_EXPORT void free_C7ZipArchive(c7z_Archive* self);

c7zip_EXPORT bool c7zArc_GetItemCount(c7z_Archive* self, unsigned int * pNumItems);

c7zip_EXPORT bool c7zArc_GetItemInfo(c7z_Archive* self, unsigned int index, c7z_ArchiveItem ** ppArchiveItem);
c7zip_EXPORT bool c7zArc_ExtractByIndex(c7z_Archive* self, unsigned int index, c7z_OutStream * pOutStream);
c7zip_EXPORT bool c7zArc_ExtractByIndexPW(c7z_Archive* self, unsigned int index, c7z_OutStream * pOutStream, const wchar_t* password);
c7zip_EXPORT bool c7zArc_ExtractByItem(c7z_Archive* self, const c7z_ArchiveItem * pArchiveItem, c7z_OutStream * pOutStream);

c7zip_EXPORT void c7zArc_Close(c7z_Archive* self);//frees the pointer

c7zip_EXPORT bool c7zArc_GetUInt64Property(c7z_Archive* self, int propertyIndex, unsigned __int64 * const val);
c7zip_EXPORT bool c7zArc_GetBoolProperty(c7z_Archive* self, int propertyIndex, bool * const val);
c7zip_EXPORT bool c7zArc_GetStringProperty(c7z_Archive* self, int propertyIndex, wchar_t ** val);
c7zip_EXPORT bool c7zArc_GetFileTimeProperty(c7z_Archive* self, int propertyIndex, unsigned __int64 * const val);

//Library
c7zip_EXPORT c7z_Library* create_C7ZipLibrary();
c7zip_EXPORT void free_C7ZipLibrary(c7z_Library* self);
c7zip_EXPORT bool c7zLib_Initialize(c7z_Library* self);
c7zip_EXPORT void c7zLib_Deinitialize(c7z_Library* self);
c7zip_EXPORT bool c7zLib_GetSupportedExts(c7z_Library* self, const wchar_t *** exts, size_t * size);
c7zip_EXPORT void free_extarr(const wchar_t** exts);
c7zip_EXPORT bool c7zLib_OpenArchive(c7z_Library* self, c7z_InStream* pInStream, c7z_Archive ** ppArchive);
c7zip_EXPORT bool c7zLib_IsInitialized(c7z_Library* self);
c7zip_EXPORT ErrorCodeEnum c7zLib_GetLastError(c7z_Library* self);
