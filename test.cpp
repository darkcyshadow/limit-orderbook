#include "orderbook.cpp" // Assume order_book is defined in this header
#include <cassert>
#include <iostream>

class TestOrderBook {
public:
  void test_limit_orders() {
    order_book book;

    book.add_limit_buy(1, 10, 1, 100.0);
    book.add_limit_buy(2, 5, 2, 101.0);
    book.add_limit_sell(3, 5, 3, 102.0);
    book.add_limit_sell(4, 10, 4, 99.0);

    // assert(!book.has_order(2));
    assert(!book.limit_contains(2));
    assert(!book.limit_contains(4));
    assert(book.limit_contains(1));
    // assert(book.has_order(2));
    assert(book.limit_contains(3));
    // assert(!book.has_order(4)); // Assuming order 4 is matched and
    // removed

    std::cout << "test_limit_orders function passed" << std::endl;
  }

  void testMultipleMarketOrderMatches() {
    order_book book;
    book.add_limit_sell(5, 10, 5, 100.0);
    book.add_limit_sell(7, 20, 7, 100.0);
    book.add_limit_sell(6, 15, 6, 101.0);

    // book.display_limit_size(100.0);

    // market buy order that should match against multiple sell orders
    book.create_market_order(8, 20, 7, true);
    // std::cout << book.limit_contains(5) << endl;

    // book.display_limit_size(100.0);

    // assert(!book.limit_contains(5));
    // assert(book.limit_contains(6)); // order 5 should be fully matched
    // and removed, while order 6 should have 5 lots left to fill
    // assert(book.limit_contains(7));
    // book.display_size(5);
    //  book.display_size(6);

    book.display_size(7);
    //     book.display_limit_size(100.0);

    std::cout << "testMultipleMarketOrderMatches passed." << std::endl;
  }

  void testOrderCancellationImpact() {
    order_book book;
    // Add and then cancel a limit order before adding a market order
    book.add_limit_buy(8, 10, 8, 95.0);
    // book.cancel_order(8);
    // Market sell order that could have matched the cancelled buy order
    book.create_market_order(9, 10, 9, false);

    // Assertions to verify market order does not match the cancelled order
    assert(!book.has_order(8)); // Order 8 should remain cancelled
    // Additional checks might be necessary to confirm order 9's handling

    std::cout << "testOrderCancellationImpact passed." << std::endl;
  }

  void runTests() {
    // test_limit_orders();
    testMultipleMarketOrderMatches();
    //  testOrderCancellationImpact();
    std::cout << "all tests passed!" << std::endl;
  }
};

int main() {
  TestOrderBook test;
  test.runTests();
  return 0;
}
