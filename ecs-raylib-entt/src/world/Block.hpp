#ifndef BLOCK_H
#define BLOCK_H

#include <raylib.h>
#include <string>

namespace World {

// Enum representing the different block types
enum class BlockType {
    Soil,
    Stone,
    Gold,
    Silver
};

// Structure representing a single block in the world
struct Block {
    BlockType type;
    Color color;
    
    // Constructor
    Block(BlockType t = BlockType::Soil) : type(t) {
        color = GetColorFromType(type);
    }
    
    // Get color based on block type
    static Color GetColorFromType(BlockType type) {
        switch (type) {
            case BlockType::Soil:
                return BROWN;
            case BlockType::Stone:
                return GRAY;
            case BlockType::Gold:
                return GOLD;
            case BlockType::Silver:
                return Color{192, 192, 192, 255}; // Silver color
            default:
                return WHITE;
        }
    }
    
    // Get string name of block type
    static std::string GetTypeName(BlockType type) {
        switch (type) {
            case BlockType::Soil:
                return "Soil";
            case BlockType::Stone:
                return "Stone";
            case BlockType::Gold:
                return "Gold";
            case BlockType::Silver:
                return "Silver";
            default:
                return "Unknown";
        }
    }
    
    // Get the name of this block
    std::string GetName() const {
        return GetTypeName(type);
    }
};

} // namespace World

#endif // BLOCK_H
