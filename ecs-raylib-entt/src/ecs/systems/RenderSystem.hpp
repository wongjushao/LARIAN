#ifndef RENDER_SYSTEM_H
#define RENDER_SYSTEM_H

#include <entt/entt.hpp>
#include <raylib.h>
#include "../components/Components.hpp"

namespace ECS {

class RenderSystem {
public:
    RenderSystem() : blockSize(1.0f) {}
    
    void SetBlockSize(float size) {
        blockSize = size;
    }
    
    // Render all visible blocks
    void Render(entt::registry& registry, int startY = 0, int endY = 36) {
        // Get all entities with Position, BlockData, and Renderable components
        auto view = registry.view<Position, BlockData, Renderable>();
        
        for (auto entity : view) {
            auto& pos = view.get<Position>(entity);
            auto& blockData = view.get<BlockData>(entity);
            auto& renderable = view.get<Renderable>(entity);
            
            // Skip if not visible or outside Y range
            if (!renderable.visible || pos.y < startY || pos.y >= endY) {
                continue;
            }
            
            // Calculate 3D position
            Vector3 position = {
                pos.x * blockSize,
                pos.y * blockSize,
                pos.z * blockSize
            };
            
            Vector3 size = { blockSize, blockSize, blockSize };
            
            // Draw the block
            if (renderable.wireframe) {
                DrawCubeWiresV(position, size, blockData.color);
            } else {
                DrawCubeV(position, size, blockData.color);
                DrawCubeWiresV(position, size, ColorBrightness(blockData.color, -0.3f));
            }
        }
    }
    
    // Render only blocks matching a specific layer
    void RenderLayer(entt::registry& registry, int layer) {
        auto view = registry.view<Position, BlockData, Renderable>();
        
        for (auto entity : view) {
            auto& pos = view.get<Position>(entity);
            
            if (pos.y != layer) continue;
            
            auto& blockData = view.get<BlockData>(entity);
            auto& renderable = view.get<Renderable>(entity);
            
            if (!renderable.visible) continue;
            
            Vector3 position = {
                pos.x * blockSize,
                pos.y * blockSize,
                pos.z * blockSize
            };
            
            Vector3 size = { blockSize, blockSize, blockSize };
            
            if (renderable.wireframe) {
                DrawCubeWiresV(position, size, blockData.color);
            } else {
                DrawCubeV(position, size, blockData.color);
                DrawCubeWiresV(position, size, ColorBrightness(blockData.color, -0.3f));
            }
        }
    }
    
    // Render only surface blocks (for surface-only view)
    void RenderSurfaces(entt::registry& registry) {
        auto view = registry.view<Position, BlockData, Renderable, Surface>();
        
        for (auto entity : view) {
            auto& pos = view.get<Position>(entity);
            auto& blockData = view.get<BlockData>(entity);
            auto& renderable = view.get<Renderable>(entity);
            auto& surface = view.get<Surface>(entity);
            
            // Only render exposed surfaces in top 3 layers
            if (!renderable.visible || !surface.isExposed || pos.y >= 3) {
                continue;
            }
            
            Vector3 position = {
                pos.x * blockSize,
                pos.y * blockSize,
                pos.z * blockSize
            };
            
            Vector3 size = { blockSize, blockSize, blockSize };
            
            if (renderable.wireframe) {
                DrawCubeWiresV(position, size, blockData.color);
            } else {
                DrawCubeV(position, size, blockData.color);
                DrawCubeWiresV(position, size, ColorBrightness(blockData.color, -0.3f));
            }
        }
    }
    
    // Render only underground blocks (below layer 3)
    void RenderUnderground(entt::registry& registry) {
        auto view = registry.view<Position, BlockData, Renderable>();
        
        for (auto entity : view) {
            auto& pos = view.get<Position>(entity);
            
            if (pos.y < 3) continue; // Skip surface layers
            
            auto& blockData = view.get<BlockData>(entity);
            auto& renderable = view.get<Renderable>(entity);
            
            if (!renderable.visible) continue;
            
            Vector3 position = {
                pos.x * blockSize,
                pos.y * blockSize,
                pos.z * blockSize
            };
            
            Vector3 size = { blockSize, blockSize, blockSize };
            
            if (renderable.wireframe) {
                DrawCubeWiresV(position, size, blockData.color);
            } else {
                DrawCubeV(position, size, blockData.color);
                DrawCubeWiresV(position, size, ColorBrightness(blockData.color, -0.3f));
            }
        }
    }
    
    // Toggle wireframe mode for all renderable entities
    void ToggleWireframe(entt::registry& registry, bool wireframe) {
        auto view = registry.view<Renderable>();
        for (auto entity : view) {
            auto& renderable = view.get<Renderable>(entity);
            renderable.wireframe = wireframe;
        }
    }
    
    // Get count of renderable entities
    size_t GetRenderableCount(entt::registry& registry) {
        return registry.view<Renderable>().size();
    }

private:
    float blockSize;
};

} // namespace ECS

#endif // RENDER_SYSTEM_H
