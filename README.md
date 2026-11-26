# C++ Limit Order Book

A limit order book in C++20 that matches incoming limit orders against resting orders by price-time priority. I am building it to learn C++ and the mechanics of exchange matching, and I am extending it step by step.

## What it does

- Accepts buy and sell **limit orders** and matches them against the opposite side of the book.
- **Price priority:** a buy trades against the lowest ask first; a sell trades against the highest bid first.
- **Time priority:** orders at the same price fill in arrival order (FIFO).
- **Partial fills:** an incoming order can fill part of a resting order, and a resting order can be filled by several incoming orders.
- **Multi-level matching:** an aggressive order keeps trading through price levels until its limit price stops it or it is fully filled.
- Any unfilled remainder **rests in the book** at its limit price.
- Every match produces a `Fill` (incoming ID, resting ID, price, quantity). Trades happen at the **resting order's price**.

## Design

| Piece | Choice | Why |
| --- | --- | --- |
| Price levels | `std::map<Price, std::deque<Order>>`, one map per side | Keeps levels sorted, so the best bid (`rbegin`) and best ask (`begin`) are direct lookups, and insert / erase are O(log n) |
| Queue per level | `std::deque<Order>` | FIFO: match from the front with `pop_front()`, new orders join with `push_back()` |
| Types | `OrderId`, `Price` (`int64_t`), `Quantity` (`uint64_t`) | Integer prices avoid floating-point rounding |
| Matching | `add_order` → private `match_buy` / `match_sell` | Matches on a local copy of the incoming order, then rests any remainder |
| Cleanup | Filled orders are popped; empty price levels are erased | Keeps best-price lookups correct |

## Project layout

```
src/
  order.hpp             OrderId, Price, Quantity, Side, Order, Fill
  order_book.hpp        OrderBook interface
  order_book.cpp        matching logic and queries
  order_book_test.cpp   automated tests
  main.cpp              small demo that prints book state
```

## Build and run

Requires g++ with C++20 support (developed on Ubuntu WSL).

```bash
cd src

# Run the tests
g++ -std=c++20 -Wall -Wextra -Werror order_book_test.cpp order_book.cpp -o order_book_test
./order_book_test
# Expected: All order book checks passed

# Run the demo
g++ -std=c++20 -Wall -Wextra main.cpp order_book.cpp -o order
./order
```

## Tests

`order_book_test.cpp` uses `assert` and covers:

- resting buy and sell orders and best bid / best ask
- an exact fill that empties both sides
- arrival order at one price, with a partial fill of the second order
- a buy that trades across two ask levels
- a sell that trades across two bid levels (highest bid first)
- an aggressive buy whose remainder rests as a new bid

Note: `assert` is disabled if you compile with `-DNDEBUG`, so build the tests without it.

## Known limitations

- No order cancellation or modification yet.
- No market orders.
- No validation of input: zero quantity and duplicate order IDs are not rejected yet.
- `best_bid_price()` / `best_ask_price()` return `0` for an empty side, which is ambiguous with a real price of 0.
- Finding an order by ID would currently require scanning the book.

## Roadmap

1. Choose a market data feed (order-level) for replay testing.
2. Write input rules: zero quantity, duplicate IDs, unknown IDs, price or quantity changes.
3. Cancel by order ID.
4. Modify orders with a defined priority rule.
5. Market orders.
6. A simple reference matcher and seeded randomized tests that compare both implementations after every command.
7. Benchmarks: orders per second and memory use on a replayed feed.

## License

MIT, see [LICENSE](LICENSE).
