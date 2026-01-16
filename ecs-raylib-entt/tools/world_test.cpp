#include "../src/world/World.hpp"
#include <iostream>
#include <cassert>

// Test that surface layers contain only Soil and Stone
void TestSurfaceLayers() {
    std::cout << "Testing Surface Layers..." << std::endl;
    
    World::World world;
    world.Generate();
    
    for (int y = 0; y < 3; ++y) {
        for (int z = 0; z < world.GetDepth(); ++z) {
            for (int x = 0; x < world.GetWidth(); ++x) {
                const World::Block& block = world.GetBlock(x, y, z);
                assert(block.type == World::BlockType::Soil || 
                       block.type == World::BlockType::Stone);
            }
        }
    }
    
    std::cout << "✓ Surface layers contain only Soil and Stone" << std::endl;
}

// Test that underground layers contain no Soil
void TestUndergroundLayers() {
    std::cout << "Testing Underground Layers..." << std::endl;
    
    World::World world;
    world.Generate();
    
    for (int y = 3; y < world.GetHeight(); ++y) {
        for (int z = 0; z < world.GetDepth(); ++z) {
            for (int x = 0; x < world.GetWidth(); ++x) {
                const World::Block& block = world.GetBlock(x, y, z);
                assert(block.type != World::BlockType::Soil);
            }
        }
    }
    
    std::cout << "✓ Underground layers contain no Soil" << std::endl;
}

// Test surface layer distribution (should be approximately 80/20)
void TestSurfaceDistribution() {
    std::cout << "Testing Surface Distribution..." << std::endl;
    
    World::World world;
    world.Generate();
    
    int soilCount = 0;
    int stoneCount = 0;
    int surfaceBlocks = world.GetWidth() * world.GetDepth() * 3;
    
    for (int y = 0; y < 3; ++y) {
        for (int z = 0; z < world.GetDepth(); ++z) {
            for (int x = 0; x < world.GetWidth(); ++x) {
                const World::Block& block = world.GetBlock(x, y, z);
                if (block.type == World::BlockType::Soil) {
                    soilCount++;
                } else if (block.type == World::BlockType::Stone) {
                    stoneCount++;
                }
            }
        }
    }
    
    double soilPercentage = (soilCount * 100.0) / surfaceBlocks;
    double stonePercentage = (stoneCount * 100.0) / surfaceBlocks;
    
    std::cout << "  Soil: " << soilPercentage << "% (expected ~80%)" << std::endl;
    std::cout << "  Stone: " << stonePercentage << "% (expected ~20%)" << std::endl;
    
    // Allow 10% margin of error due to randomness
    assert(soilPercentage > 70.0 && soilPercentage < 90.0);
    assert(stonePercentage > 10.0 && stonePercentage < 30.0);
    
    std::cout << "✓ Surface distribution is within expected range" << std::endl;
}

// Test that all positions are valid
void TestPositionValidity() {
    std::cout << "Testing Position Validity..." << std::endl;
    
    World::World world;
    world.Generate();
    
    // Valid positions
    assert(world.IsValidPosition(0, 0, 0));
    assert(world.IsValidPosition(35, 35, 35));
    assert(world.IsValidPosition(18, 18, 18));
    
    // Invalid positions
    assert(!world.IsValidPosition(-1, 0, 0));
    assert(!world.IsValidPosition(0, -1, 0));
    assert(!world.IsValidPosition(0, 0, -1));
    assert(!world.IsValidPosition(36, 0, 0));
    assert(!world.IsValidPosition(0, 36, 0));
    assert(!world.IsValidPosition(0, 0, 36));
    assert(!world.IsValidPosition(100, 100, 100));
    
    std::cout << "✓ Position validity checks working correctly" << std::endl;
}

// Test layer identification
void TestLayerIdentification() {
    std::cout << "Testing Layer Identification..." << std::endl;
    
    World::World world;
    
    // Surface layers
    assert(world.IsSurfaceLayer(0));
    assert(world.IsSurfaceLayer(1));
    assert(world.IsSurfaceLayer(2));
    
    // Underground layers
    assert(!world.IsSurfaceLayer(3));
    assert(!world.IsSurfaceLayer(10));
    assert(!world.IsSurfaceLayer(35));
    
    std::cout << "✓ Layer identification working correctly" << std::endl;
}

// Test block colors match their types
void TestBlockColors() {
    std::cout << "Testing Block Colors..." << std::endl;
    
    World::Block soil(World::BlockType::Soil);
    World::Block stone(World::BlockType::Stone);
    World::Block gold(World::BlockType::Gold);
    World::Block silver(World::BlockType::Silver);
    
    assert(soil.color.r == BROWN.r && soil.color.g == BROWN.g && soil.color.b == BROWN.b);
    assert(stone.color.r == GRAY.r && stone.color.g == GRAY.g && stone.color.b == GRAY.b);
    assert(gold.color.r == GOLD.r && gold.color.g == GOLD.g && gold.color.b == GOLD.b);
    
    std::cout << "✓ Block colors match their types" << std::endl;
}

// Test world dimensions
void TestWorldDimensions() {
    std::cout << "Testing World Dimensions..." << std::endl;
    
    World::World world;
    
    assert(world.GetWidth() == 36);
    assert(world.GetHeight() == 36);
    assert(world.GetDepth() == 36);
    
    std::cout << "✓ World dimensions are correct (36x36x36)" << std::endl;
}

// Run multiple generations to test consistency
void TestMultipleGenerations() {
    std::cout << "Testing Multiple Generations..." << std::endl;
    
    World::World world;
    
    for (int i = 0; i < 5; ++i) {
        world.Generate();
        
        // Verify rules still hold
        for (int y = 0; y < 3; ++y) {
            for (int z = 0; z < world.GetDepth(); ++z) {
                for (int x = 0; x < world.GetWidth(); ++x) {
                    const World::Block& block = world.GetBlock(x, y, z);
                    assert(block.type == World::BlockType::Soil || 
                           block.type == World::BlockType::Stone);
                }
            }
        }
        
        for (int y = 3; y < world.GetHeight(); ++y) {
            for (int z = 0; z < world.GetDepth(); ++z) {
                for (int x = 0; x < world.GetWidth(); ++x) {
                    const World::Block& block = world.GetBlock(x, y, z);
                    assert(block.type != World::BlockType::Soil);
                }
            }
        }
    }
    
    std::cout << "✓ Multiple generations maintain rule consistency" << std::endl;
}

int main() {
    std::cout << "\n========================================" << std::endl;
    std::cout << "      WORLD STRUCTURE TEST SUITE" << std::endl;
    std::cout << "========================================\n" << std::endl;
    
    try {
        TestWorldDimensions();
        std::cout << std::endl;
        
        TestLayerIdentification();
        std::cout << std::endl;
        
        TestPositionValidity();
        std::cout << std::endl;
        
        TestBlockColors();
        std::cout << std::endl;
        
        TestSurfaceLayers();
        std::cout << std::endl;
        
        TestUndergroundLayers();
        std::cout << std::endl;
        
        TestSurfaceDistribution();
        std::cout << std::endl;
        
        TestMultipleGenerations();
        std::cout << std::endl;
        
        std::cout << "========================================" << std::endl;
        std::cout << "  ✓ ALL TESTS PASSED SUCCESSFULLY!" << std::endl;
        std::cout << "========================================" << std::endl;
        
        // Show a sample world
        std::cout << "\nGenerating a sample world for inspection:" << std::endl;
        World::World world;
        world.Generate();
        world.PrintStatistics();
        
    } catch (const std::exception& e) {
        std::cerr << "\n✗ TEST FAILED: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
