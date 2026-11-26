#include "order.hpp"
#include "order_book.hpp"

#include <iostream>
#include <map>
#include <random>
#include <deque>

int main() {
    OrderBook book;

    book.add_order(Order{1, Side::Buy, 100, 10});
    book.add_order(Order{2, Side::Buy, 100, 5});
    book.add_order(Order{3, Side::Buy, 101, 7});

    book.add_order(Order{4, Side::Sell, 103, 8});
    book.add_order(Order{5, Side::Sell, 103, 3});
    book.add_order(Order{6, Side::Sell, 102, 6});

    std::cout << "Bids at price 100: " << book.bid_count(100) << std::endl; // Should output 2
    std::cout << "First bid ID at price 100: " << book.first_bid_id(100) << std::endl; // Should output 1
    std::cout << "Best bid price: " << book.best_bid_price() << std::endl; // Should output 101

    std::cout << "Asks at price 100: " << book.ask_count(100) << std::endl; // Should output 2
    std::cout << "First ask ID at price 102: " << book.first_ask_id(102) << std::endl; // Should output 6
    std::cout << "Best ask price: " << book.best_ask_price() << std::endl; // Should output 102

    OrderBook levels_book;

    levels_book.add_order(Order{20, Side::Sell, 101, 3});
    levels_book.add_order(Order{21, Side::Sell, 102, 5});
    levels_book.add_order(Order{22, Side::Buy, 102, 6});

    std::cout << "Asks at 101: " << levels_book.ask_count(101) << '\n';
    std::cout << "Asks at 102: " << levels_book.ask_count(102) << '\n';
    std::cout << "First ask ID at 102: "
            << levels_book.first_ask_id(102) << '\n';
    std::cout << "First ask remaining at 102: "
            << levels_book.first_ask_remaining(102) << '\n';
    std::cout << "Bids at 102: " << levels_book.bid_count(102) << '\n';

    OrderBook fill_book;

    fill_book.add_order(Order{20, Side::Sell, 101, 3});
    fill_book.add_order(Order{21, Side::Sell, 102, 5});

    std::vector<Fill> fills =
        fill_book.add_order(Order{22, Side::Buy, 102, 6});

    std::cout << "Fill count: " << fills.size() << '\n';

    for (const Fill& fill : fills) {
        std::cout << "Incoming " << fill.incoming_id
                << ", resting " << fill.resting_id
                << ", price " << fill.price
                << ", quantity " << fill.quantity << '\n';
    }

    OrderBook sell_fill_book;

    sell_fill_book.add_order(Order{30, Side::Buy, 101, 5});
    sell_fill_book.add_order(Order{31, Side::Buy, 102, 3});

    std::vector<Fill> sell_fills =
        sell_fill_book.add_order(Order{32, Side::Sell, 101, 6});

    std::cout << "Sell fill count: " << sell_fills.size() << '\n';

    for (const Fill& fill : sell_fills) {
        std::cout << "Incoming " << fill.incoming_id
                << ", resting " << fill.resting_id
                << ", price " << fill.price
                << ", quantity " << fill.quantity << '\n';
    }

    std::cout << "First bid remaining at 101: "
            << sell_fill_book.first_bid_remaining(101) << '\n';
}

