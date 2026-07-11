#pragma once

#include "BoyLib/Rect.h"
#include "BoyLib/UString.h"
#include "BoyLib/Vector2.h"
#include <string>
#include "TinyXML/tinyxml.h"
#include <vector>

namespace Boy
{

	class Font;
	class Image;
	class Sound;
	class Resource;
	class ResourceGroup;
	class ResourceLoader;

	class ResourceManager
	{
	public:
		
		ResourceManager(ResourceLoader *loader, unsigned char *key);
		virtual ~ResourceManager();

		// resource file parsing:
		virtual bool parseResourceFile(const std::string &fileName, unsigned char *key = NULL);

		// resource loading/unloading:
		virtual bool loadResourceGroup(const std::string &groupName);
		virtual void unloadResourceGroup(const std::string &groupName);
		virtual void reloadResources();
		virtual void destroyResources();
		virtual void initResources();

		// id based resource access:
		virtual Image *getImage(const std::string &id); 
		virtual Font *getFont(const std::string &id);
		virtual Sound *getSound(const std::string &id);

	private:
		void parseResourceGroup(TiXmlElement *elem);
		void addResource(
			const std::string &id, 
			const std::string &path, 
			ResourceGroup *group,
			Resource *resource);
		void mapResource(const std::string &id, 
			const std::string &path, 
			ResourceGroup *group);
		Resource *createResource(const char *type, const std::string &path);

	private:
		// resource loader:
		ResourceLoader *mResourceLoader;

		// resource files:
		std::vector<std::string> mParsedResourceFiles;

		// resource groups:
		std::map<std::string,ResourceGroup*> mResourceGroups;

		// resources:
		std::map<std::string,Resource*> mResourcesByPath;
		std::map<std::string,Resource*> mResourcesById;
	};
}
