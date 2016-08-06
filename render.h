#ifndef _RENDER_H_
#define _RENDER_H_

#include <SDL2/SDL.h>
#include "map.h"
#include "defines.h"

class Render{
private:
    // General
    Map *map;
    int lastFPSCalc;
    int framesInSec;
    int fps;
    void calcFPS();

    // SDL General
    SDL_Surface *screen;
    SDL_Texture *scrtex;
    SDL_Window *window;
    SDL_Renderer *renderer;
    void flip();

    // Primitives
    void drawString(SDL_Surface *screen, int x, int y, const char *text, SDL_Surface *charset);
    void drawPixel(SDL_Surface *surface, int x, int y, Uint32 color);
    void drawLine(SDL_Surface *screen, int x, int y, int l, int dx, int dy, Uint32 color);
    void drawRectangle(SDL_Surface *screen, int x, int y, int l, int k, Uint32 outlineColor, Uint32 fillColor);
    void drawSurface(SDL_Surface *src, int x, int y);
    void dottedBox(SDL_Surface *screen, int x, int y, int w, int h, Uint32 fillColor);

    // Resources
    SDL_Surface *charset;
    SDL_Surface *tileset;
    SDL_Surface *blockBitmap[90];
    SDL_Surface *ballBitmap[6];
    SDL_Surface *paletteBitmap[8];
    SDL_Surface *iconBitmap[8];
    SDL_Surface *backgroundTile;
    SDL_Surface *background;
    bool loadResources();
    void parseTileset(SDL_Surface *_tileset);

    // Render variables
    int startX, startY, rows;
    SDL_Rect fieldBG;

public:
    Render(Map *_map);
    ~Render();
    void drawFrame();
    bool dialogYesNo(const char *caption);
};

#endif
