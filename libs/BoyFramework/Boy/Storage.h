
#pragma once

#include <stdio.h>
#include <map>

namespace Boy
{
	/*
	 * interface for a generic mass storage device that can read/write files
	 *
	*/

	typedef int BoyFileHandle;

	class Storage
	{

		public:

			enum StorageResult
			{
				STORAGE_OK			= 0,
				STORAGE_FAIL		= 1,
			};

			enum StorageModeFlags
			{
				STORAGE_MODE_READ	= 0x0001,
				STORAGE_MODE_WRITE	= 0x0002,
				STORAGE_MODE_MASK	= 0x000F,

				STORAGE_MUST_EXIST	= 0x0010,
				STORAGE_OPEN_ALWAYS	= 0x0020,
				STORAGE_DISPO_MASK	= 0x00F0,
			};

			Storage();

			// file io interface
			StorageResult FileOpen( const char *pFilePathUtf8, int modeFlags, BoyFileHandle *pFileHandleOut );
			StorageResult FileRead( BoyFileHandle fileHandle, void *pBuffer, int readSizeBytes );
			StorageResult FileWrite( BoyFileHandle fileHandle, const void *pBuffer, int writeSizeBytes );
			StorageResult FileClose( BoyFileHandle fileHandle );
			int FileGetSize( BoyFileHandle openFileHandle );

			// helpers
			StorageResult FileGetSize( const char *pFilePath, int *pSizeBytesOut );

		private:

			FILE *GetFilePtr( BoyFileHandle hFile );

			int mFileKey;
			std::map<int,FILE*> mOpenFiles;

	};

}