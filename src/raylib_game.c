/*******************************************************************************************
*
*   raylib gamejam template
*
*   Code licensed under an unmodified zlib/libpng license, which is an OSI-certified,
*   BSD-like license that allows static linking with closed source software
*
*   Copyright (c) 2022-2026 Ramon Santamaria (@raysan5)
*
********************************************************************************************/

#include "raylib.h"

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>      // Emscripten library
#endif

#include <math.h>                           // Required for: sqrtf()
#include <stdio.h>                          // Required for: printf()
#include <stdlib.h>                         // Required for: 
#include <string.h>                         // Required for:

//----------------------------------------------------------------------------------
// Defines and Macros
//----------------------------------------------------------------------------------
// Simple log system to avoid printf() calls if required
// NOTE: Avoiding those calls, also avoids const strings memory usage
#define SUPPORT_LOG_INFO
#if defined(SUPPORT_LOG_INFO)
    #define LOG(...) printf(__VA_ARGS__)
#else
    #define LOG(...)
#endif

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------
typedef enum { 
    SCREEN_LOGO = 0, 
    SCREEN_TITLE, 
    SCREEN_GAMEPLAY, 
    SCREEN_ENDING
} GameScreen;

// https://www.redblobgames.com/grids/hexagons/#coordinates-cube
typedef struct {
    Vector2 pos;
} Hex;

// TODO: Define your custom data types here

//----------------------------------------------------------------------------------
// Global Variables Definition (local to this module)
//----------------------------------------------------------------------------------
static const int screenWidth = 720;
static const int screenHeight = 720;

static RenderTexture2D target = { 0 };  // Render texture to render our game
static int frameCounter = 0;

static const int hexRadius = 32;
static const int hexWidth = 55;
static const int hexHeigth = 64;

static const int hexMapCenter = 3;
static const int hexQs = 7;
static const int hexRs = 7;
static const int hexSs = 7;

// TODO: Define global variables here, recommended to make them static

static Hex ***hexMap;
static Texture2D hexSprite;
static Rectangle hexRec;

//----------------------------------------------------------------------------------
// Module Functions Declaration
//----------------------------------------------------------------------------------
static void UpdateDrawFrame(void);      // Update and Draw one frame
static void SetupHexMap();
static void DrawHexMap();
static void ClearHexMap();

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
#if !defined(_DEBUG)
    SetTraceLogLevel(LOG_NONE);         // Disable raylib trace log messages
#endif

    // Initialization
    //--------------------------------------------------------------------------------------
    InitWindow(screenWidth, screenHeight, "raylib gamejam template");
    
    // TODO: Load resources / Initialize variables at this point
    hexSprite = LoadTexture("D:\\GameDev\\RayLibJam\\ultra-space-robot-transform-sequence-game\\src\\resources\\HexTile.png");
    hexRec.width = hexWidth;
    hexRec.height = hexHeigth;
    SetupHexMap();
    
    // Render texture to draw, enables screen scaling
    // NOTE: If screen is scaled, mouse input should be scaled proportionally
    target = LoadRenderTexture(screenWidth, screenHeight);
    SetTextureFilter(target.texture, TEXTURE_FILTER_BILINEAR);

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(UpdateDrawFrame, 60, 1);
#else
    SetTargetFPS(60);     // Set our game frames-per-second

    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button
    {
        UpdateDrawFrame();
    }
#endif

    // De-Initialization
    //--------------------------------------------------------------------------------------
    UnloadRenderTexture(target);
    UnloadTexture(hexSprite);
    ClearHexMap();
    
    // TODO: Unload all loaded resources at this point

    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}

//--------------------------------------------------------------------------------------------
// Module Functions Definition
//--------------------------------------------------------------------------------------------
// Update and draw frame
void UpdateDrawFrame(void)
{
    // Update
    //----------------------------------------------------------------------------------
    // TODO: Update variables / Implement example logic at this point
   
    frameCounter++;
    //----------------------------------------------------------------------------------

    // Draw
    //----------------------------------------------------------------------------------
    // Render game screen to a texture, 
    // it could be useful for scaling or further shader postprocessing
    BeginTextureMode(target);
        ClearBackground(RAYWHITE);
        
        DrawHexMap();
        
    EndTextureMode();
    
    // Render to screen (main framebuffer)
    BeginDrawing();
        ClearBackground(RAYWHITE);
        
        // Draw render texture to screen, scaled if required
        DrawTexturePro(target.texture, (Rectangle){ 0, 0, (float)target.texture.width, -(float)target.texture.height }, 
            (Rectangle){ 0, 0, (float)target.texture.width, (float)target.texture.height }, (Vector2){ 0, 0 }, 0.0f, WHITE);

        // TODO: Draw everything that requires to be drawn at this point, maybe UI?

    EndDrawing();
    //----------------------------------------------------------------------------------  
}

void SetupHexMap()
{
    hexMap = (Hex**)MemAlloc(hexQs * sizeof(Hex*));

    for (int q = 0; q < hexQs; q++) {
        hexMap[q] = (Hex*)MemAlloc(hexRs * sizeof(Hex));

        for (int r = 0; r < hexRs; r++) {
            hexMap[q][r] = (Hex**)MemAlloc(hexSs * sizeof(Hex));
        }
    }

    float horizontalSpacing = 0.48 * hexWidth;
    float verticalSpacing = 0.71 * hexHeigth;
    for (int q = 0; q < hexQs; q++) {
        for (int r = 0; r < hexRs; r++) {
            for (int s = 0; s < hexSs; s++) {
                Vector2 pos = { (s - q) * horizontalSpacing, r * verticalSpacing};
                hexMap[q][r][s].pos = pos;
            }
        }
    }
}

void DrawHexMap()
{
    for (int q = 0; q < hexQs; q++) {
        for (int r = 0; r < hexRs; r++) {
            for (int s = 0; s < hexSs; s++) {
                if (q != 0 && q == r && r == s)
                    continue;

                if ((q + r + s) % 2 != 0)
                    continue;

                Vector2 pos = hexMap[q][r][s].pos;
                DrawTextureRec(hexSprite, hexRec, pos, WHITE);
            }
        }
    }
}

void ClearHexMap()
{
    for (int q = 0; q < hexQs; q++) {
        for (int r = 0; r < hexRs; r++) {
            MemFree(hexMap[q][r]);
        }

        MemFree(hexMap[q]);
    }

    MemFree(hexMap);
}
