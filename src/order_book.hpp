#pragma once
#include "order.hpp"

#include <map>
#include <deque>
#include <vector>


class OrderBook{
    std::map<Price, std::deque<Order>> Bids;
    std::map<Price, std::deque<Order>> Asks;

    public:
        std::size_t bid_count(Price price) const;
        std::size_t ask_count(Price price) const;
        OrderId first_bid_id(Price price) const;
        OrderId first_ask_id(Price price) const;
        Price best_bid_price() const;
        Price best_ask_price() const;
        std::vector<Fill> add_order(const Order& order);
        Quantity first_bid_remaining(Price price) const;
        Quantity first_ask_remaining(Price price) const;

    private:
        void match_buy(Order& incoming_order, std::vector<Fill>& fills);
        void match_sell(Order& incoming_order, std::vector<Fill>& fills);

};