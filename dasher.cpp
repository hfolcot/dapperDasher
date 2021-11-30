#include "raylib.h"

struct AnimData
{
    Rectangle rec;
    Vector2 pos;
    int frame;
    float updateTime;
    float runningTime;
};

bool isOnGround(AnimData data, int windowHeight)
{
    return data.pos.y >= windowHeight - data.rec.height;
}

AnimData updateAnimData(AnimData data, float deltaTime, int maxFrame)
{
    //update running time
    data.runningTime += deltaTime;
    if (data.runningTime >= data.updateTime)
    {
        data.runningTime = 0.0;
        //update animation frame
        data.rec.x = data.frame * data.rec.width;
        data.frame++;
        if (data.frame > maxFrame)
        {
            data.frame = 0;
        }
    }
    return data;
}

int main()
{
    int windowDimensions[2];
    windowDimensions[0] = 2048;
    windowDimensions[1] = 1480;
    InitWindow(windowDimensions[0], windowDimensions[1], "Dapper Dasher");

    const int gravity = 4'000;

    Texture2D nebula = LoadTexture("textures/12_nebula_spritesheet.png");
    float bgX{};
    float mgX{};
    float fgX{};
    const int sizeOfNebulae{6};
    AnimData nebulae[sizeOfNebulae]{};

    for (int i = 0; i < sizeOfNebulae; i++)
    {
        nebulae[i].rec.x = 0.0;
        nebulae[i].rec.y = 0.0;
        nebulae[i].rec.width = nebula.width / 8;
        nebulae[i].rec.height = nebula.height / 8;
        nebulae[i].pos.y = windowDimensions[1] - nebula.height / 8;
        nebulae[i].pos.x = windowDimensions[0] + ((windowDimensions[0] * 0.5) * i);
        nebulae[i].frame = 0;
        nebulae[i].runningTime = 0.0;
        nebulae[i].updateTime = 0.0;
    }

    float finishLine{nebulae[sizeOfNebulae - 1].pos.x};

    int nebVel{-600};

    Texture2D scarfy = LoadTexture("textures/scarfy.png");
    AnimData scarfyData{
        {0.0, 0.0, scarfy.width / 6, scarfy.height},
        {windowDimensions[0] / 2 - scarfyData.rec.width / 2, windowDimensions[1] - scarfyData.rec.height},
        0,
        1.0 / 12.0,
        0.0};

    int velocity = 0;
    const int jumpVel = -2400;

    Texture2D background = LoadTexture("textures/far-buildings.png");
    Texture2D midground = LoadTexture("textures/back-buildings.png");
    Texture2D foreground = LoadTexture("textures/foreground.png");
    bool collision{};
    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        const float dT = GetFrameTime();

        BeginDrawing();
        ClearBackground(WHITE);

        bgX -= 80 * dT;
        if (bgX <= -background.width * 8)
        {
            bgX = 0.0;
        }

        mgX -= 160 * dT;
        if (mgX <= -midground.width * 8)
        {
            mgX = 0.0;
        }

        fgX -= 240 * dT;
        if (fgX <= -foreground.width * 8)
        {
            fgX = 0.0;
        }

        Vector2 bg1Pos{bgX, 0.0};
        DrawTextureEx(background, bg1Pos, 0.0, 8.0, WHITE);

        Vector2 bg2Pos{bgX + background.width * 8, 0.0};
        DrawTextureEx(background, bg2Pos, 0.0, 8.0, WHITE);

        Vector2 mg1Pos{mgX, 0.0};
        DrawTextureEx(midground, mg1Pos, 0.0, 8.0, WHITE);

        Vector2 mg2Pos{mgX + midground.width * 8, 0.0};
        DrawTextureEx(midground, mg2Pos, 0.0, 8.0, WHITE);

        Vector2 fg1Pos{fgX, 0.0};
        DrawTextureEx(foreground, fg1Pos, 0.0, 8.0, WHITE);

        Vector2 fg2Pos{fgX + foreground.width * 8, 0.0};
        DrawTextureEx(foreground, fg2Pos, 0.0, 8.0, WHITE);

        if (isOnGround(scarfyData, windowDimensions[1]))
        {
            velocity = 0;
        }
        else
        {
            velocity += gravity * dT;
        }
        if (IsKeyPressed(KEY_SPACE) && isOnGround(scarfyData, windowDimensions[1]))
        {
            velocity += jumpVel;
        }

        for (int i = 0; i < sizeOfNebulae; i++)
        {
            nebulae[i].pos.x += nebVel * dT;
        }

        // Update actor positions
        scarfyData.pos.y += velocity * dT;
        finishLine += nebVel * dT;

        for (int i = 0; i < sizeOfNebulae; i++)
        {
            nebulae[i] = updateAnimData(nebulae[i], dT, 7);
        }

        if (isOnGround(scarfyData, windowDimensions[1]))
        {
            scarfyData = updateAnimData(scarfyData, dT, 5);
        }

        for (AnimData nebula : nebulae)
        {
            float pad{50};
            Rectangle nebRec{
                nebula.pos.x + pad,
                nebula.pos.y + pad,
                nebula.rec.width - 2 * pad,
                nebula.rec.height - 2 * pad};
            Rectangle scarfyRec{
                scarfyData.pos.x,
                scarfyData.pos.y,
                scarfyData.rec.width,
                scarfyData.rec.height};
            if (CheckCollisionRecs(nebRec, scarfyRec))
            {
                collision = true;
            }
        }

        if (collision)
        {
            DrawText("Game Over!", windowDimensions[0] / 2 - 200, windowDimensions[1] / 2, 100, RED);
        }
        else if (!collision && finishLine < scarfyData.pos.x)
        {
            DrawText("You win!", windowDimensions[0] / 2 - 200, windowDimensions[1] / 2, 100, GREEN);
        }
        else
        {

            for (int i = 0; i < sizeOfNebulae; i++)
            {
                DrawTextureRec(nebula, nebulae[i].rec, nebulae[i].pos, WHITE);
            }

            //Draw scarfy
            DrawTextureRec(scarfy, scarfyData.rec, scarfyData.pos, WHITE);
        }
        EndDrawing();
    }
    UnloadTexture(nebula);
    UnloadTexture(scarfy);
    UnloadTexture(background);
    UnloadTexture(midground);
    UnloadTexture(foreground);
    CloseWindow();
}