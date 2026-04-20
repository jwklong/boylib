#include "ResourceManager.h"

#include <algorithm>
#include "BoyLib/BoyUtil.h"
#include "Environment.h"
#include <fstream>
#include <iostream>
#include "Font.h"
#include "Image.h"
#include "Sound.h"
#include "Resource.h"
#include "ResourceGroup.h"
#include "ResourceLoader.h"
#include <string>
#include <stdio.h>

using namespace Boy;

ResourceManager::ResourceManager(ResourceLoader *loader)
{
    mResourceLoader = loader;
}

ResourceManager::~ResourceManager()
{

}

bool ResourceManager::parseResourceFile(const std::string &fileName)
{
	// if we've already loaded the fonts:
	if (find(mParsedResourceFiles.begin(),mParsedResourceFiles.end(),fileName) != mParsedResourceFiles.end())
	{
		// ignore this call:
		return true;
	}

	TiXmlDocument doc;

    bool success = doc.LoadFile(fileName.c_str());
    if (!success)
    {
        return false;
    }
	
	TiXmlElement *root = doc.RootElement();

	for (TiXmlElement *e = root->FirstChildElement() ; e!=NULL ; e = e->NextSiblingElement())
	{
		if (Boy::Environment::instance()->stricmp(e->Value(),"resources")==0)
		{
			parseResourceGroup(e);
		}
		else
		{
			assert(false);
		}
	}

	doc.Clear();

	return true;
}

void ResourceManager::parseResourceGroup(TiXmlElement *elem)
{
	std::string groupId = elem->Attribute("id");
	ResourceGroup *group = new ResourceGroup();
	mResourceGroups[groupId] = group;

	std::string basePath;
	std::string idPrefix;

	for (TiXmlElement *child = elem->FirstChildElement() ; child!=NULL ; child = child->NextSiblingElement())
	{
		const char *val = child->Value();
		if (Boy::Environment::instance()->stricmp(val,"SetDefaults")==0)
		{
			idPrefix = child->Attribute("idprefix");
			basePath = child->Attribute("path");
			// add trailing slash if necessary:
			if (basePath[basePath.size()-1]!='/' && 
				basePath[basePath.size()-1]!='\\')
			{
				basePath += '/';
			}
			// remove "./" from begining if it's there:
			if (basePath.substr(0,2)=="./" ||
				basePath.substr(0,2)==".\\")
			{
				basePath = basePath.substr(2);
			}
		}
		else
		{
			std::string id = idPrefix + child->Attribute("id");
			std::string fullPath = basePath;
			fullPath.append(child->Attribute("path"));

			// if this resource path already exists:
			if (mResourcesByPath.find(fullPath)!=mResourcesByPath.end())
			{
				// just create a mapping:
				mapResource(id, fullPath, group);
			}
			else
			{
				// create and add the resource:
				Boy::Resource *res = createResource(val,fullPath);
				addResource(id, fullPath, group, res);
			}
		}
	}
}

Resource *ResourceManager::createResource(const char *type, const std::string &path)
{
	Resource *res;
	if (Boy::Environment::instance()->stricmp(type,"image")==0)
	{
		res = mResourceLoader->createImage(path);
	}
	else if (Boy::Environment::instance()->stricmp(type,"font")==0)
	{
		res = new Font(mResourceLoader, path);
	}
	else if (Boy::Environment::instance()->stricmp(type,"sound")==0)
	{
		res = mResourceLoader->createSound(path);
	}
	else
	{
		assert(false);
	}
	return res;
}

void ResourceManager::addResource(const std::string &id, 
								  const std::string &path, 
								  ResourceGroup *group, 
								  Resource *resource)
{
	assert(resource!=NULL);

	// add it to the group:
	group->addResourcePath(path);

	// create a path/id->resource mapping:
	mResourcesByPath[path] = resource;
	mResourcesById[id] = resource;
}

void ResourceManager::mapResource(const std::string &id, 
								  const std::string &path, 
								  ResourceGroup *group)
{
	group->addResourcePath(path);

	// get the resource pointed to by this resource path:
	Resource *res = mResourcesByPath[path];

	// map new id to existing resource:
	mResourcesById[id] = res;
}

bool ResourceManager::loadResourceGroup(const std::string &groupName)
{
	// note: it's ok to load a group more than once as long as it's
	// released more than once too.  different parts of the code
	// can use the same resource group and each of them should load
	// it when they need it and unload it when they done with it
	if (mResourceGroups.find(groupName) != mResourceGroups.end())
	{
		// disable full screen toggle while resource load/unload:
		Environment::instance()->disableFullScreenToggle();

		ResourceGroup *g = mResourceGroups[groupName];
		bool success = true;
		if (!g->isEmpty())
		{
			for (const std::string *path = g->getFirstPath() ; path!=NULL ; path = g->getNextPath())
			{
				assert(mResourcesByPath.find(*path) != mResourcesByPath.end());
				Resource *res = mResourcesByPath[*path];
				assert(res!=NULL);
				success &= res->load();
			}
		}

		Environment::instance()->enableFullScreenToggle();

		return success;
	}
	else
	{
		assert(false);

		return false;
	}
}

void ResourceManager::unloadResourceGroup(const std::string &groupName)
{
	if (mResourceGroups.find(groupName) != mResourceGroups.end())
	{
		// disable full screen toggle while resource load/unload:
		Environment::instance()->disableFullScreenToggle();

		ResourceGroup *g = mResourceGroups[groupName];
		for (const std::string *path=g->getFirstPath() ; path!=NULL ; path=g->getNextPath())
		{
			Resource *res = mResourcesByPath[*path];
			assert(res!=NULL);
			res->release();
		}

		Environment::instance()->enableFullScreenToggle();
	}
	else
	{
		assert(false);
	}
}

void ResourceManager::reloadResources()
{
	// disable full screen toggle while resource load/unload:
	Environment::instance()->disableFullScreenToggle();

	// iterate over all reasources:
	for (std::map<std::string,Resource*>::iterator iter = mResourcesByPath.begin() ;
		iter != mResourcesByPath.end() ; 
		iter ++)
	{
		iter->second->reload();
	}

	Environment::instance()->enableFullScreenToggle();
}

void ResourceManager::destroyResources()
{
	// disable full screen toggle while resource load/unload:
	Environment::instance()->disableFullScreenToggle();

	// iterate over all reasources:
	for (std::map<std::string,Resource*>::iterator iter = mResourcesByPath.begin() ;
		iter != mResourcesByPath.end() ; 
		iter ++)
	{
		iter->second->destroy();
	}

	Environment::instance()->enableFullScreenToggle();
}

void ResourceManager::initResources()
{
	// disable full screen toggle while resource load/unload:
	Environment::instance()->disableFullScreenToggle();

	// iterate over all reasources:
	for (std::map<std::string,Resource*>::iterator iter = mResourcesByPath.begin() ;
		iter != mResourcesByPath.end() ; 
		iter ++)
	{
		iter->second->init();
	}

	Environment::instance()->enableFullScreenToggle();
}

Image *ResourceManager::getImage(const std::string &id)
{
	std::map<std::string,Resource*>::iterator iter = mResourcesById.find(id);
	if (iter==mResourcesById.end())
	{
		printf("[WARNING: ResourceManager::getImage] image with id %s not found\n",id.c_str());
		return NULL;
	}
	Image *image = dynamic_cast<Image*>(iter->second);
	assert(image->isLoaded());
	return image;
}

Font *ResourceManager::getFont(const std::string &id)
{
	assert(mResourcesById.find(id)!=mResourcesById.end());
	Font *font = dynamic_cast<Font*>(mResourcesById[id]);
	assert(font->isLoaded());
	return font;
}

Sound *ResourceManager::getSound(const std::string &id)
{
	assert(mResourcesById.find(id)!=mResourcesById.end());
	Sound *sound = dynamic_cast<Sound*>(mResourcesById[id]);
	assert(sound->isLoaded());
	return sound;
}