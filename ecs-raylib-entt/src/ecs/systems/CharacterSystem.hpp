#ifndef CHARACTER_SYSTEM_H
#define CHARACTER_SYSTEM_H

#include <entt/entt.hpp>
#include <raylib.h>
#include <iostream>
#include "../components/CharacterComponents.hpp"

namespace ECS {

class CharacterSystem {
public:
    CharacterSystem() = default;
    
    entt::entity CreateCharacter(entt::registry& registry, const char* modelPath, Vector3 startPos) {
        std::cout << "Loading character model from: " << modelPath << std::endl;
        
        auto character = registry.create();
        
        Model model = LoadModel(modelPath);
        if (model.meshCount == 0) {
            std::cerr << "Failed to load character model!" << std::endl;
            registry.destroy(character);
            return entt::null;
        }
        
        int animCount = 0;
        ModelAnimation* animations = LoadModelAnimations(modelPath, &animCount);
        
        std::cout << "Loaded model with " << animCount << " animations" << std::endl;
        
        auto& modelComp = registry.emplace<CharacterModel>(character);
        modelComp.model = model;
        modelComp.animations = animations;
        modelComp.animationCount = animCount;
        modelComp.currentAnimation = 9;
        modelComp.currentFrame = 0;
        
        registry.emplace<Transform>(character, startPos);
        
        auto& transform = registry.get<Transform>(character);
        transform.scale = {0.3f, 0.3f, 0.3f};  // 30% size
        
        auto& animState = registry.emplace<AnimationState>(character);
        animState.currentState = AnimationState::STANDING;
        animState.animationTime = 0.0f;
        
        registry.emplace<Movement>(character);
        
        registry.emplace<CharacterTag>(character);
        
        registry.emplace<CameraFollow>(character);
        
        std::cout << "Character entity created successfully!" << std::endl;
        return character;
    }
    
    // Update character animations
    void UpdateAnimations(entt::registry& registry, float deltaTime) {
        auto view = registry.view<CharacterModel, AnimationState, Movement>();
        
        for (auto entity : view) {
            auto& modelComp = view.get<CharacterModel>(entity);
            auto& animState = view.get<AnimationState>(entity);
            auto& movement = view.get<Movement>(entity);
            
            AnimationState::State desiredState = movement.isMoving ? 
                AnimationState::WALKING : AnimationState::STANDING;
            
            if (desiredState != animState.currentState) {
                animState.currentState = desiredState;
                modelComp.currentAnimation = (int)desiredState;
                modelComp.currentFrame = 0;
                animState.animationTime = 0.0f;
            }
            
            if (modelComp.animationCount > 0 && 
                modelComp.currentAnimation < modelComp.animationCount) {
                
                animState.animationTime += deltaTime;
                
                ModelAnimation anim = modelComp.animations[modelComp.currentAnimation];
                int frameCount = anim.frameCount;
                
                modelComp.currentFrame = (int)(animState.animationTime * 30.0f) % frameCount;
                
                UpdateModelAnimation(modelComp.model, anim, modelComp.currentFrame);
            }
        }
    }
    
    // Handle character input and movement
    void UpdateMovement(entt::registry& registry, float deltaTime) {
        auto view = registry.view<Transform, Movement, CharacterTag>();
        
        for (auto entity : view) {
            auto& transform = view.get<Transform>(entity);
            auto& movement = view.get<Movement>(entity);
            
            // Reset velocity
            movement.velocity = {0.0f, 0.0f, 0.0f};
            movement.isMoving = false;

            // --- rotation first (A/D) ---
            const float turnSpeed = 120.0f; // degrees per second

            if (IsKeyDown(KEY_A)) {
                // turn left (counter‑clockwise)
                transform.rotation.y += turnSpeed * deltaTime;
            }
            if (IsKeyDown(KEY_D)) {
                // turn right (clockwise)
                transform.rotation.y -= turnSpeed * deltaTime;
            }

            // Keep rotation in [0, 360)
            if (transform.rotation.y >= 360.0f) transform.rotation.y -= 360.0f;
            if (transform.rotation.y <   0.0f) transform.rotation.y += 360.0f;

            // --- compute forward vector from yaw ---
            float yawRad = transform.rotation.y * DEG2RAD;
            Vector3 forward = { sinf(yawRad), 0.0f, cosf(yawRad) };

            // --- movement (W/S) ---
            float walkSpeed = movement.speed;        // use your existing speed as walk
            float runSpeed  = movement.speed * 1.8f; // S = backward run

            if (IsKeyDown(KEY_W)) {
                // move forward (walk)
                movement.velocity.x += forward.x * walkSpeed;
                movement.velocity.z += forward.z * walkSpeed;
                movement.isMoving = true;
            }
            if (IsKeyDown(KEY_S)) {
                // move backward (run)
                movement.velocity.x -= forward.x * runSpeed;
                movement.velocity.z -= forward.z * runSpeed;
                movement.isMoving = true;
            }

            // Apply velocity to position
            transform.position.x += movement.velocity.x * deltaTime;
            transform.position.z += movement.velocity.z * deltaTime;
            
            // Keep character within world bounds (36x36 world, blocks are 1.0 size)
            const float worldMin = 0.5f;
            const float worldMax = 35.5f;
            
            if (transform.position.x < worldMin) transform.position.x = worldMin;
            if (transform.position.x > worldMax) transform.position.x = worldMax;
            if (transform.position.z < worldMin) transform.position.z = worldMin;
            if (transform.position.z > worldMax) transform.position.z = worldMax;
        }
    }
    
    // Update camera to follow character
	void UpdateCamera(entt::registry& registry, Camera3D& camera) {
        auto view = registry.view<Transform, CameraFollow, CharacterTag>();
        
        for (auto entity : view) {
            auto& transform = view.get<Transform>(entity);
            auto& cameraFollow = view.get<CameraFollow>(entity);
            
            float angleRad = transform.rotation.y * DEG2RAD;

            // Offset behind character:
            // forward = (sin(yaw), 0, cos(yaw))
            Vector3 forward = { sinf(angleRad), 0.0f, cosf(angleRad) };

            Vector3 desiredPos = {
                transform.position.x - forward.x * cameraFollow.distance,
                transform.position.y + cameraFollow.height,
                transform.position.z - forward.z * cameraFollow.distance
            };

            const float smooth = 0.1f;

            // Smoothly move camera.position towards desiredPos
            camera.position.x += (desiredPos.x - camera.position.x) * smooth;
            camera.position.y += (desiredPos.y - camera.position.y) * smooth;
            camera.position.z += (desiredPos.z - camera.position.z) * smooth;

            // Always look at the character
            camera.target = transform.position;
        }
    }
    
    // Render character
    void Render(entt::registry& registry) {
        auto view = registry.view<CharacterModel, Transform>();
        
        for (auto entity : view) {
            auto& modelComp = view.get<CharacterModel>(entity);
            auto& transform = view.get<Transform>(entity);
            
            // Draw model with transform
            DrawModelEx(modelComp.model, 
                       transform.position,
                       {0.0f, 1.0f, 0.0f},  // Rotate around Y axis
                       transform.rotation.y,
                       transform.scale,
                       WHITE);
            
            // Debug: Draw position indicator
            // DrawSphere(transform.position, 0.2f, RED);
        }
    }
    
    // Cleanup
    void UnloadCharacter(entt::registry& registry, entt::entity character) {
        if (registry.valid(character) && registry.all_of<CharacterModel>(character)) {
            auto& modelComp = registry.get<CharacterModel>(character);
            
            // Unload animations
            if (modelComp.animations != nullptr) {
                UnloadModelAnimations(modelComp.animations, modelComp.animationCount);
            }
            
            // Unload model
            UnloadModel(modelComp.model);
            
            registry.destroy(character);
            std::cout << "Character unloaded." << std::endl;
        }
    }
};

} // namespace ECS

#endif // CHARACTER_SYSTEM_H
