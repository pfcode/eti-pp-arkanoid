#ifndef _MAP_H_
#define _MAP_H_

#include "defines.h"

struct Ball{
    char type;
    float angle;    // clockwise
    float x;
    float y;
    float speed;
};

struct Palette{
    char type;
    char big;
    int pos;
    int boundLeft;
    int boundRight;
    int h;
    int w;
    Ball *dockedBalls[MAX_BALLS];
};

struct Block{
    char type;
};

struct Level{
    Block *block[BLOCKS_W][BLOCKS_H];
    int blockAmount;
    Level *nextLevel;
};

struct Entity{
    char type;
    float x;
    float y;
    char icon;
};

struct Award{
    char type;
    int activationTime;
    int duration;
};

class Map{
private:
    Block *block[BLOCKS_W][BLOCKS_H];
    Palette *palette;
    Ball *ball[MAX_BALLS];
    Entity *entity[MAX_ENTITIES];
    Award *award[MAX_AWARDS];
    int balls, points, lives;
    int fieldWidth, fieldHeight, fieldBreakpoint;
    bool lose;
    bool stickyPalette;

    void resetVariables();

public:
    Map();
    ~Map();

    int blockAmount, levelAmount, currentLevel;
    int maxLives;
    Level *level;

    bool loadMap(const char* path);
    Block* getBlock(int x, int y);
    Palette* getPalette();
    void setBlock(int x, int y, char _type);
    void setPalette(char _type, char _big, bool center);
    void movePalette(int delta);
    int getBallAmount();
    Ball *getBall(int n);
    void calcBall(int n);
    void calcEntity(int n);
    void doPhysics();
    void setField(int _w, int _h);
    void deleteBlock(int x, int y);
    void createBall(int type, bool docked);
    void removeBall(int n);
    void undockBalls();
    int getPoints();
    int getLives();
    void addPoints(int dpoints);
    void setLives(int _lives);
    bool isLose();
    void setLose(bool _lose);
    bool nextLevel();
    void restartLevel();
    void removeEntity(int n);
    int createAward(int x, int y, char type, char icon);
    Entity *getEntity(int n);
    void parseAward(int n);
    void registerAward(char type, int duration);
    void calcAward(int n);
};

#endif
