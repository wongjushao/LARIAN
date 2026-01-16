#ifndef COMPONENTS_H
#define COMPONENTS_H

#include <raylib.h>
#include "../../world/Block.hpp"

namespace ECS {

// Position component - where the block is in 3D space
struct Position {
    int x, y, z;
    
    Position() : x(0), y(0), z(0) {}
    Position(int _x, int _y, int _z) : x(_x), y(_y), z(_z) {}
};

// Block data component - what type of block it is
struct BlockData {
    World::BlockType type;
    Color color;
    
    BlockData() : type(World::BlockType::Stone), color(WHITE) {}
    BlockData(World::BlockType t, Color c) : type(t), color(c) {}
};

// Renderable component - should this entity be rendered?
struct Renderable {
    bool visible;
    bool wireframe;
    
    Renderable() : visible(true), wireframe(false) {}
    Renderable(bool v) : visible(v), wireframe(false) {}
};

// Surface component - is this block an exposed surface?
struct Surface {
    bool isExposed;
    
    Surface() : isExposed(false) {}
    Surface(bool exposed) : isExposed(exposed) {}
};

// Mineable component - can this block be mined?
struct Mineable {
    bool canMine;
    int value;        // Points/resources when mined
    float hardness;   // Time to mine
    
    Mineable() : canMine(true), value(0), hardness(1.0f) {}
    Mineable(bool mine, int val, float hard = 1.0f) 
        : canMine(mine), value(val), hardness(hard) {}
};

// Tag components for different block types (no AirTag needed for solid world)
struct SoilTag {};
struct StoneTag {};
struct GoldTag {};
struct SilverTag {};

struct Character {
	float height;
	float radius;
	Vector3 velocity;
	bool onGround;

	Character() : height(1.8f), radius(0.3f), velocity({0, 0, 0}), onGround(false) {}
    Character(float h, float r) : height(h), radius(r), velocity({0, 0, 0}), onGround(false) {}
};

struct CameraComponent {
    float pitch;  // Up/down rotation
    float yaw;    // Left/right rotation
    
    CameraComponent() : pitch(0.0f), yaw(0.0f) {}
};

} // namespace ECS

#endif // COMPONENTS_H
