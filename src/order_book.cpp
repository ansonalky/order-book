#include "order_book.hpp"
#include "order.hpp"
#include <vector>

std::vector<Fill> OrderBook::add_order(const Order& order) {
    Order incoming = order;
    std::vector<Fill> fills;
    if (incoming.side == Side::Buy) {
        match_buy(incoming, fills);
        if (incoming.remaining > 0) {
            Bids[incoming.price].push_back(incoming);
        }
    } else {
        match_sell(incoming, fills);
        if (incoming.remaining > 0) {
            Asks[incoming.price].push_back(incoming);
        }
    }

    return fills;
}

std::size_t OrderBook::bid_count(Price price) const {
    auto it = Bids.find(price);
    if (it != Bids.end()) {
        return it->second.size();
    }
    return 0;
}

std::size_t OrderBook::ask_count(Price price) const {
    auto it = Asks.find(price);
    if (it != Asks.end()) {
        return it->second.size();
    }
    return 0;
}

OrderId OrderBook::first_bid_id(Price price) const {
    auto it = Bids.find(price);
    if (it != Bids.end() && !it->second.empty()) {
        return it->second.front().id;
    }
    return 0;
}

OrderId OrderBook::first_ask_id(Price price) const {
    auto it = Asks.find(price);
    if (it != Asks.end() && !it->second.empty()) {
        return it->second.front().id;
    }
    return 0;
}

Price OrderBook::best_bid_price() const {
    if (!Bids.empty()) {
        return Bids.rbegin()->first; // The last element in the map has the highest price
    }
    return 0; // Return 0 if there are no bids
}

Price OrderBook::best_ask_price() const {
    if (!Asks.empty()) {
        return Asks.begin()->first; // The first element in the map has the lowest price
    }
    return 0; // Return 0 if there are no asks
}

void OrderBook::match_buy(Order& incoming_order, std::vector<Fill>& fills) {
    while (incoming_order.remaining > 0 && !Asks.empty()) {
        auto level = Asks.begin(); 
        
        if (level -> first > incoming_order.price) {
            break;
        }

        auto& ask_queue = level->second;
        Order& ask_order = ask_queue.front();
        Quantity trade_quantity = std::min(incoming_order.remaining, ask_order.remaining);
        incoming_order.remaining -= trade_quantity;
        ask_order.remaining -= trade_quantity;

        fills.push_back(Fill{
            incoming_order.id, 
            ask_order.id, 
            ask_order.price, 
            trade_quantity
        });
        
        if (ask_order.remaining == 0) {
                ask_queue.pop_front(); // Remove the ask order if fully filled
        }

        if (ask_queue.empty()) {
            Asks.erase(level);
        }

    }
}

void OrderBook::match_sell(Order& incoming_order, std::vector<Fill>& fills) {
    while (incoming_order.remaining > 0 && !Bids.empty()) {
        auto level = Bids.end(); // Get the last element (highest price)
        --level; // Move to the actual highest bid

        if (level->first < incoming_order.price) {
            break;
        }

        auto& bid_queue = level->second;
        Order& bid_order = bid_queue.front();
        Quantity trade_quantity = std::min(incoming_order.remaining, bid_order.remaining);
        incoming_order.remaining -= trade_quantity;
        bid_order.remaining -= trade_quantity;

        fills.push_back(Fill{
            incoming_order.id, 
            bid_order.id, 
            bid_order.price, 
            trade_quantity
        });
        
        if (bid_order.remaining == 0) {
                bid_queue.pop_front(); // Remove the bid order if fully filled
        }

        if (bid_queue.empty()) {
            Bids.erase(level);
        }

    }
}

Quantity OrderBook::first_bid_remaining(Price price) const {
    auto it = Bids.find(price);

    if (it == Bids.end() || it->second.empty()) {
        return 0;
    }

    return it->second.front().remaining;
}

Quantity OrderBook::first_ask_remaining(Price price) const {
    auto it = Asks.find(price);

    if (it == Asks.end() || it->second.empty()) {
        return 0;
    }

    return it->second.front().remaining;
}

