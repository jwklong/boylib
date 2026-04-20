#pragma once

#include <string>

namespace Boy
{
	class ResourceLoader;

	class Resource
	{
	public:

		Resource(ResourceLoader *loader, const std::string &path);
		virtual ~Resource();

		virtual bool load();
		virtual void reload();
		virtual void release();

		bool isLoaded();

		const std::string &getPath() { return mPath; }

		virtual bool init() { return true; };
		virtual void destroy() {};

	protected:

		ResourceLoader *mLoader;
		std::string mPath;

	private:

		int mRefCount;

	};
}