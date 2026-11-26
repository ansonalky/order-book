#include "order_book.hpp"

#include <cassert>
#include <iostream>
#include <vector>

void check_fill(const Fill& fill, OrderId incoming, OrderId resting,
                Price price, Quantity quantity) {
    assert(fill.incoming_id == incoming);
    assert(fill.resting_id == resting);
    assert(fill.price == price);
    assert(fill.quantity == quantity);
}

int main() {
    {
        OrderBook book;
        assert(book.add_order(Order{1, Side::Buy, 100, 5}).empty());
        assert(book.add_order(Order{2, Side::Sell, 102, 7}).empty());
        assert(book.bid_count(100) == 1);
        assert(book.ask_count(102) == 1);
        assert(book.best_bid_price() == 100);
        assert(book.best_ask_price() == 102);
    }

    {
        OrderBook book;
        book.add_order(Order{3, Side::Sell, 100, 5});
        const auto fills = book.add_order(Order{4, Side::Buy, 100, 5});
        assert(fills.size() == 1);
        check_fill(fills[0], 4, 3, 100, 5);
        assert(book.ask_count(100) == 0);
        assert(book.bid_count(100) == 0);
        assert(book.best_ask_price() == 0);
        assert(book.best_bid_price() == 0);
    }

    {
        OrderBook book;
        book.add_order(Order{5, Side::Sell, 101, 3});
        book.add_order(Order{6, Side::Sell, 101, 5});
        const auto fills = book.add_order(Order{7, Side::Buy, 102, 6});
        assert(fills.size() == 2);
        check_fill(fills[0], 7, 5, 101, 3);
        check_fill(fills[1], 7, 6, 101, 3);
        assert(book.first_ask_id(101) == 6);
        assert(book.first_ask_remaining(101) == 2);
        assert(book.bid_count(102) == 0);
    }

    {
        OrderBook book;
        book.add_order(Order{8, Side::Sell, 101, 3});
        book.add_order(Order{9, Side::Sell, 102, 5});
        const auto fills = book.add_order(Order{10, Side::Buy, 102, 6});
        assert(fills.size() == 2);
        check_fill(fills[0], 10, 8, 101, 3);
        check_fill(fills[1], 10, 9, 102, 3);
        assert(book.ask_count(101) == 0);
        assert(book.first_ask_remaining(102) == 2);
    }

    {
        OrderBook book;
        book.add_order(Order{11, Side::Buy, 101, 5});
        book.add_order(Order{12, Side::Buy, 102, 3});
        const auto fills = book.add_order(Order{13, Side::Sell, 101, 6});
        assert(fills.size() == 2);
        check_fill(fills[0], 13, 12, 102, 3);
        check_fill(fills[1], 13, 11, 101, 3);
        assert(book.bid_count(102) == 0);
        assert(book.first_bid_remaining(101) == 2);
    }

    {
        OrderBook book;
        book.add_order(Order{14, Side::Sell, 102, 6});
        const auto fills = book.add_order(Order{15, Side::Buy, 103, 10});
        assert(fills.size() == 1);
        check_fill(fills[0], 15, 14, 102, 6);
        assert(book.ask_count(102) == 0);
        assert(book.first_bid_remaining(103) == 4);
    }

    std::cout << "All order book checks passed\n";
}
