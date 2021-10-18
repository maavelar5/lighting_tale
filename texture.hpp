#ifndef TEXTURE
#define TEXTURE

#include "globals.hpp"
#include "utils.hpp"

struct Texture
{
    uint         w, h, id;
    SDL_Surface *surface;
};

static Texture load_xpm (char *data[])
{
    uint         w, h, id;
    SDL_Surface *regsurface = IMG_ReadXPMFromArray ((char **)data);
    SDL_Surface *surface    = 0;

    assert (regsurface != NULL);

    surface = SDL_ConvertSurfaceFormat (regsurface, SDL_PIXELFORMAT_RGBA32, 0);

    int mode            = GL_RGB;
    int internal_format = GL_SRGB_ALPHA;

    if (surface->format->BytesPerPixel == 4)
    {
        mode = GL_RGBA;
    }

    glGenTextures (1, &id);
    glBindTexture (GL_TEXTURE_2D, id);

    glTexImage2D (GL_TEXTURE_2D, 0, internal_format, surface->w, surface->h, 0,
                  mode, GL_UNSIGNED_BYTE, surface->pixels);

    glGenerateMipmap (GL_TEXTURE_2D);

    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    w = surface->w;
    h = surface->h;

    SDL_FreeSurface (regsurface);
    SDL_FreeSurface (surface);

    return { w, h, id, 0 };
}

#endif
