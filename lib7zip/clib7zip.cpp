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
 */

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cwchar>
#include <algorithm>

#include <lib7zip.h>


#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

wchar_t* wstring2wchar_t(const std::wstring& s){
	/*
	This function is exceptionally verbose.
	wstring::copy appears to be the best bet (versus wsncpy or whatever)
	*/
	auto len = s.length();
	auto lenp1 = len + 1;

	wchar_t* buf = static_cast<wchar_t*>(malloc(sizeof(wchar_t) * lenp1));
	if(!buf){ return NULL; }

	s.copy(buf, len);
	buf[len] = '\0';
	return buf;
}

extern "C"
{
	#include "clib7zip.h"

	//Object
	c7zip_EXPORT void free_c7z_Object(c7z_Object* self) {
		delete static_cast<C7ZipObject*>(self);
	}

	//ObjectPtrArray
	c7zip_EXPORT c7z_ObjPtrArr* create_c7z_ObjPtrArr() {
		return static_cast<c7z_ObjPtrArr*>(new C7ZipObjectPtrArray);
	}

	c7zip_EXPORT c7z_ObjPtrArr* create_c7z_ObjPtrArr1(bool auto_release) {
		return static_cast<c7z_ObjPtrArr*>(new C7ZipObjectPtrArray(auto_release));
	}

	c7zip_EXPORT void free_c7z_ObjPtrArr(c7z_ObjPtrArr* self) {
		delete static_cast<C7ZipObjectPtrArray*>(self);
	}

	c7zip_EXPORT void c7zOPArr_clear(c7z_ObjPtrArr* self) {
		return static_cast<C7ZipObjectPtrArray*>(self)->clear();
	}

	c7zip_EXPORT size_t c7zOPArr_GetSize(c7z_ObjPtrArr* self) {
		return static_cast<C7ZipObjectPtrArray*>(self)->size();
	}

	c7zip_EXPORT c7z_Object* c7zOPArr_GetItemAt(c7z_ObjPtrArr* self, size_t index) {
		return (*static_cast<C7ZipObjectPtrArray*>(self))[index];
	}

	// Archive Item
	c7zip_EXPORT void free_C7ZipArchiveItem(c7z_ArchiveItem* self) {
		delete static_cast<C7ZipArchiveItem*>(self);
	}

	c7zip_EXPORT const wchar_t* c7zItm_GetFullPath(c7z_ArchiveItem* self) {
		return static_cast<C7ZipArchiveItem*>(self)->GetFullPath().c_str();
	}

	c7zip_EXPORT unsigned __int64 c7zItm_GetSize(c7z_ArchiveItem* self) {
		return static_cast<C7ZipArchiveItem*>(self)->GetSize();
	}

	c7zip_EXPORT bool c7zItm_IsDir(c7z_ArchiveItem* self) {
		return static_cast<C7ZipArchiveItem*>(self)->IsDir();
	}

	c7zip_EXPORT bool c7zItm_IsEncrypted(c7z_ArchiveItem* self) {
		return static_cast<C7ZipArchiveItem*>(self)->IsEncrypted();
	}

	c7zip_EXPORT unsigned int c7zItm_GetArchiveIndex(c7z_ArchiveItem* self) {
		return static_cast<C7ZipArchiveItem*>(self)->GetArchiveIndex();
	}

	c7zip_EXPORT bool c7zItm_GetUInt64Property(c7z_ArchiveItem* self, int propertyIndex, unsigned __int64 * const val) {
		return static_cast<C7ZipArchiveItem*>(self)->GetUInt64Property((lib7zip::PropertyIndexEnum)propertyIndex, *val);
	}

	c7zip_EXPORT bool c7zItm_GetFileTimeProperty(c7z_ArchiveItem* self, int propertyIndex, unsigned __int64 * const val) {
		return static_cast<C7ZipArchiveItem*>(self)->GetFileTimeProperty((lib7zip::PropertyIndexEnum)propertyIndex, *val);
	}

	
	c7zip_EXPORT bool c7zItm_GetStringProperty(c7z_ArchiveItem* self, int propertyIndex, wchar_t ** val) {
		wstring _val;
		if(!static_cast<C7ZipArchiveItem*>(self)->GetStringProperty((lib7zip::PropertyIndexEnum)propertyIndex,  _val)) {
			return false;
		}

		return !!(*val = wstring2wchar_t(_val));
	}

	c7zip_EXPORT bool c7zItm_GetBoolProperty(c7z_ArchiveItem* self, int propertyIndex, bool * val) {
		return static_cast<C7ZipArchiveItem*>(self)->GetBoolProperty((lib7zip::PropertyIndexEnum)propertyIndex, *val);
	}

	// InStream

	c7zip_EXPORT const wchar_t* c7zInSt_GetExt(c7z_InStream* self) {
		//return L"7z";
		return static_cast<C7ZipInStream*>(self)->GetExt().c_str();
	}

	c7zip_EXPORT int c7zInSt_Read(c7z_InStream* self, void* data, unsigned int size, unsigned int *processedSize) {
		return static_cast<C7ZipInStream*>(self)->Read(data, size, processedSize);
	}

	c7zip_EXPORT int c7zInSt_Seek(c7z_InStream* self, __int64 offset, unsigned int seekOrigin, unsigned __int64 *newPosition) {
		return static_cast<C7ZipInStream*>(self)->Seek(offset, seekOrigin, newPosition);
	}

	c7zip_EXPORT int c7zInSt_GetSize(c7z_InStream* self, unsigned __int64 * size) {
		return static_cast<C7ZipInStream*>(self)->GetSize(size);
	}

	//Out Streem
	c7zip_EXPORT int c7zOutSt_Write(c7z_OutStream* self, const void *data, unsigned int size, unsigned int *processedSize) {
		return static_cast<C7ZipOutStream*>(self)->Write(data, size, processedSize);
	}

	c7zip_EXPORT int c7zOutSt_Seek(c7z_OutStream* self, __int64 offset, unsigned int seekOrigin, unsigned __int64 *newPosition) {
		return static_cast<C7ZipOutStream*>(self)->Seek(offset, seekOrigin, newPosition);
	}

	c7zip_EXPORT int c7zOutSt_SetSize(c7z_OutStream* self, unsigned __int64 size) {
		return static_cast<C7ZipOutStream*>(self)->SetSize(size);
	}

	//Archive
	
	c7zip_EXPORT void free_C7ZipArchive(c7z_Archive* self){
		delete static_cast<C7ZipArchive*>(self);
	}

	c7zip_EXPORT bool c7zArc_GetItemCount(c7z_Archive* self, unsigned int * pNumItems) {
		return static_cast<C7ZipArchive*>(self)->GetItemCount(pNumItems);
	}

	c7zip_EXPORT bool c7zArc_GetItemInfo(c7z_Archive* self, unsigned int index, c7z_ArchiveItem ** ppArchiveItem) {
		C7ZipArchiveItem* item;

		if(!static_cast<C7ZipArchive*>(self)->GetItemInfo(index, &item)) {
			return false;
		}

		*ppArchiveItem = static_cast<c7z_ArchiveItem*>(item);
		return true;
	}

	c7zip_EXPORT bool c7zArc_ExtractByIndex(c7z_Archive* self, unsigned int index, c7z_OutStream * pOutStream) {
		return static_cast<C7ZipArchive*>(self)->Extract(index, static_cast<C7ZipOutStream*>(pOutStream));
	}

	c7zip_EXPORT bool c7zArc_ExtractByIndexPW(c7z_Archive* self, unsigned int index, c7z_OutStream * pOutStream, const wchar_t* password) {
		const wstring pwd(password);
		return static_cast<C7ZipArchive*>(self)->Extract(index, static_cast<C7ZipOutStream*>(pOutStream), pwd);
	}

	c7zip_EXPORT bool c7zArc_ExtractByItem(c7z_Archive* self, const c7z_ArchiveItem * pArchiveItem, c7z_OutStream * pOutStream) {
		return static_cast<C7ZipArchive*>(self)->Extract(static_cast<const C7ZipArchiveItem*>(pArchiveItem), static_cast<C7ZipOutStream*>(pOutStream));
	}

	c7zip_EXPORT void c7zArc_Close(c7z_Archive* self) {
		static_cast<C7ZipArchive*>(self)->Close();
	}

	c7zip_EXPORT bool c7zArc_GetUInt64Property(c7z_Archive* self, int propertyIndex, unsigned __int64 * const val) {
		return static_cast<C7ZipArchive*>(self)->GetUInt64Property( (lib7zip::PropertyIndexEnum)propertyIndex, *val);
	}

	c7zip_EXPORT bool c7zArc_GetBoolProperty(c7z_Archive* self, int propertyIndex, bool * const val) {
		return static_cast<C7ZipArchive*>(self)->GetBoolProperty( (lib7zip::PropertyIndexEnum)propertyIndex, *val);
	}

	c7zip_EXPORT bool c7zArc_GetStringProperty(c7z_Archive* self, int propertyIndex, wchar_t ** val) {
		wstring _val;
		if(!static_cast<C7ZipArchive*>(self)->GetStringProperty((lib7zip::PropertyIndexEnum)propertyIndex,  _val)) {
			return false;
		}

		return !!(*val = wstring2wchar_t(_val));
	}

	c7zip_EXPORT bool c7zArc_GetFileTimeProperty(c7z_Archive* self, int propertyIndex, unsigned __int64 * const val) {
		return static_cast<C7ZipArchive*>(self)->GetFileTimeProperty( (lib7zip::PropertyIndexEnum)propertyIndex, *val);
	}

	//Library
	c7zip_EXPORT c7z_Library* create_C7ZipLibrary() {
		return static_cast<c7z_Library*>(new C7ZipLibrary());
	}

	c7zip_EXPORT void free_C7ZipLibrary(c7z_Library* self) {
		delete static_cast<C7ZipLibrary*>(self);
	}

	c7zip_EXPORT bool c7zLib_Initialize(c7z_Library* self) {
		return static_cast<C7ZipLibrary*>(self)->Initialize();
	}

	c7zip_EXPORT void c7zLib_Deinitialize(c7z_Library* self) {
		return static_cast<C7ZipLibrary*>(self)->Deinitialize();
	}

	c7zip_EXPORT bool c7zLib_GetSupportedExts(c7z_Library* self, const wchar_t*** exts, size_t * size) {
		if(!exts){ return false; } //TODO set arg error
		WStringArray in;
		const bool r = static_cast<C7ZipLibrary*>(self)->GetSupportedExts(in);
		if(!r){
			return false;
		}
	
		*size = in.size();
		*exts = static_cast<const wchar_t**>(malloc( sizeof(wchar_t*) * in.size() ));
		if(!*exts){ return false; } // TODO set mem error

		WStringArray::const_iterator it;
		unsigned int i;
		for(it = in.begin(), i = 0; it < in.end(); it++, i++){
			(*exts)[i] = it->c_str();
		}

		return true;
	}

	c7zip_EXPORT bool c7zLib_OpenArchive(c7z_Library* self, c7z_InStream* pInStream, c7z_Archive ** ppArchive) {
		C7ZipArchive* archive;
		bool r = static_cast<C7ZipLibrary*>(self)->OpenArchive(static_cast<C7ZipInStream*>(pInStream), &archive);
		*ppArchive = static_cast<c7z_Archive*>(archive);
		return r;
	}

	//TODO: const C7ZipObjectPtrArray * c7zLib_GetInternalObjectsArray();

	c7zip_EXPORT bool c7zLib_IsInitialized(c7z_Library* self) {
		return static_cast<C7ZipLibrary*>(self)->IsInitialized();
	}

	c7zip_EXPORT ErrorCodeEnum c7zLib_GetLastError(c7z_Library* self) {
		return static_cast<ErrorCodeEnum>( static_cast<C7ZipLibrary*>(self)->GetLastError() );
	}

	c7zip_EXPORT void free_extarr(const wchar_t** exts){ free(exts); }

	c7zip_EXPORT void free_C7ZipInStream(c7z_InStream* stream){
		delete static_cast<C7ZipInStream*>(stream);
	}
}//end extern "C"
