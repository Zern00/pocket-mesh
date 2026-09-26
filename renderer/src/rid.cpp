#include "pocketscene/rendering/rid.hpp"

namespace pocketscene::rendering {

std::string_view resource_kind_name(const ResourceKind kind) noexcept {
    switch (kind) {
    case ResourceKind::Invalid:
        return "invalid";
    case ResourceKind::Texture:
        return "texture";
    case ResourceKind::Material:
        return "material";
    case ResourceKind::Mesh:
        return "mesh";
    case ResourceKind::Instance:
        return "instance";
    case ResourceKind::Scenario:
        return "scenario";
    case ResourceKind::Viewport:
        return "viewport";
    }

    return "unknown";
}

} // namespace pocketscene::rendering
