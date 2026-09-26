#include "pocketscene/rendering/rid.hpp"

#include <cassert>

int main() {
    using pocketscene::rendering::resource_kind_name;
    using pocketscene::rendering::ResourceKind;
    using pocketscene::rendering::Rid;

    constexpr auto rid = Rid::make(42U, 7U, ResourceKind::Mesh);
    static_assert(rid.valid());
    static_assert(rid.index() == 42U);
    static_assert(rid.generation() == 7U);
    static_assert(rid.kind() == ResourceKind::Mesh);

    constexpr Rid invalid;
    static_assert(!invalid.valid());
    assert(resource_kind_name(rid.kind()) == "mesh");
}
