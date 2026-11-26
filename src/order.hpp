#pragma once
#include <cstdint>

using OrderId = std::uint64_t;
using Price = std::int64_t;
using Quantity = std::uint64_t;

enum class Side { Buy, Sell };

struct Order {
    OrderId id;
    Side side;
    Price price;
    Quantity remaining;
};

struct Fill {
    OrderId incoming_id;
    OrderId resting_id;
    Price price;
    Quantity quantity;
};