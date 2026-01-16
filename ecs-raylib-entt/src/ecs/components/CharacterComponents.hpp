#ifndef CHARACTER_COMPONENTS_H
#define CHARACTER_COMPONENTS_H

#include <raylib.h>
#include <vector>

namespace ECS {

// Character model component
struct CharacterModel {
    Model model;
    ModelAnimation* animations;
    int animationCount;
    int currentAnimation;
    int currentFrame;
    
    CharacterModel() : model{0}, animations(nullptr), animationCount(0), 
                       currentAnimation(0), currentFrame(0) {}
};

// Animation state component
struct AnimationState {
    enum State {
        STANDING = 9,  // Animation 9 is standing
        WALKING = 12   // Animation 12 is walking
    };
    
    State currentState;
    float animationTime;
    
    AnimationState() : currentState(STANDING), animationTime(0.0f) {}
};

// Character transform component (for smooth movement, different from grid Position)
struct Transform {
    Vector3 position;
    Vector3 rotation;
    Vector3 scale;
    
    Transform() : position{0.0f, 0.0f, 0.0f}, 
                  rotation{0.0f, 0.0f, 0.0f},
                  scale{1.0f, 1.0f, 1.0f} {}
    Transform(Vector3 pos) : position(pos), 
                            rotation{0.0f, 0.0f, 0.0f},
                            scale{1.0f, 1.0f, 1.0f} {}
};

// Character movement component
struct Movement {
    Vector3 velocity;
    float speed;
    float rotationSpeed;
    bool isMoving;
    
    Movement() : velocity{0.0f, 0.0f, 0.0f}, 
                 speed(3.0f), 
                 rotationSpeed(180.0f),
                 isMoving(false) {}
};

// Character controller tag
struct CharacterTag {};

// Camera follow component (for third-person camera)
struct CameraFollow {
    float distance;
    float height;
    float angle;
    
    CameraFollow() : distance(8.0f), height(4.0f), angle(0.0f) {}  // Closer camera for smaller character
};

} // namespace ECS

#endif // CHARACTER_COMPONENTS_H
