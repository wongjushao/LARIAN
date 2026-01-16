#include <raylib.h>
#include <raymath.h>
#include <rcamera.h>
#include <entt/entt.hpp>
#include "world/World.hpp"
#include "ecs/components/Components.hpp"
#include "ecs/components/CharacterComponents.hpp"
#include "ecs/systems/WorldSystem.hpp"
#include "ecs/systems/RenderSystem.hpp"
#include "ecs/systems/CharacterSystem.hpp"
#include <iostream>

// Constants for rendering
constexpr int SCREEN_WIDTH = 1920;
constexpr int SCREEN_HEIGHT = 1080;
constexpr float BLOCK_SIZE = 1.0f;  // Size of each block in 3D space

// Camera settings
Camera3D camera = { 0 };

// Viewing options
bool showSurfaceOnly = false;
bool showUndergroundOnly = false;
int currentLayer = -1;  // -1 means show all layers
bool wireframeMode = false;

// ECS
entt::registry registry;
ECS::WorldSystem worldSystem;
ECS::RenderSystem renderSystem;
ECS::CharacterSystem characterSystem;

// Character entity
entt::entity playerCharacter = entt::null;
bool useCharacterCamera = true;  // Toggle between free camera and character follow

void InitializeCamera() {
    camera.position = (Vector3){ 50.0f, 40.0f, 50.0f };
    camera.target = (Vector3){ 18.0f, 18.0f, 18.0f };
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;
}

// Draw world using ECS
void DrawWorld() {
    if (currentLayer >= 0) {
        // Render single layer
        renderSystem.RenderLayer(registry, currentLayer);
    } else if (showSurfaceOnly) {
        // Render surface only (layers 0-2)
        renderSystem.RenderSurfaces(registry);
    } else if (showUndergroundOnly) {
        // Render underground only (layers 3+)
        renderSystem.RenderUnderground(registry);
    } else {
        // Render all visible blocks
        renderSystem.Render(registry);
    }
}

// Draw UI overlay
void DrawUI() {
    int uiX = 10;
    int uiY = 10;
    int lineHeight = 25;
    
    // Background panel
    DrawRectangle(5, 5, 450, 550, ColorAlpha(BLACK, 0.7f));
    
    // Title
    DrawText("3D VOXEL WORLD (ECS)", uiX, uiY, 24, RAYWHITE);
    uiY += lineHeight + 10;
    
    // ECS info
    DrawText(TextFormat("Total Entities: %d", (int)registry.storage<entt::entity>().size()), 
             uiX, uiY, 18, LIGHTGRAY);
    uiY += lineHeight;
    
    DrawText(TextFormat("Renderable: %d", (int)renderSystem.GetRenderableCount(registry)), 
             uiX, uiY, 18, LIGHTGRAY);
    uiY += lineHeight;
    
    // View mode
    const char* viewMode = "All Layers";
    if (currentLayer >= 0) {
        viewMode = TextFormat("Layer %d", currentLayer);
    } else if (showSurfaceOnly) {
        viewMode = "Surface Only (0-2)";
    } else if (showUndergroundOnly) {
        viewMode = "Underground (3-35)";
    }
    DrawText(TextFormat("View: %s", viewMode), uiX, uiY, 18, YELLOW);
    uiY += lineHeight;
    
    DrawText(TextFormat("ECS Architecture: EnTT + Raylib"), uiX, uiY, 16, GREEN);
    uiY += lineHeight + 10;
    
    // Legend
    DrawText("LEGEND:", uiX, uiY, 20, RAYWHITE);
    uiY += lineHeight;
    
    int legendX = uiX + 20;
    DrawRectangle(legendX, uiY, 20, 20, BROWN);
    DrawText("Soil (Exposed Surface)", legendX + 30, uiY + 2, 16, LIGHTGRAY);
    uiY += lineHeight;
    
    DrawRectangle(legendX, uiY, 20, 20, GRAY);
    DrawText("Stone", legendX + 30, uiY + 2, 16, LIGHTGRAY);
    uiY += lineHeight;
    
    DrawRectangle(legendX, uiY, 20, 20, GOLD);
    DrawText("Gold (Underground)", legendX + 30, uiY + 2, 16, LIGHTGRAY);
    uiY += lineHeight;
    
    DrawRectangle(legendX, uiY, 20, 20, Color{192, 192, 192, 255});
    DrawText("Silver (Underground)", legendX + 30, uiY + 2, 16, LIGHTGRAY);
    uiY += lineHeight;
    
    DrawRectangleLines(legendX, uiY, 20, 20, DARKGRAY);
    DrawText("Air (Caves)", legendX + 30, uiY + 2, 16, LIGHTGRAY);
    uiY += lineHeight + 10;
    
    // Controls
    DrawText("CONTROLS:", uiX, uiY, 20, RAYWHITE);
    uiY += lineHeight;
    
    DrawText("WASD/Arrows - Move Camera", uiX, uiY, 16, LIGHTGRAY);
    uiY += lineHeight - 5;
    DrawText("Mouse - Look Around", uiX, uiY, 16, LIGHTGRAY);
    uiY += lineHeight - 5;
    DrawText("Scroll - Zoom", uiX, uiY, 16, LIGHTGRAY);
    uiY += lineHeight - 5;
    DrawText("R - Regenerate World", uiX, uiY, 16, GREEN);
    uiY += lineHeight - 5;
    DrawText("P - Print Statistics", uiX, uiY, 16, GREEN);
    uiY += lineHeight - 5;
    DrawText("1 - Show All Layers", uiX, uiY, 16, SKYBLUE);
    uiY += lineHeight - 5;
    DrawText("2 - Surface Only", uiX, uiY, 16, SKYBLUE);
    uiY += lineHeight - 5;
    DrawText("3 - Underground Only", uiX, uiY, 16, SKYBLUE);
    uiY += lineHeight - 5;
    DrawText("Up/Down - Change Layer", uiX, uiY, 16, SKYBLUE);
    uiY += lineHeight - 5;
    DrawText("T - Wireframe Toggle", uiX, uiY, 16, SKYBLUE);
    uiY += lineHeight - 5;
    DrawText("C - Toggle Camera Mode", uiX, uiY, 16, YELLOW);
    uiY += lineHeight - 5;
    DrawText("ESC - Exit", uiX, uiY, 16, RED);
    
    // FPS
    DrawText(TextFormat("FPS: %d", GetFPS()), SCREEN_WIDTH - 100, 10, 20, GREEN);
}

int main() {
    // Initialize window
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "ECS Raylib EnTT - 3D Voxel World (ECS Architecture)");
    SetTargetFPS(60);
    
    // Initialize camera
    InitializeCamera();
    
    // Initialize render system
    renderSystem.SetBlockSize(BLOCK_SIZE);
    
    // Create and generate world
    World::World world;
    world.Generate();
    
    // Populate ECS registry from world
    std::cout << "Populating ECS registry..." << std::endl;
    worldSystem.PopulateFromWorld(registry, world);
    
    // Print initial statistics
    std::cout << "\n3D World generated successfully!" << std::endl;
    world.PrintStatistics();
    worldSystem.PrintStatistics(registry);
    
    // Load character
    const char* characterPath = "../src/assets/Ultimate Platformer Pack - Dec 2021/Character/glTF/Character.gltf";

    // Pick a valid spawn column (for example, center of the world)
    int spawnX = World::WORLD_WIDTH / 2;
    int spawnZ = World::WORLD_DEPTH / 2;

    int surfaceY = world.GetSurfaceLevel(spawnX, spawnZ);

    // Spawn so feet stand on top of that block
    Vector3 startPos = {
        spawnX + 0.5f,
        static_cast<float>(surfaceY) + 1.0f,   // +1.0f: top of the block
        spawnZ + 0.5f
    };

    playerCharacter = characterSystem.CreateCharacter(registry, characterPath, startPos);
    
    if (playerCharacter == entt::null) {
        std::cerr << "Warning: Failed to load character, continuing without character." << std::endl;
    }
    
    // Main game loop
    while (!WindowShouldClose()) {
        // ===== UPDATE =====
        
        float deltaTime = GetFrameTime();
        
        // Update character movement and animations
        if (playerCharacter != entt::null) {
            characterSystem.UpdateMovement(registry, deltaTime);
            characterSystem.UpdateAnimations(registry, deltaTime);
            
            // Update camera to follow character (if enabled)
            if (useCharacterCamera) {
                characterSystem.UpdateCamera(registry, camera);
            }
        }
        
        // Camera controls (only if not using character camera)
        if (!useCharacterCamera) {
            UpdateCamera(&camera, CAMERA_THIRD_PERSON);
        }
        
        // Toggle camera mode
        if (IsKeyPressed(KEY_C)) {
            useCharacterCamera = !useCharacterCamera;
            if (!useCharacterCamera) {
                // Reset to free camera
                InitializeCamera();
            }
        }
        
        // Handle input
        if (IsKeyPressed(KEY_R)) {
            std::cout << "\nRegenerating world..." << std::endl;
            world.Generate();
            world.PrintStatistics();
            
            // Repopulate ECS registry
            std::cout << "Repopulating ECS registry..." << std::endl;
            worldSystem.PopulateFromWorld(registry, world);
            worldSystem.PrintStatistics(registry);
        }
        
        if (IsKeyPressed(KEY_P)) {
            world.PrintStatistics();
            worldSystem.PrintStatistics(registry);
        }
        
        // View mode controls
        if (IsKeyPressed(KEY_ONE)) {
            showSurfaceOnly = false;
            showUndergroundOnly = false;
            currentLayer = -1;
        }
        
        if (IsKeyPressed(KEY_TWO)) {
            showSurfaceOnly = true;
            showUndergroundOnly = false;
            currentLayer = -1;
        }
        
        if (IsKeyPressed(KEY_THREE)) {
            showSurfaceOnly = false;
            showUndergroundOnly = true;
            currentLayer = -1;
        }
        
        // Layer navigation
        if (IsKeyPressed(KEY_UP)) {
            if (currentLayer < 0) {
                currentLayer = 0;
            } else if (currentLayer < world.GetHeight() - 1) {
                currentLayer++;
            }
            showSurfaceOnly = false;
            showUndergroundOnly = false;
        }
        
        if (IsKeyPressed(KEY_DOWN)) {
            if (currentLayer < 0) {
                currentLayer = world.GetHeight() - 1;
            } else if (currentLayer > 0) {
                currentLayer--;
            }
            showSurfaceOnly = false;
            showUndergroundOnly = false;
        }
        
        // Wireframe toggle
        if (IsKeyPressed(KEY_T)) {
            wireframeMode = !wireframeMode;
            renderSystem.ToggleWireframe(registry, wireframeMode);
        }
        
        // ===== DRAW =====
        
        BeginDrawing();
        ClearBackground(DARKGRAY);
        
        BeginMode3D(camera);
        
        // Draw grid
        DrawGrid(40, 1.0f);
        
        // Draw the world using ECS
        DrawWorld();
        
        // Draw character
        if (playerCharacter != entt::null) {
            characterSystem.Render(registry);
        }
        
        EndMode3D();
        
        // Draw UI
        DrawUI();
        
        EndDrawing();
    }
    
    // Cleanup
    if (playerCharacter != entt::null) {
        characterSystem.UnloadCharacter(registry, playerCharacter);
    }
    
    CloseWindow();
    
    return 0;
}
