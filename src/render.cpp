#include <cstdio>
#include <cstring>
#include <string>
#include "render.h"

using namespace std;

Render::Render(Map *_map){
    map = _map;

    if(SDL_Init(SDL_INIT_EVERYTHING) != 0){
        printf("[ERROR]\tSDL_Init: %s\n", SDL_GetError());
        delete this;
        return;
    }

    int rc = SDL_CreateWindowAndRenderer(INIT_WIDTH, INIT_HEIGHT, 0,
            &window, &renderer);
    if(rc != 0) {
        SDL_Quit();
        printf("SDL_CreateWindowAndRenderer error: %s\n", SDL_GetError());
        delete this;
        return;
    };

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
    SDL_RenderSetLogicalSize(renderer, INIT_WIDTH, INIT_HEIGHT);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

    SDL_SetWindowTitle(window, WINDOW_TITLE);


    screen = SDL_CreateRGBSurface(0, INIT_WIDTH, INIT_HEIGHT, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
    scrtex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, INIT_WIDTH, INIT_HEIGHT);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    //dottedBox(screen, 0, 0, screen->w, screen->h, ABSOLUTE_BACKGROUND);
    SDL_RenderPresent(renderer);

    if(!loadResources()){
        printf("[ERROR]\tCannot load resources.\n");
        delete this;
        return;
    }
}

Render::~Render(){

}

bool Render::loadResources(){
    // ASCII Charset
    charset = SDL_LoadBMP(RESOURCE_CHARSET);
    if(!charset){
        printf("Cannot load: %s\n", RESOURCE_CHARSET);
        return false;
    }
    SDL_SetColorKey(charset, true, 0x000000);

    // Tileset
    tileset = SDL_LoadBMP(RESOURCE_TILESET);
    if(!tileset){
        printf("Cannot load: %s\n", RESOURCE_TILESET);
        return false;
    }
    SDL_SetColorKey(tileset, true, 0x000000);

    // Background Tile
    backgroundTile = SDL_LoadBMP(RESOURCE_BACKGROUND);
    if(!backgroundTile){
        printf("Cannot load: %s\n", RESOURCE_BACKGROUND);
        return false;
    }

    // Calculate variables
    startX = screen->w/2 - ((BLOCKS_W+2)*BLOCK_W)/2;
    startY = 40;
    rows = (screen->h - startY)/BLOCK_H + 1;
    fieldBG.x = startX;
    fieldBG.y = startY;
    fieldBG.w = (BLOCKS_W+2)*BLOCK_W;
    fieldBG.h = (rows)*BLOCK_H;
    map->setField(fieldBG.w - 2*BLOCK_W - BALL_W, fieldBG.h - BLOCK_H);

    // Pre-render background
    SDL_Rect r;
    background = SDL_CreateRGBSurface(0, fieldBG.w, fieldBG.h, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
    for(int i=0; i<(fieldBG.w/backgroundTile->w + 1); i++)
        for(int j=0; j<(fieldBG.h/backgroundTile->h +1); j++){
            r.x = i*backgroundTile->w;
            r.y = j*backgroundTile->h;
            r.w = backgroundTile->w;
            r.h = backgroundTile->h;
            SDL_BlitSurface(backgroundTile, nullptr, background, &r);
        }

    parseTileset(tileset);

    return true;
}

void Render::parseTileset(SDL_Surface *_tileset){
    // Get blocks
    for(int i=0; i<10; i++){
        for(int j=0; j<8; j++){
            SDL_Rect cut;
            cut.x = i*BLOCK_W;
            cut.y = j*BLOCK_H;
            cut.w = BLOCK_W;
            cut.h = BLOCK_H;
            blockBitmap[i*8 + j] = SDL_CreateRGBSurface(0, cut.w, cut.h, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
            SDL_BlitSurface(_tileset, &cut, blockBitmap[i*8 + j], nullptr);
        }
    }

    // Get balls
    for(int i=0; i<3; i++){
        for(int j=0; j<2; j++){
            SDL_Rect cut;
            cut.x = i*BALL_W;
            cut.y = j*BALL_H + 144;
            cut.w = BALL_W;
            cut.h = BALL_H;
            ballBitmap[i*2 + j] = SDL_CreateRGBSurface(0, cut.w, cut.h, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
            SDL_BlitSurface(_tileset, &cut, ballBitmap[i*2 + j], nullptr);
        }
    }

    // Get small palettes
    for(int i=0; i<2; i++){
        for(int j=0; j<2; j++){
            SDL_Rect cut;
            cut.x = i*PALETTE_SW + 24;
            cut.y = j*PALETTE_SH + 144;
            cut.w = PALETTE_SW;
            cut.h = PALETTE_SH;
            paletteBitmap[i*2 + j] = SDL_CreateRGBSurface(0, cut.w, cut.h, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
            SDL_BlitSurface(_tileset, &cut, paletteBitmap[i*2 + j], nullptr);
        }
    }

    // Get big palettes
    for(int i=0; i<2; i++){
        for(int j=0; j<2; j++){
            SDL_Rect cut;
            cut.x = i*PALETTE_BW;
            cut.y = j*PALETTE_BH + 128;
            cut.w = PALETTE_BW;
            cut.h = PALETTE_BH;
            paletteBitmap[i*2 + j + 4] = SDL_CreateRGBSurface(0, cut.w, cut.h, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
            SDL_BlitSurface(_tileset, &cut, paletteBitmap[i*2 + j + 4], nullptr);
        }
    }

    // Get icons
    for(int i=0; i<8; i++){
        SDL_Rect cut;
        cut.x = i*ICON_W + 120;
        cut.y = 144;
        cut.w = ICON_W;
        cut.h = ICON_H;
        iconBitmap[i] = SDL_CreateRGBSurface(0, cut.w, cut.h, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
        SDL_BlitSurface(_tileset, &cut, iconBitmap[i], nullptr);
    }
}

void Render::drawSurface(SDL_Surface *src, int x, int y){
    if(!src)
        return;
    SDL_Rect dest;
    dest.x = x;
    dest.y = y;
    dest.w = src->w;
    dest.h = src->h;
    SDL_BlitSurface(src, nullptr, screen, &dest);
}

void Render::drawFrame(){
    SDL_BlitSurface(background, nullptr, screen, &fieldBG);
    for(int i=0; i<BLOCKS_W+2; i++){
        for(int j=0; j<rows; j++){
            // Borders
            if(i == 0 || i == BLOCKS_W+1 || j == 0){
                drawSurface(blockBitmap[6], i*BLOCK_W+startX, j*BLOCK_H+startY);
            } else if(j<BLOCKS_H+1){
                // Blocks
                if(map->getBlock(i-1, j-1)->type != EMPTY_BLOCK_ID){
                    drawSurface(blockBitmap[map->getBlock(i-1, j-1)->type], i*BLOCK_W+startX, j*BLOCK_H+startY);
                }
            }
            // Palette
            if(j-1 == PALETTE_ROW){
                drawSurface(paletteBitmap[map->getPalette()->type + 4*(map->getPalette()->big - 1)], map->getPalette()->pos + BLOCK_W + startX, PALETTE_ROW*BLOCK_H + BLOCK_H + startY);
                if(DEBUG_MODE){
                    char dd[8];
                    sprintf(dd, "<%d,%d>", map->getPalette()->pos, map->getPalette()->pos+map->getPalette()->w);
                    drawString(screen, map->getPalette()->pos + BLOCK_W + startX, BLOCK_H*PALETTE_ROW + startY + BLOCK_H + 8, dd, charset);
                }
            }
        }
    }

    // Entities
    for(int i=0; i<MAX_BALLS; i++)
        if(map->getBall(i) && ballBitmap[map->getBall(i)->type]){
            drawSurface(ballBitmap[map->getBall(i)->type], int(map->getBall(i)->x) + startX + BLOCK_W, int(map->getBall(i)->y) + startY + BLOCK_H);
        }

    for(int i=0; i< MAX_ENTITIES; i++)
        if(map->getEntity(i)){
            drawSurface(iconBitmap[map->getEntity(i)->icon], int(map->getEntity(i)->x) + startX, int(map->getEntity(i)->y) + startY);
            if(DEBUG_MODE){
                char dd[4];
                sprintf(dd, "n:%d, X: %f", i, map->getEntity(i)->x);
                drawString(screen, int(map->getEntity(i)->x) + startX, int(map->getEntity(i)->y)-8 + startY, dd, charset);
            }
        }

    // Status
    drawRectangle(screen, 10, 12, screen->w - 10, 24, 0, 0);
    for(int i=0; i<map->maxLives; i++){
        if(i<map->getLives()) drawSurface(iconBitmap[ICON_HEART], 10 + i*ICON_W, 12);
        else drawSurface(iconBitmap[ICON_HEART_DARK], 10 + i*ICON_W, 12);
    }
    char line[80];
    sprintf(line, "Punkty: %d  Poziom: %d/%d  FPS: %d  Klocki: %d/%d", map->getPoints(), map->currentLevel + 1, map->levelAmount, fps, map->blockAmount, map->level->blockAmount);
    drawString(screen, 20 + map->maxLives*ICON_W, 16, line, charset);
    flip();

    calcFPS();
}

void Render::calcFPS(){
    int time = SDL_GetTicks();
    if(time-lastFPSCalc >= 1000){
        fps = framesInSec;
        framesInSec = 0;
        lastFPSCalc = time;
    } else{
        framesInSec++;
    }
}

void Render::flip(){
    SDL_UpdateTexture(scrtex, nullptr, screen->pixels, screen->pitch);
    SDL_RenderCopy(renderer, scrtex, nullptr, nullptr);
    SDL_RenderPresent(renderer);
}

void Render::drawString(SDL_Surface *screen, int x, int y, const char *text, SDL_Surface *charset) {
    int px, py, c;
    SDL_Rect s, d;
    s.w = 8;
    s.h = 8;
    d.w = 8;
    d.h = 8;
    while(*text) {
        c = *text & 255;
        px = (c % 16) * 8;
        py = (c / 16) * 8;
        s.x = px;
        s.y = py;
        d.x = x;
        d.y = y;
        SDL_BlitSurface(charset, &s, screen, &d);
        x += 8;
        text++;
    };
};

void Render::drawPixel(SDL_Surface *surface, int x, int y, Uint32 color) {
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * surface->format->BytesPerPixel;
    *(Uint32 *)p = color;
};

void Render::drawLine(SDL_Surface *screen, int x, int y, int l, int dx, int dy, Uint32 color) {
    for(int i = 0; i < l; i++) {
        drawPixel(screen, x, y, color);
        x += dx;
        y += dy;
    };
};

void Render::drawRectangle(SDL_Surface *screen, int x, int y, int l, int k, Uint32 outlineColor, Uint32 fillColor) {
    int i;
    drawLine(screen, x, y, k, 0, 1, outlineColor);
    drawLine(screen, x + l - 1, y, k, 0, 1, outlineColor);
    drawLine(screen, x, y, l, 1, 0, outlineColor);
    drawLine(screen, x, y + k - 1, l, 1, 0, outlineColor);
    for(i = y + 1; i < y + k - 1; i++)
        drawLine(screen, x + 1, i, l - 2, 1, 0, fillColor);
};

void Render::dottedBox(SDL_Surface *screen, int x, int y, int w, int h, Uint32 fillColor){
    for(int i=0; i<w; i++)
        for(int j=0; j<h; j++){
            if((i % 2 == 0 && j % 2 == 1) || (i % 2 == 1 && j % 2 == 0)){
                drawPixel(screen, x+i, y+j, fillColor);
            }
        }
}

bool Render::dialogYesNo(const char *caption){
    int w = 400;
    int h = 100;
    int x = screen->w/2 - w/2;
    int y = screen->h/2 - h/2;
    dottedBox(screen, 0, 0, screen->w, screen->h, ABSOLUTE_BACKGROUND);
    drawRectangle(screen, x, y, w, h, 0xAAAAAA, 0x444444);
    int strpos = w/2 - strlen(caption)*4;
    drawString(screen, x + strpos, y + 10, caption, charset);

    int choose = 0;
    int focus = 1;
    int colorY = 0x88FF88;
    int colorN = 0xAA8888;
    flip();
    while(!choose){
        const Uint8 *keys = SDL_GetKeyboardState(nullptr);
        SDL_PumpEvents();
        if(keys[SDL_SCANCODE_LEFT]){
            focus = 1;
            colorY = 0x88FF88;
            colorN = 0xAA8888;
        }
        if(keys[SDL_SCANCODE_RIGHT]){
            focus = 2;
            colorN = 0xFF8888;
            colorY = 0x88AA88;
        }
        if(keys[SDL_SCANCODE_RETURN]){
            break;
        }
        if(keys[SDL_SCANCODE_ESCAPE]){
            return false;
        }
        drawRectangle(screen, x + 100, y + h - 40, 50, 20, 0xFFFFFF, colorY);
        drawString(screen, x + 112, y + h - 34, "TAK", charset);
        drawRectangle(screen, x + 250, y + h - 40, 50, 20, 0xFFFFFF, colorN);
        drawString(screen, x + 262, y + h - 34, "NIE", charset);
        flip();
        SDL_Delay(33);
    }

    drawRectangle(screen, 0, 0, screen->w, screen->h, 0, 0);

    if(focus == 1){
        return true;
    } else{
        return false;
    }
}

