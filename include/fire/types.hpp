#pragma once

#include <fmt/color.h>

#include <ranges>

// concept to require that T is normal range to values of type V
template <class T, class V>
concept range_of = std::ranges::common_range<T> && std::same_as<V, std::ranges::range_value_t<T>>;

namespace fmt::style {
constexpr auto warn = fmt::emphasis::bold | fg(fmt::color::red);
}