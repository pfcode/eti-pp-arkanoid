#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <fstream>
#include <iostream>
#include <cstring>
#include <string>
#include "map.h"

using namespace std;

Map::Map(){
    for(int i=0; i<BLOCKS_W; i++){
        for(int j=0; j<BLOCKS_H; j++){
            block[i][j] = (Block *)malloc(sizeof(Block));
            block[i][j]->type = 0;
        }
    }

    for (int i = 0; i < MAX_ENTITIES; i++) {
        entity[i] = nullptr;
    }

    for (int i = 0; i < MAX_AWARDS; i++) {
        award[i] = nullptr;
    }

    for (int i = 0; i < MAX_BALLS; i++) {
        ball[i] = nullptr;
    }

    palette = (Palette *) malloc(sizeof(Palette));
    for(int i = 0; i < MAX_BALLS; i++) {
        palette->dockedBalls[i] = nullptr;
    }

    lose = false;
    levelAmount = 0;
    currentLevel = -1;
    fieldBreakpoint = PALETTE_ROW * BLOCK_H;
    maxLives = LIVES;
}

Map::~Map(){

}

void Map::resetVariables(){
    levelAmount = 0;
    currentLevel = -1;
    lose = false;
    balls = 0;
    lives = LIVES;
    points = 0;
    blockAmount = 0;
    level = nullptr;
    maxLives = LIVES;
    stickyPalette = false;

    for (int i = 0; i < MAX_ENTITIES; i++) {
        if (entity[i] != nullptr) {
            delete entity[i];
            entity[i] = nullptr;
        }
    }

    for (int i = 0; i < MAX_AWARDS; i++) {
        if (award[i] != nullptr) {
            award[i]->duration = -1;
            calcAward(i);
        }
    }
}

bool Map::loadMap(const char* path){
    if(DEBUG_MODE) printf("[DBG]\tLoading levels from file\n");

    resetVariables();

    ifstream file(path);
    if(!file.is_open()){
        printf("[ERROR]\tCouldn't open the level file: %s", path);
        return false;
    }
    string s;
    Level *prevLevel;
    while(getline(file, s)){
        if(s.find("LEVEL") == 0){
            Level *l;
            l = (Level *)malloc(sizeof(Level));
            l->nextLevel = nullptr;
            l->blockAmount = 0;
            for (int i = 0; i < BLOCKS_H; i++) {
                for (int j = 0; j < BLOCKS_W; j++) {
                    l->block[i][j] = nullptr;
                }
            }
            for(int i=0; i<BLOCKS_H; i++){
                if(!getline(file, s))
                    return false;
                for(int j=0; j<BLOCKS_W; j++){
                    char c = s[j];
                    if(c == '\0')
                        break;
                    l->block[j][i] = (Block *)malloc(sizeof(Block));
                    if(c == '.')
                        l->block[j][i]->type = EMPTY_BLOCK_ID;
                    else if(c >= 65 && c<=90){
                        l->block[j][i]->type = char(c - 65);
                        if(char(c - 65) != BLOCK_BLOCKY_ID) l->blockAmount++;
                    }
                }
            }
            if(levelAmount == 0){
                level = (Level *)malloc(sizeof(Level));
                level->nextLevel = l;
                level->blockAmount = 0;
            } else{
                prevLevel->nextLevel = l;
            }
            prevLevel = l;
            levelAmount++;
        }
    }
    file.close();
    nextLevel();
    return true;
}

bool Map::nextLevel(){
    if(level->nextLevel){
        for(int i=0; i<BLOCKS_W; i++) {
            for(int j=0; j<BLOCKS_H; j++){
                block[i][j] = (Block *)malloc(sizeof(Block));
                memcpy(block[i][j], level->nextLevel->block[i][j], sizeof(Block));
            }
        }

        blockAmount = level->nextLevel->blockAmount;

        // Reset balls
        for(int i=0; i<MAX_BALLS; i++)
            removeBall(i);
        balls = 0;
        createBall(0, true);

        // Reset entities
        for(int i=0; i< MAX_ENTITIES; i++){
            if(entity[i] != nullptr) {
                delete entity[i];
                entity[i] = nullptr;
            }
        }

        for(int i=0; i< MAX_AWARDS; i++){
            if(award[i] != nullptr){
                award[i]->duration = -1;
                calcAward(i);
            }
        }

        currentLevel++;
        lose = false;
        level = level->nextLevel;
        return true;
    } else{
        return false;
    }
}

void Map::restartLevel(){
    for(int i=0; i<BLOCKS_W; i++){
        for(int j=0; j<BLOCKS_H; j++){
            block[i][j] = (Block *)malloc(sizeof(Block));
            memcpy(block[i][j], level->block[i][j], sizeof(Block));
        }
    }

    blockAmount = level->blockAmount;

    for(int i=0; i<MAX_BALLS; i++)
    {
        removeBall(i);
    }

    balls = 0;
    createBall(0, true);

    lose = false;
}

/* When the type is below zero, function will create a new ball
*  in position of ball[-type + 1]. */
void Map::createBall(int type, bool docked){
    int n = -1;
    for(int i=0; i<MAX_BALLS; i++){
        if(!ball[i]){
            n = i;
            break;
        }
    }
    if(n == -1)
        return;

    ball[n] = (Ball *)malloc(sizeof(Ball));
    if(type<0 && ball[-type - 1]){
        ball[n]->x = ball[-type - 1]->x;
        ball[n]->y = ball[-type - 1]->y;
        ball[n]->type = rand() % 6;
    } else{
        ball[n]->x = palette->pos + palette->w/2 - BALL_W/2;
        ball[n]->y = fieldBreakpoint - BALL_H;
        ball[n]->type = type;
    }
    ball[n]->angle = -1.0 * (rand() % 40 + 30)/100.0;
    ball[n]->speed = BALL_SPEED;


    if(docked){
        for(int i=0; i<MAX_BALLS; i++){
            if(!palette->dockedBalls[i]){
                palette->dockedBalls[i] = ball[n];
                break;
            }
        }
        ball[n]->speed = 0;
    }
    balls++;
}

void Map::addPoints(int dpoints){
    points += dpoints;
}

void Map::removeBall(int n){
    if(ball[n]){
        for(int i=0; i<MAX_BALLS; i++){
            if(palette->dockedBalls[i] && palette->dockedBalls[i] == ball[n]){
                palette->dockedBalls[i] = nullptr;
            }
        }

        delete(ball[n]);
        ball[n] = nullptr;
        balls--;
    }
}


void Map::undockBalls(){
    for(int i=0; i<MAX_BALLS; i++){
        if(palette->dockedBalls[i]){
            palette->dockedBalls[i]->speed = BALL_SPEED;
            palette->dockedBalls[i]->angle = -1.0 * (rand() % 40 + 30)/100.0;
            palette->dockedBalls[i] = nullptr;
        }
    }
}

Block* Map::getBlock(int x, int y){
    if(x>=0 && x<BLOCKS_W && y>=0 && y<BLOCKS_H){
        return block[x][y];
    } else{
        return nullptr;
    }
}

void Map::setBlock(int x, int y, char _type){
    if(x>=0 && x<BLOCKS_W && y>=0 && y<BLOCKS_H){
        block[x][y]->type = _type;
    }
}

Palette* Map::getPalette(){
    return palette;
}


void Map::setPalette(char _type, char _big, bool center){
    palette->type = _type;
    palette->big = _big;

    if(_big == PALETTE_SMALL){
        palette->boundLeft = 0;
        palette->boundRight = fieldWidth - PALETTE_SW;
        palette->w = PALETTE_SW;
        palette->h = PALETTE_SH;
        if(center)
            palette->pos = (fieldWidth- 2*BLOCK_W)/2 - PALETTE_SW/2;
    } else if(_big == PALETTE_BIG){
        palette->boundLeft = 0;
        palette->boundRight = fieldWidth - PALETTE_BW + BALL_W;
        palette->w = PALETTE_BW;
        palette->h = PALETTE_BH;
        if(center)
            palette->pos = (fieldWidth- 2*BLOCK_W)/2 - PALETTE_BW/2;
    }
}

void Map::movePalette(int delta){
    if(palette->pos+delta < palette->boundLeft || palette->pos+delta > palette->boundRight)
        return;
    palette->pos += delta;
    for(int i=0; i<MAX_BALLS; i++){
        if(palette->dockedBalls[i]){
            palette->dockedBalls[i]->x += delta;
        }
    }
}

int Map::getBallAmount(){
    return balls;
}

Ball* Map::getBall(int n){
    if(n>=0 && n<MAX_BALLS && ball[n]){
        return ball[n];
    }
    return nullptr;
}

// Top or bottom collision
float shiftAngleH(float angle){
    return -angle;
}

/* Angle is a float between (-1.0; 1.0> */
// Left or right collision
float shiftAngleV(float angle){
    if(angle>0){
        return 1.0-angle;
    }
    if(angle<0){
        return -1.0-angle;
    }
    return -angle;
}

void Map::calcBall(int n){
    if(n>=0 && n<MAX_BALLS && ball[n]){
        float dx = cos(ball[n]->angle*PI)*ball[n]->speed;
        float dy = sin(ball[n]->angle*PI)*ball[n]->speed;
        ball[n]->x += dx;
        ball[n]->y += dy;
        // Border collision
            if(ball[n]->x<=0 || ball[n]->x>=fieldWidth){
                ball[n]->angle = shiftAngleV(ball[n]->angle);
                ball[n]->x -= dx;
                if(ACCELERATE_ON_BOUNCE)
                    ball[n]->speed += ACCELERATE_ON_BOUNCE;
            } else if(ball[n]->y<=0){
                ball[n]->angle = shiftAngleH(ball[n]->angle);
                ball[n]->y -= dy;
                if(ACCELERATE_ON_BOUNCE)
                    ball[n]->speed += ACCELERATE_ON_BOUNCE;
            }
        // Palette collision
            if(ball[n]->y >= fieldBreakpoint && ball[n]->y <= fieldBreakpoint + palette->h
                    && ball[n]->x >= palette->pos && ball[n]->x <= palette->pos + palette->w){
                if(stickyPalette){
                    for(int i=0; i<MAX_BALLS; i++){
                        if(!palette->dockedBalls[i]){
                            float palx = ball[n]->x - palette->pos;
                            ball[n]->angle = -0.9 + (palx / palette->w) * 0.7;
                            ball[n]->y = PALETTE_ROW*BLOCK_H - BALL_H;
                            ball[n]->speed = 0;
                            palette->dockedBalls[i] = ball[n];
                            break;
                        }
                    }

                } else{
                    ball[n]->y = fieldBreakpoint - BALL_H;
                    float palx = ball[n]->x - palette->pos;
                    ball[n]->angle = -0.9 + (palx / palette->w) * 0.7;
                }
            }
        // Nearby block collision
            int bx = ball[n]->x / BLOCK_W;
            int by = ball[n]->y / BLOCK_H;
            int px = (ball[n]->x-dx) / BLOCK_W;
            int py = (ball[n]->y-dy) / BLOCK_H;
            if(bx >= 0 && bx < BLOCKS_W && by >= 0 && by < BLOCKS_H) {
                if(block[bx][by]->type != EMPTY_BLOCK_ID){
                    if(block[bx][by]->type != BLOCK_BLOCKY_ID) deleteBlock(bx, by);
                    if(bx == px){
                        ball[n]->angle = shiftAngleH(ball[n]->angle);
                        ball[n]->x -= dx;
                        if(ACCELERATE_ON_BOUNCE && ball[n]->speed < BALL_MAX_SPEED)
                            ball[n]->speed += ACCELERATE_ON_BOUNCE;
                    }
                    if(by == py){
                        ball[n]->angle = shiftAngleV(ball[n]->angle);
                        ball[n]->y -= dy;
                        if(ACCELERATE_ON_BOUNCE && ball[n]->speed < BALL_MAX_SPEED)
                            ball[n]->speed += ACCELERATE_ON_BOUNCE;
                    }

                }

            }

        // Fly through breakpoint
            if(ball[n]->y > fieldHeight){
                //ball[n]->angle = shiftAngleH(ball[n]->angle);
                removeBall(n);
            }
    }
    if(balls<=0) {
        lose = true;
        lives--;
    } else if(blockAmount <= 0){
        lose = true;
    }
}

/* Register an award to remove after duration time */
void Map::registerAward(char type, int duration){
    for(int i=0; i< MAX_AWARDS; i++){
        if(award[i] && award[i]->type == type){
            return;
        }
    }
    for(int i=0; i< MAX_AWARDS; i++){
        if(!award[i]){
            award[i] = (Award *)malloc(sizeof(Award));
            award[i]->activationTime = SDL_GetTicks();
            award[i]->type = type;
            award[i]->duration = duration;
            break;
        }
    }
}

/* Parse catched entity(award) */
void Map::parseAward(int n){
    switch(entity[n]->type){
        default: break;
        case AWARD_TRIPLEBALL:
            for(int i=0; i<MAX_BALLS; i++){
                if(ball[i]){
                    createBall(-i - 1, false);
                    createBall(-i - 1, false);
                    break;
                }
            }
            break;
        case AWARD_LIFE:
            lives++;
            if(lives > LIVES) maxLives = lives;
            break;
        case AWARD_BIGPALETTE:
            registerAward(AWARD_BIGPALETTE, 20000);
            setPalette(palette->type, PALETTE_BIG, false);
            break;
        case AWARD_STICKYPALETTE:
            registerAward(AWARD_STICKYPALETTE, 0);
            setPalette(PALETTE_STICKY, palette->big, false);
            stickyPalette = true;
            break;
        case AWARD_SPEEDDOWN:
            registerAward(AWARD_SPEEDDOWN, 20000);
            for(int i=0; i<MAX_BALLS; i++){
                if(ball[i]){
                    ball[i]->speed = BALL_SPEED/2;
                }
            }
            break;
    }
}

/* Check duration on registered awards
 * duration=0 means that award will stay until the life/level over
 * duration<0 means that award will be removed right now */
void Map::calcAward(int n){
    if(award[n]){
        if((award[n]->activationTime + award[n]->duration < SDL_GetTicks() && award[n]->duration != 0) || award[n]->duration<0){
            switch(award[n]->type){
                default: break;
                case AWARD_BIGPALETTE:
                    setPalette(palette->type, PALETTE_SMALL, false);
                    break;
                case AWARD_STICKYPALETTE:
                    stickyPalette = false;
                    setPalette(PALETTE_DEFAULT, palette->big, false);
                    break;
                case AWARD_SPEEDDOWN:
                    for(int i=0; i<MAX_BALLS; i++){
                        if(ball[i]){
                            if(ball[i]->speed>0) ball[i]->speed = BALL_SPEED;
                        }
                    }
                    break;
            }
            delete award[n];
            award[n] = nullptr;
        }
    }
}

void Map::calcEntity(int n){
    if(entity[n]){
        entity[n]->y += AWARD_SPEED;
        if(entity[n]->y >= fieldBreakpoint && entity[n]->y <= fieldBreakpoint + palette->h
                && entity[n]->x - ICON_W >= palette->pos && entity[n]->x - ICON_W <= palette->pos + palette->w){
            parseAward(n);
            removeEntity(n);
        }
    }
}

void Map::deleteBlock(int x, int y){
    points += block[x][y]->type * POINTS_MULTIPLIER;
    block[x][y]->type = EMPTY_BLOCK_ID;
    blockAmount--;

    // Random awards
    if(rand() % AWARD_PROPABILITY == 0){
        switch(rand() % 6){
            default: break;
            case AWARD_LIFE:
                createAward(x*BLOCK_W, y*BLOCK_H, AWARD_LIFE, ICON_HEART);
                addPoints(PRIZE_LIFE);
                break;
            case AWARD_TRIPLEBALL:
                createAward(x*BLOCK_W, y*BLOCK_H, AWARD_TRIPLEBALL, ICON_TRIPLEBALL);
                addPoints(PRIZE_TRIPLEBALL);
                break;
            case AWARD_BIGPALETTE:
                createAward(x*BLOCK_W, y*BLOCK_H, AWARD_BIGPALETTE, ICON_BIGPALETTE);
                addPoints(PRIZE_BIGPALETTE);
                break;
            case AWARD_STICKYPALETTE:
                createAward(x*BLOCK_W, y*BLOCK_H, AWARD_STICKYPALETTE, ICON_STICKYPALETTE);
                addPoints(PRIZE_STICKYPALETTE);
                break;
            case AWARD_SPEEDDOWN:
                createAward(x*BLOCK_W, y*BLOCK_H, AWARD_SPEEDDOWN, ICON_SPEEDDOWN);
                addPoints(PRIZE_SPEEDDOWN);
                break;

        }
    }
}

void Map::doPhysics(){
    for(int i=0; i< MAX_BALLS; i++){
        if(ball[i])
            calcBall(i);
    }

    for(int i=0; i< MAX_ENTITIES; i++){
        if(entity[i]){
            calcEntity(i);
        }
    }
    for(int i=0; i< MAX_AWARDS; i++){
        if(award[i]){
            calcAward(i);
        }
    }
}

void Map::setField(int _w, int _h){
    fieldWidth = _w;
    fieldHeight = _h;
}

bool Map::isLose(){
    return lose;
}

void Map::setLose(bool _lose){
    lose = _lose;
}

int Map::getPoints(){
    return points;
}

int Map::getLives(){
    return lives;
}

void Map::setLives(int _lives){
    lives = _lives;
};

int Map::createAward(int x, int y, char type, char icon){
    for(int i=0; i< MAX_ENTITIES; i++){
        if(entity[i] != nullptr){
            entity[i] = (Entity *)malloc(sizeof(Entity));
            entity[i]->type = type;
            entity[i]->x = x;
            entity[i]->y = y;
            entity[i]->icon = icon;
            return i;
        }
    }

    return -1;
}

void Map::removeEntity(int n){
    delete entity[n];
    entity[n] = nullptr;
}

Entity* Map::getEntity(int n){
    return entity[n];
}