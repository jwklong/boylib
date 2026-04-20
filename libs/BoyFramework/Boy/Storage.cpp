#include "Storage.h"

#include "BoyLib/UString.h"
#include <stdio.h>

using namespace Boy;

Storage::Storage()
{
    mFileKey = 0;
}

Storage::StorageResult Storage::FileOpen( const char *pFilePathUtf8, int modeFlags, BoyFileHandle *pFileHandleOut )
{
	StorageResult result = STORAGE_FAIL;

	// must specify a read/write and a create/open flag
	// must specify a handle to set
	if( (modeFlags & STORAGE_MODE_MASK) && (modeFlags & STORAGE_DISPO_MASK) && pFileHandleOut )
	{
		const char *pModeStr = NULL;
		switch( modeFlags )
		{
			case STORAGE_MODE_READ | STORAGE_MUST_EXIST:
				pModeStr = "rb";
				break;

			case STORAGE_MODE_WRITE | STORAGE_MUST_EXIST:
				pModeStr = "r+b";
				break;

			case STORAGE_MODE_WRITE | STORAGE_OPEN_ALWAYS:
				pModeStr = "w+b";
				break;
		}

		// check for bad flag combo
		if( pModeStr )
		{
            FILE *f = NULL;
            f = fopen( pFilePathUtf8, pModeStr );
            if( f != NULL )
            {
                ++mFileKey;
                mOpenFiles[ mFileKey ] = f;
                *pFileHandleOut = (BoyFileHandle)mFileKey;
                result = STORAGE_OK;
            }
		}
	}
	
	return result;
}

Storage::StorageResult Storage::FileRead( BoyFileHandle fileHandle, void *pBuffer, int readSizeBytes )
{
	StorageResult result = STORAGE_FAIL;

	// validate file handle and dest buffer
	FILE *f = GetFilePtr( fileHandle );
	if( f && pBuffer )
	{
		int bytesRead = (int)fread( pBuffer, 1, readSizeBytes, f );
		if( bytesRead == readSizeBytes )
		{
			// any time you don't read exactly as much as you want, the call fails
			// always use FileGetSize() to figure out exactly how much you want to read ahead of time if need be
			result = STORAGE_OK;
		}
	}

	return result;
}

Storage::StorageResult Storage::FileWrite( BoyFileHandle fileHandle, const void *pBuffer, int writeSizeBytes )
{
	StorageResult result = STORAGE_FAIL;

	// validate file handle and src buffer
	FILE *f = GetFilePtr( fileHandle );
	if( f && pBuffer )
	{
		int bytesWritten = (int)fwrite( pBuffer, 1, writeSizeBytes, f );
		if( bytesWritten == writeSizeBytes )
		{
			result = STORAGE_OK;
		}
	}

	return result;
}

Storage::StorageResult Storage::FileClose( BoyFileHandle fileHandle )
{
	StorageResult result = STORAGE_FAIL;

	// validate file handle
	FILE *f = GetFilePtr( fileHandle );
	if( f )
	{
		int closeResult = fclose( f );
		if( closeResult != EOF )
		{
			int key = (int)fileHandle;
			mOpenFiles.erase( key );
			result = STORAGE_OK;
		}
	}

	return result;
}

int Storage::FileGetSize( BoyFileHandle openFileHandle )
{
	int sizeBytes = -1;

	// validate file handle
	FILE *f = GetFilePtr( openFileHandle );
	if( f )
	{
		long int origPos = ftell( f );
		fseek( f, 0, SEEK_END );
		sizeBytes = ftell( f );
		fseek( f, origPos, SEEK_SET );
	}

	return sizeBytes;
}

FILE *Storage::GetFilePtr( BoyFileHandle hFile )
{
	FILE *pRet = NULL;

	int key = (int)hFile;
	std::map<int,FILE*>::iterator i = mOpenFiles.find( key );
	if( i != mOpenFiles.end() )
	{
		pRet = i->second;
	}

	return pRet;
}

Storage::StorageResult Storage::FileGetSize( const char *pFilePath, int *pSizeBytesOut )
{
	StorageResult result = STORAGE_FAIL;

	BoyFileHandle hFile;
	StorageResult openResult = FileOpen( pFilePath, STORAGE_MODE_READ | STORAGE_MUST_EXIST, &hFile );
	if( (openResult == STORAGE_OK) && pSizeBytesOut )
	{
		*pSizeBytesOut = FileGetSize( hFile );
		result = STORAGE_OK;
	}
	if (openResult == STORAGE_OK) FileClose(hFile);

	return result;
}