#pragma once

#include "pocketscene/rendering/rid.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <span>

namespace pocketscene::rendering {

enum class DebugMode : std::uint8_t {
    Lit,
    Unlit,
    Wireframe,
    Normals,
    Uv,
    Depth,
};

struct TextureDesc {
    std::uint32_t width = 0;
    std::uint32_t height = 0;
    bool srgb = false;
};

struct MeshUpload {
    std::span<const std::byte> vertices;
    std::span<const std::byte> indices;
    std::uint32_t vertex_stride = 0;
    std::uint32_t index_count = 0;
};

using Transform3d = std::array<float, 16>;

class RenderingServer {
  public:
    virtual ~RenderingServer() = default;

    [[nodiscard]] virtual Rid texture_create(const TextureDesc& desc,
                                             std::span<const std::byte> pixels) = 0;
    [[nodiscard]] virtual Rid mesh_create(const MeshUpload& upload) = 0;
    [[nodiscard]] virtual Rid scenario_create() = 0;
    [[nodiscard]] virtual Rid viewport_create() = 0;
    [[nodiscard]] virtual Rid instance_create() = 0;

    virtual void instance_set_base(Rid instance, Rid mesh) = 0;
    virtual void instance_set_scenario(Rid instance, Rid scenario) = 0;
    virtual void instance_set_transform(Rid instance, const Transform3d& transform) = 0;
    virtual void viewport_set_scenario(Rid viewport, Rid scenario) = 0;
    virtual void viewport_set_debug_mode(Rid viewport, DebugMode mode) = 0;
    virtual void free_rid(Rid rid) = 0;
};

} // namespace pocketscene::rendering
