#include "World.hpp"
#include <iostream>
#include <iomanip>
#include <map>

namespace World {

World::World() : rng(std::random_device{}()) {
    InitializeGrid();
}

void World::InitializeGrid() {
    grid.clear();
    grid.resize(WORLD_HEIGHT);
    for (int y = 0; y < WORLD_HEIGHT; ++y) {
        grid[y].resize(WORLD_DEPTH);
        for (int z = 0; z < WORLD_DEPTH; ++z) {
            grid[y][z].resize(WORLD_WIDTH);
        }
    }
}

void World::Generate() {
    // Generate completely solid world
    // Blocks on ANY boundary surface (6 faces): 80% Soil, 20% Stone
    // Interior blocks: 70% Stone, 20% Gold, 10% Silver
    
    for (int y = 0; y < WORLD_HEIGHT; ++y) {
        for (int z = 0; z < WORLD_DEPTH; ++z) {
            for (int x = 0; x < WORLD_WIDTH; ++x) {
                // Check if this block is on any boundary (exposed surface)
                if (IsExposedSurface(x, y, z)) {
                    // Surface block: 80% Soil, 20% Stone
                    grid[y][z][x] = GenerateSurfaceBlock();
                } else {
                    // Interior block: Stone, Gold, or Silver
                    grid[y][z][x] = GenerateUndergroundBlock();
                }
            }
        }
    }
}

Block World::GenerateSurfaceBlock() {
    // Generate random number between 0 and 99
    std::uniform_int_distribution<int> dist(0, 99);
    int roll = dist(rng);
    
    // 80% chance for Soil (0-79), 20% chance for Stone (80-99)
    if (roll < 80) {
        return Block(BlockType::Soil);
    } else {
        return Block(BlockType::Stone);
    }
}

Block World::GenerateUndergroundBlock() {
    // Underground distribution:
    // 70% Stone, 20% Gold, 10% Silver (implementation-defined)
    // You can adjust these percentages as needed
    std::uniform_int_distribution<int> dist(0, 99);
    int roll = dist(rng);
    
    if (roll < 70) {
        return Block(BlockType::Stone);
    } else if (roll < 90) {
        return Block(BlockType::Gold);
    } else {
        return Block(BlockType::Silver);
    }
}

const Block& World::GetBlock(int x, int y, int z) const {
    if (!IsValidPosition(x, y, z)) {
        static Block emptyBlock(BlockType::Stone);
        return emptyBlock;
    }
    return grid[y][z][x];
}

Block& World::GetBlockMutable(int x, int y, int z) {
    static Block errorBlock(BlockType::Stone);
    if (!IsValidPosition(x, y, z)) {
        return errorBlock;
    }
    return grid[y][z][x];
}

void World::SetBlock(int x, int y, int z, const Block& block) {
    if (IsValidPosition(x, y, z)) {
        grid[y][z][x] = block;
    }
}

bool World::IsValidPosition(int x, int y, int z) const {
    return x >= 0 && x < WORLD_WIDTH && 
           y >= 0 && y < WORLD_HEIGHT && 
           z >= 0 && z < WORLD_DEPTH;
}

bool World::IsSurfaceLayer(int y) const {
    return y < SURFACE_LAYER_COUNT;
}

bool World::IsExposedSurface(int x, int y, int z) const {
    // A block is part of the surface shell if it's within SURFACE_LAYER_COUNT distance
    // from ANY of the 6 boundaries
    // This creates a thick shell around the entire cube
    
    // Top layers (y = 0 to SURFACE_LAYER_COUNT-1)
    if (y < SURFACE_LAYER_COUNT) return true;
    
    // Bottom layers (y = WORLD_HEIGHT-SURFACE_LAYER_COUNT to WORLD_HEIGHT-1)
    if (y >= WORLD_HEIGHT - SURFACE_LAYER_COUNT) return true;
    
    // Left layers (x = 0 to SURFACE_LAYER_COUNT-1)
    if (x < SURFACE_LAYER_COUNT) return true;
    
    // Right layers (x = WORLD_WIDTH-SURFACE_LAYER_COUNT to WORLD_WIDTH-1)
    if (x >= WORLD_WIDTH - SURFACE_LAYER_COUNT) return true;
    
    // Front layers (z = 0 to SURFACE_LAYER_COUNT-1)
    if (z < SURFACE_LAYER_COUNT) return true;
    
    // Back layers (z = WORLD_DEPTH-SURFACE_LAYER_COUNT to WORLD_DEPTH-1)
    if (z >= WORLD_DEPTH - SURFACE_LAYER_COUNT) return true;
    
    // Not in any surface layer = interior
    return false;
}

bool World::IsAir(int x, int y, int z) const {
    // No air blocks in solid world
    return false;
}

void World::Clear() {
    InitializeGrid();
}

int World::GetSurfaceLevel(int x, int z) const {
    if (x < 0 || x >= WORLD_WIDTH || z < 0 || z >= WORLD_DEPTH) {
        return 0; // fallback
    }

    // Find the highest solid block (from top down) at this (x,z)
    for (int y = WORLD_HEIGHT - 1; y >= 0; --y) {
        const Block& b = grid[y][z][x];
        if (true) {   // or just "if (true)" since you have no air yet
            return y;
        }
    }
    // If nothing found, just return 0
    return 0;
}

void World::PrintStatistics() const {
    // Count each block type
    std::map<BlockType, int> counts;
    std::map<BlockType, int> surfaceCounts;
    std::map<BlockType, int> interiorCounts;
    int exposedSurfaceCount = 0;
    int interiorBlockCount = 0;
    
    for (int y = 0; y < WORLD_HEIGHT; ++y) {
        for (int z = 0; z < WORLD_DEPTH; ++z) {
            for (int x = 0; x < WORLD_WIDTH; ++x) {
                BlockType type = grid[y][z][x].type;
                counts[type]++;
                
                // Check if this is an exposed surface (on any boundary)
                if (IsExposedSurface(x, y, z)) {
                    exposedSurfaceCount++;
                    surfaceCounts[type]++;
                } else {
                    interiorBlockCount++;
                    interiorCounts[type]++;
                }
            }
        }
    }
    
    int totalBlocks = WORLD_WIDTH * WORLD_HEIGHT * WORLD_DEPTH;
    
    std::cout << "\n===== WORLD STATISTICS (3D - SOLID WORLD WITH 6-FACE SURFACES) =====" << std::endl;
    std::cout << "World Size: " << WORLD_WIDTH << "x" << WORLD_HEIGHT << "x" << WORLD_DEPTH
              << " (" << totalBlocks << " blocks)" << std::endl;
    std::cout << "All Blocks Solid: " << totalBlocks << " (100%)" << std::endl;
    std::cout << "Exposed Surface Blocks: " << exposedSurfaceCount 
              << " (" << std::fixed << std::setprecision(1) 
              << (exposedSurfaceCount * 100.0 / totalBlocks) << "% - on boundaries)" << std::endl;
    std::cout << "Interior Blocks: " << interiorBlockCount 
              << " (" << std::fixed << std::setprecision(1) 
              << (interiorBlockCount * 100.0 / totalBlocks) << "% - not on boundaries)" << std::endl;
    
    std::cout << "\n----- Overall Distribution -----" << std::endl;
    for (const auto& [type, count] : counts) {
        double percentage = (count * 100.0) / totalBlocks;
        std::cout << std::setw(10) << Block::GetTypeName(type) << ": " 
                  << std::setw(5) << count << " (" 
                  << std::fixed << std::setprecision(1) << percentage << "%)" << std::endl;
    }
    
    std::cout << "\n----- Exposed Surface Distribution (80/20 Soil/Stone) -----" << std::endl;
    for (const auto& [type, count] : surfaceCounts) {
        double percentage = (count * 100.0) / exposedSurfaceCount;
        std::cout << std::setw(10) << Block::GetTypeName(type) << ": " 
                  << std::setw(5) << count << " (" 
                  << std::fixed << std::setprecision(1) << percentage << "%)" << std::endl;
    }
    
    std::cout << "\n----- Interior Block Distribution (70/20/10 Stone/Gold/Silver) -----" << std::endl;
    for (const auto& [type, count] : interiorCounts) {
        double percentage = (count * 100.0) / interiorBlockCount;
        std::cout << std::setw(10) << Block::GetTypeName(type) << ": " 
                  << std::setw(5) << count << " (" 
                  << std::fixed << std::setprecision(1) << percentage << "%)" << std::endl;
    }
    
    std::cout << "\n============================" << std::endl;
}

} // namespace World
