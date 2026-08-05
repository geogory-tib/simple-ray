/*******************************************************************************************
*
*   SetupPrompt v1.0.0 - Tool Description
*
*   LICENSE: Propietary License
*
*   Copyright (c) 2022 geogory. All Rights Reserved.
*
*   Unauthorized copying of this file, via any medium is strictly prohibited
*   This project is proprietary and confidential unless the owner allows
*   usage in any other form by expresely written permission.
*
**********************************************************************************************/

#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

//----------------------------------------------------------------------------------
// Controls Functions Declaration
//----------------------------------------------------------------------------------


//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main()
{
    // Initialization
    //---------------------------------------------------------------------------------------
    int screenWidth = 800;
    int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "setup_prompt");

    // setup_prompt: controls initialization
    //----------------------------------------------------------------------------------
    bool texture_folder_boxEditMode = false;
    char texture_folder_boxText[128] = "";
    bool output_folder_boxEditMode = false;
    char output_folder_boxText[128] = "";
    bool submit_boxPressed = false;
    //----------------------------------------------------------------------------------

    SetTargetFPS(60);
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        // TODO: Implement required update logic
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR))); 

            // raygui: controls drawing
            //----------------------------------------------------------------------------------
            GuiGroupBox((Rectangle){ 0, 48, 432, 232 }, "Provide Texture, Sprite Textures and output path");
            if (GuiTextBox((Rectangle){ 24, 88, 240, 40 }, texture_folder_boxText, 128, texture_folder_boxEditMode)) texture_folder_boxEditMode = !texture_folder_boxEditMode;
            if (GuiTextBox((Rectangle){ 24, 160, 240, 40 }, output_folder_boxText, 128, output_folder_boxEditMode)) output_folder_boxEditMode = !output_folder_boxEditMode;
            GuiLabel((Rectangle){ 24, 64, 240, 24 }, "Texture Folder");
            GuiLabel((Rectangle){ 24, 136, 240, 24 }, "Map Output");
            submit_boxPressed = GuiButton((Rectangle){ 304, 248, 120, 24 }, "Submit"); 
            //----------------------------------------------------------------------------------

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}

//------------------------------------------------------------------------------------
// Controls Functions Definitions (local)
//------------------------------------------------------------------------------------

