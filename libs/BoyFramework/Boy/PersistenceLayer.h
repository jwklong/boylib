#pragma once

#include <map>
#include <string>
#include "BoyLib/UString.h"

namespace Boy
{
	class PersistenceLayer
	{
	public:

		PersistenceLayer(const BoyLib::UString &filename);
		virtual ~PersistenceLayer();

		virtual void putString(const std::string &name, const std::string &value, bool persist=false);
		virtual const std::string getString(const std::string &name);
		virtual bool remove(const std::string &name, bool persist=false);

		virtual int getKeyCount();
		virtual const std::string getKey(int i);

		virtual void persist();

	private:

		void load();
		void save();
		void parse(char *data, int size);
		int readInt(char *data, int *pos); // pos will be modified!

	private:

		std::map<std::string,std::string> mValues;
		BoyLib::UString mFileName;

	};
}