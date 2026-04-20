#include "ResourceLoader.h"

#include "SDL2/SDL_image.h"
#include "SDL2/SDL_mixer.h"
#include "GL/gl.h"
#include "GL/glext.h"
#include "Image.h"
#include "Sound.h"
#include <string>
#include <iostream>

using namespace Boy;

ResourceLoader::ResourceLoader()
{
}

ResourceLoader::~ResourceLoader()
{
}

bool ResourceLoader::load(Image *image) {
    SDL_Surface* surface = NULL;
    std::string path = image->getPath() + ".png";
    
    surface = IMG_Load(path.c_str());

    if (surface == NULL) {
        printf("Could not load image %s: %s\n", path.c_str(), IMG_GetError());
        return false;
    }

    SDL_Surface *temp = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_RGBA32, 0);
    SDL_FreeSurface(surface);
    surface = temp;

    if (surface == NULL) {
        printf("Could not load image %s: %s\n", path.c_str(), SDL_GetError());
        return false;
    }

    GLuint textureID = 0;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(
        GL_TEXTURE_2D, 
        0, 
        GL_RGBA, 
        surface->w, 
        surface->h, 
        0, 
        GL_RGBA, 
        GL_UNSIGNED_BYTE, 
        surface->pixels
    );
    
    image->setSize(surface->w, surface->h);
    image->setTextureId(textureID);
    
    SDL_FreeSurface(surface); 
    glBindTexture(GL_TEXTURE_2D, 0); 

    return true;
}

bool ResourceLoader::load(Sound *sound) {
    std::string path = sound->getPath() + ".ogg";

    Mix_Chunk* chunk = Mix_LoadWAV(path.c_str());

    if (chunk == NULL) {
        printf("Could not load sound %s: %s\n", path.c_str(), Mix_GetError());
        return false;
    }

    sound->setChunk(chunk);

    return true;
}

Image *ResourceLoader::createImage(const std::string &filename)
{
    return new Image(this, filename);
}

Sound *ResourceLoader::createSound(const std::string &filename)
{
    return new Sound(this, filename);
}