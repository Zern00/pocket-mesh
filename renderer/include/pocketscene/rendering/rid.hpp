#pragma once

#include <compare>
#include <cstdint>
#include <string_view>

namespace pocketscene::rendering {

enum class ResourceKind : std::uint16_t {
    Invalid = 0,
    Texture,
    Material,
    Mesh,
    Instance,
    Scenario,
    Viewport,
};

class Rid final {
  public:
    constexpr Rid() = default;

    [[nodiscard]] static constexpr Rid make(const std::uint32_t index,
                                            const std::uint16_t generation,
                                            const ResourceKind kind) noexcept {
        const auto raw_kind = static_cast<std::uint64_t>(kind);
        const auto raw_generation = static_cast<std::uint64_t>(generation);
        const auto raw_index = static_cast<std::uint64_t>(index);
        return Rid{(raw_kind << kind_shift) | (raw_generation << generation_shift) | raw_index};
    }

    [[nodiscard]] constexpr bool valid() const noexcept { return kind() != ResourceKind::Invalid; }

    [[nodiscard]] constexpr ResourceKind kind() const noexcept {
        return static_cast<ResourceKind>((value_ >> kind_shift) & field_mask);
    }

    [[nodiscard]] constexpr std::uint16_t generation() const noexcept {
        return static_cast<std::uint16_t>((value_ >> generation_shift) & field_mask);
    }

    [[nodiscard]] constexpr std::uint32_t index() const noexcept {
        return static_cast<std::uint32_t>(value_ & index_mask);
    }

    [[nodiscard]] constexpr std::uint64_t value() const noexcept { return value_; }

    auto operator<=>(const Rid&) const = default;

  private:
    explicit constexpr Rid(const std::uint64_t value) : value_(value) {}

    static constexpr std::uint64_t generation_shift = 32U;
    static constexpr std::uint64_t kind_shift = 48U;
    static constexpr std::uint64_t index_mask = 0xFFFF'FFFFULL;
    static constexpr std::uint64_t field_mask = 0xFFFFULL;

    std::uint64_t value_ = 0;
};

[[nodiscard]] std::string_view resource_kind_name(ResourceKind kind) noexcept;

} // namespace pocketscene::rendering
