#include "PersistenceLayer.h"

#include <assert.h>
#include <filesystem>
#include "Crypto.h"
#include "Storage.h"
#include "Environment.h"
#include "BoyLib/BoyUtil.h"
#include <iostream>

#ifdef _WIN32
	#include <shlobj.h>
#endif

using namespace Boy;

PersistenceLayer::PersistenceLayer(const BoyLib::UString &filename, unsigned char *key)
{
	mKey = key;

    #ifdef _WIN32
        wchar_t buffer[MAX_PATH];
        HRESULT result = SHGetFolderPathW(NULL, CSIDL_LOCAL_APPDATA, NULL, 0, buffer);
	    assert(result==S_OK);
        mFileName.append(buffer);
        mFileName.append("\\Boy\\");
    #elif __linux__
        mFileName.append("save/");
    #endif

	mFileName.append(filename);

    std::filesystem::path folderPath = mFileName.wc_str();
    folderPath = folderPath.parent_path();
    if (!std::filesystem::exists(folderPath))
    {
        std::filesystem::create_directories(folderPath);
    }

    load();
}

PersistenceLayer::~PersistenceLayer()
{
}

bool PersistenceLayer::remove(const std::string &name, bool persist)
{
	// find the name:
	std::map<std::string,std::string>::iterator iter = mValues.find(name);

	// if it doesn't exist
	if (iter==mValues.end())
	{
		return false;
	}

	// erase it:
	mValues.erase(iter);

	if (persist)
	{
		this->persist();
	}

	// success:
	return true;
}

void PersistenceLayer::putString(const std::string &name, const std::string &value, bool persist)
{
	assert(Trim(name).size()>0);
	if (mValues[name]!=value)
	{
		mValues[name] = value;
		if (persist)
		{
			this->persist();
		}
	}
}

const std::string PersistenceLayer::getString(const std::string &name)
{
	std::map<std::string,std::string>::iterator iter = mValues.find(name);
	if (iter!=mValues.end())
	{
		return iter->second;
	}
	else
	{
		return std::string("");
	}
}

void PersistenceLayer::load()
{
	BoyFileHandle hFile;
	Storage *pStorage = Environment::instance()->getStorage();
	Storage::StorageResult result = pStorage->FileOpen( mFileName.toUtf8(), Storage::STORAGE_MODE_READ | Storage::STORAGE_MUST_EXIST, &hFile );
	if( result == Storage::STORAGE_OK )
	{
		int size = pStorage->FileGetSize( hFile );
		char *data = new char [size];
		result = pStorage->FileRead( hFile, data, size );
		assert( result == Storage::STORAGE_OK );
		pStorage->FileClose( hFile );

		char *decData;
		int decSize;
		Boy::aesDecrypt(mKey,data,size,&decData,&decSize);

		parse(decData, decSize);

		delete[] data;
	}
}

void PersistenceLayer::save()
{
	envDebugLog("------------------------ persisting data ----------------------------\n");
	std::ostringstream writeStr;
	for (std::map<std::string,std::string>::iterator i=mValues.begin() ; i!=mValues.end() ; i++)
	{
		if (i->first.length()!=0) // skip null values
		{
			assert(i->first.length()>0);
			writeStr << i->first.length() << "," << i->first;
			writeStr << i->second.length() << "," << i->second;
			envDebugLog("%s = %s\n",i->first.c_str(),i->second.c_str());
		}
	}
	envDebugLog("---------------------------------------------------------------------\n");

	// mark the end of the data:
	writeStr << ",0.";

	// decrypt data into output data buffer:
	int size = (int)writeStr.str().size();
	int encSize;
	char *encData;
	Boy::aesEncrypt(mKey, writeStr.str().c_str(), size, &encData, &encSize);

	// write to file:
	BoyFileHandle hFile;
	Storage *pStorage = Environment::instance()->getStorage();
	Storage::StorageResult result = pStorage->FileOpen( mFileName.toUtf8(), Storage::STORAGE_MODE_WRITE | Storage::STORAGE_OPEN_ALWAYS, &hFile );
	assert( result == Storage::STORAGE_OK );
	result = pStorage->FileWrite( hFile, encData, encSize );
	assert( result == Storage::STORAGE_OK );
	pStorage->FileClose( hFile );
}

void PersistenceLayer::parse(char *data, int size)
{
	int namePos;
	int nameSize;
	int valuePos;
	int valueSize;

	envDebugLog("---------------- loading persistent data ----------------------------\n");

	for (int pos=0 ; pos<size ; )
	{
		// read the name size:
		nameSize = readInt(data,&pos);
		if (nameSize==0)
		{
			// we've reached the end:
			break;
		}
		
		// skip the comma:
		pos++;

		// remember beginning of name:
		namePos = pos;

		// advance the position:
		pos += nameSize;

		// read the value size:
		valueSize = readInt(data,&pos);
		if (valueSize==0)
		{
			envDebugLog("bad data in %s at position %d (zero length value)\n",mFileName.toUtf8(),pos);
			return;
		}

		// skip the comma:
		pos++;

		// remember beginning of value:
		valuePos = pos;

		// advance the position:
		pos += valueSize;

		// remember the name/value pair:
		std::string name = std::string(data+namePos,nameSize);
		std::string value = std::string(data+valuePos,valueSize);
		mValues[name] = value;

		envDebugLog("%s = %s\n", name.c_str(), value.c_str());
	}

	envDebugLog("---------------------------------------------------------------------\n");
}

int PersistenceLayer::readInt(char *data, int *pos)
{
	int value = 0;
	while (data[*pos]>='0' && data[*pos]<='9')
	{
		value *= 10;
		value += data[*pos]-'0';
		(*pos)++;
	}

	return value;
}

int PersistenceLayer::getKeyCount()
{
	return (int)mValues.size();
}

const std::string PersistenceLayer::getKey(int i)
{
	std::map<std::string,std::string>::iterator iter = mValues.begin();
	for (int x=0 ; x<i ; x++)
	{
		iter++;
	}
	return iter->first;
}

void PersistenceLayer::persist()
{
	save();
}
