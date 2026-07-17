#include "Clipboard.h"

#include "SDL2/SDL.h"

std::string Boy::getClipboardText()
{
    char *text = SDL_GetClipboardText();
    std::string result(text);
    SDL_free(text);
    return result;
}

void Boy::setClipboardText(const std::string &text)
{
    SDL_SetClipboardText(text.c_str());
}