#ifndef WORLD_H
#define WORLD_H

#include "Block.hpp"
#include <vector>
#include <random>

namespace World {

// World dimensions (3D: Width x Height x Depth)
constexpr int WORLD_WIDTH = 36;   // X-axis
constexpr int WORLD_HEIGHT = 36;  // Y-axis (vertical)
constexpr int WORLD_DEPTH = 36;   // Z-axis
constexpr int SURFACE_LAYER_COUNT = 4;

// Class representing the entire world grid
class World {
public:
    // Constructor
    World();
    
    // Destructor
    ~World() = default;
    
    // Generate the world with the specified rules
    void Generate();
    
    // Get a block at a specific position (x, y, z)
    const Block& GetBlock(int x, int y, int z) const;
    
    // Get a mutable reference to a block
    Block& GetBlockMutable(int x, int y, int z);
    
    // Set a block at a specific position
    void SetBlock(int x, int y, int z, const Block& block);
    
    // Check if coordinates are valid
    bool IsValidPosition(int x, int y, int z) const;
    
    // Check if a layer is a surface layer
    bool IsSurfaceLayer(int y) const;
    
    // Check if a block is an exposed surface
    bool IsExposedSurface(int x, int y, int z) const;

	int GetSurfaceLevel(int x, int z) const;

    // Check if a block is air (always false in solid world)
    bool IsAir(int x, int y, int z) const;
    
    // Get world dimensions
    int GetWidth() const { return WORLD_WIDTH; }
    int GetHeight() const { return WORLD_HEIGHT; }
    int GetDepth() const { return WORLD_DEPTH; }
    
    // Print world statistics (for debugging)
    void PrintStatistics() const;
    
    // Clear the world
    void Clear();

private:
    // The grid of blocks (organized as [y][z][x] for layer-major access)
    // Y = height (vertical layers), Z = depth, X = width
    std::vector<std::vector<std::vector<Block>>> grid;
    
    // Random number generator
    std::mt19937 rng;
    
    // Generate a surface layer block (80% Soil, 20% Stone)
    Block GenerateSurfaceBlock();
    
    // Generate an underground layer block (Stone, Gold, or Silver)
    Block GenerateUndergroundBlock();
    
    // Initialize the grid with default blocks
    void InitializeGrid();
};

} // namespace World

#endif // WORLD_H
