#ifndef WORLD_SYSTEM_H
#define WORLD_SYSTEM_H

#include <entt/entt.hpp>
#include <iostream>
#include "../components/Components.hpp"
#include "../../world/World.hpp"

namespace ECS {

class WorldSystem {
public:
    WorldSystem() = default;
    
    // Populate the ECS registry with entities for each block in the world
    void PopulateFromWorld(entt::registry& registry, const World::World& world) {
        // Clear existing entities
        registry.clear();
        
        // Create an entity for each block
        for (int y = 0; y < world.GetHeight(); ++y) {
            for (int z = 0; z < world.GetDepth(); ++z) {
                for (int x = 0; x < world.GetWidth(); ++x) {
                    const World::Block& block = world.GetBlock(x, y, z);
                    
                    // Create entity
                    auto entity = registry.create();
                    
                    // Add position component
                    registry.emplace<Position>(entity, x, y, z);
                    
                    // Add block data component
                    registry.emplace<BlockData>(entity, block.type, block.color);
                    
                    // Add surface component
                    bool isExposed = world.IsExposedSurface(x, y, z);
                    registry.emplace<Surface>(entity, isExposed);
                    
                    // All blocks are renderable now (no air)
                    registry.emplace<Renderable>(entity, true);
                    
                    // Add mineable component based on block type
                    switch (block.type) {
                        case World::BlockType::Soil:
                            registry.emplace<Mineable>(entity, true, 1, 0.5f);
                            registry.emplace<SoilTag>(entity);
                            break;
                        case World::BlockType::Stone:
                            registry.emplace<Mineable>(entity, true, 5, 1.5f);
                            registry.emplace<StoneTag>(entity);
                            break;
                        case World::BlockType::Gold:
                            registry.emplace<Mineable>(entity, true, 100, 2.0f);
                            registry.emplace<GoldTag>(entity);
                            break;
                        case World::BlockType::Silver:
                            registry.emplace<Mineable>(entity, true, 50, 1.8f);
                            registry.emplace<SilverTag>(entity);
                            break;
                    }
                }
            }
        }
    }
    
    // Get statistics from registry
    void PrintStatistics(entt::registry& registry) {
        size_t soilCount = 0, stoneCount = 0, goldCount = 0, silverCount = 0, renderableCount = 0;
        
        registry.view<SoilTag>().each([&soilCount](auto) { soilCount++; });
        registry.view<StoneTag>().each([&stoneCount](auto) { stoneCount++; });
        registry.view<GoldTag>().each([&goldCount](auto) { goldCount++; });
        registry.view<SilverTag>().each([&silverCount](auto) { silverCount++; });
        registry.view<Renderable>().each([&renderableCount](auto) { renderableCount++; });
        
        int totalExposed = 0;
        registry.view<Surface>().each([&totalExposed](auto& surface) {
            if (surface.isExposed) totalExposed++;
        });
        
        std::cout << "\n===== ECS STATISTICS (SOLID WORLD) =====" << std::endl;
        std::cout << "Total Entities: " << registry.storage<entt::entity>().size() << std::endl;
        std::cout << "All Renderable: " << renderableCount << " (100%)" << std::endl;
        std::cout << "Exposed Surfaces: " << totalExposed << std::endl;
        std::cout << "\n----- Block Types -----" << std::endl;
        std::cout << "  Soil: " << soilCount << std::endl;
        std::cout << "  Stone: " << stoneCount << std::endl;
        std::cout << "  Gold: " << goldCount << std::endl;
        std::cout << "  Silver: " << silverCount << std::endl;
        std::cout << "==========================" << std::endl;
    }
};

} // namespace ECS

#endif // WORLD_SYSTEM_H
