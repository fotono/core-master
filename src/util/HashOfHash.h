#pragma once
#include <xdr/fotono-types.h>

namespace std
{
template <> struct hash<fotono::uint256>
{
    size_t operator()(fotono::uint256 const& x) const noexcept;
};
}
