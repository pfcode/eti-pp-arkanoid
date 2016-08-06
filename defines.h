// General
#define INIT_WIDTH 640
#define INIT_HEIGHT 480
#define WINDOW_TITLE "Arkanoid - 155059"
#define DEBUG_MODE 0
#define LIVES 3
#define LEVEL_FILE "levels.txt"

// Style & resources
#define ABSOLUTE_BACKGROUND 0x222222
#define RESOURCE_CHARSET "./res/cs8x8.bmp"
#define RESOURCE_TILESET "./res/tileset.bmp"
#define RESOURCE_BACKGROUND "./res/drums.bmp"

// Block settings
#define BLOCKS_W 24
#define BLOCKS_H 16
#define BLOCK_W 24
#define BLOCK_H 16
#define EMPTY_BLOCK_ID 79
#define BLOCK_BLOCKY_ID 24

// Ball settings
#define BALL_W 8
#define BALL_H 8
#define BALL_SPEED 0.8
#define BALL_MAX_SPEED 1.3
#define MAX_BALLS 30
#define ACCELERATE_ON_BOUNCE 0.01

// Palette settings
#define PALETTE_SPEED 2
#define PALETTE_SW 48       // Small palette width
#define PALETTE_SH 8        // Small palette height
#define PALETTE_BW 96       // Big palette width
#define PALETTE_BH 8        // Big palette height
#define PALETTE_ROW 25      // Palette position

#define PALETTE_SMALL 1
#define PALETTE_BIG 2

#define PALETTE_DEFAULT 0
#define PALETTE_STICKY 1
#define PALETTE_GUN 2
#define PALETTE_BLUE 3

// Icon settings
#define ICON_W 16
#define ICON_H 16
#define ICON_HEART 0
#define ICON_HEART_DARK 1
#define ICON_TRIPLEBALL 2
#define ICON_BIGPALETTE 3
#define ICON_STICKYPALETTE 4
#define ICON_SPEEDDOWN 5

// Points
#define POINTS_NEXTLEVEL 10000
#define POINTS_MULTIPLIER 10

// Entities
#define MAX_ENTITIES 20

// Awards
#define AWARD_PROPABILITY 20    // More is less
#define AWARD_SPEED 0.3
#define MAX_AWARDS 20

#define AWARD_TRIPLEBALL 0
#define AWARD_LIFE 1
#define AWARD_BIGPALETTE 2
#define AWARD_STICKYPALETTE 3
#define AWARD_SPEEDDOWN 4

#define PRIZE_TRIPLEBALL 5000
#define PRIZE_LIFE 2000
#define PRIZE_BIGPALETTE 3000
#define PRIZE_STICKYPALETTE 5000
#define PRIZE_SPEEDDOWN 3000
