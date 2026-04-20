#include "Image.h"

#include "SDL2/SDL_image.h"
#include "GL/gl.h"
#include <string>
#include "ResourceLoader.h"

using namespace Boy;

Image::Image(ResourceLoader *loader, const std::string &path) : Resource(loader, path)
{
    mTextureID = 0;
    mWidth = -1;
    mHeight = -1;
}

Image::~Image()
{

}

int Image::getWidth()
{
	return mWidth;
}

int Image::getHeight()
{
	return mHeight;
}

void Image::setSize(int width, int height) {
    mWidth = width;
    mHeight = height;
}

bool Image::init()
{
	return mLoader->load(this);
}