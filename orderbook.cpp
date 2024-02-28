#include "structures.cpp"
#include <iostream>
#include <map>
#include <unordered_map>
using namespace std;

// buyOrSell true = add bid, false = add offer

class order_book {
private:
  // buy and sell maps representing both sides of order book
  map<double, limit> buy_map;
  map<double, limit> sell_map;
  // top of book iterators to buy and sell maps, reverse buy iterator as the
  // best bid is highest value
  map<double, limit>::iterator tob_sell;
  map<double, limit>::reverse_iterator tob_buy;
  // limit lookup map, stores pointers to limit objects
  unordered_map<double, limit *> limits;
  // order lookup map, maps order id's to pointers of order objects
  unordered_map<int, order_node *> order_cache;

public:
  order_book()
      : buy_map(), sell_map(), limits(), order_cache(), tob_sell(), tob_buy() {}

  void add_limit_buy(int id, int size, int time, double price) {
    // create order objcet and place into order map
    order_node *new_order = new order_node(id, price, size, time, true);
    // if the limit buy order is above or equal to best offer, submit a market
    // buy, else it remains a limit order and add to order_cache
    if (!sell_map.empty()) {
      if (new_order->price >= tob_sell->second.price) {
        market_order_buy(new_order);
      }
    } else {
      // if corersponding limit does not exist, create the limit object, update
      // parent limit of new order, store in maps, and update iterators
      if (limits.find(new_order->price) == limits.end()) {
        limit *new_limit = new limit(new_order);
        new_order->parent = new_limit;
        limits[price] = new_limit;
        buy_map[price] = *new_limit;
        tob_buy = buy_map.rbegin();
        order_cache[id] = new_order;
      }
      // if limit object exists, access by reference using by map, add order,
      // and update parent & iterator
      else {
        // limit& current_limit = buy_map[price]; this will result in log(m), we
        // want o(1);
        limit *current_limit = limits[price];
        current_limit->add_order(new_order);
        new_order->parent = current_limit;
        tob_buy = buy_map.rbegin();
        order_cache[id] = new_order;
      }
    }
  }

  // same logic as a limit buy
  void add_limit_sell(int id, int size, int time, double price) {
    order_node *new_order = new order_node(id, price, size, time, false);
    if (!buy_map.empty()) {

      if (new_order->price <= tob_buy->second.price) {
        market_order_sell(new_order);
      }
    } else {
      if (limits.count(new_order->price) == 0) {
        limit *new_limit = new limit(new_order);
        new_order->parent = new_limit;
        limits[price] = new_limit;
        sell_map[price] = *new_limit;
        tob_sell = sell_map.begin();
        order_cache[id] = new_order;
      } else {
        // limit& current_limit = sell_map[price];
        limit *current_limit = limits[price];
        order_cache[id] = new_order;
        current_limit->add_order(new_order);
        new_order->parent = current_limit;
        tob_sell = sell_map.begin();
      }
    }
  }

  /* cancel a given order using the order id as a paramter
  uses the order cache to get the pointer of the node that is to be removed,
  gets the corresponding limit object of the node, and cancels the order by
  passing in the pointer to the order node directly */
  void cancel_order(int id) {
    order_node *to_be_cancelled = order_cache[id];
    limit *parent_limit = to_be_cancelled->parent;
    parent_limit->remove_cancelled_order(to_be_cancelled);
    // tob_sell = sell_map.begin();
    std::cout << parent_limit->head_order->idNumber << endl;
    std::cout << tob_sell->second.head_order->idNumber << endl;
  }

  void remove_order(limit *curr_limit) { curr_limit->remove_order_front(); }

  /* places a market order using fifo order matching as referenced by the
  CME
  https://www.cmegroup.com/confluence/display/EPICSANDBOX/CME+Globex+Matching+Algorithm+Steps#CMEGlobexMatchingAlgorithmSteps-FIFO
  uses the top of book iterators to get the best offer, and checks the first
  order to see if there is enough size to fill the market order, if so
  update quantities indicating a match, if there is not enough size, fills
  as much of the market order as possible, cancels the exhausted limit
  order, thus updating the head order */

  void market_order_sell(order_node *new_order) {
    while (tob_buy != buy_map.rend()) {
      auto match = tob_buy->second.head_order;
      if (match->size >= new_order->size) {
        if (match->size == new_order->size) {
          new_order->size = 0;
          // match->size = 0;
          match->filled = true;
          limit *curr_limit = match->parent;
          // cancel_order(match->idNumber);
          remove_order(curr_limit);
          match->size = 0;
          return;
        } else {
          match->size -= new_order->size;
          new_order->size = 0;
          new_order->filled = true;
          return;
        }
      }
      new_order->size -= match->size;
      match->filled = true;
      limit *curr_limit = match->parent;
      cancel_order(match->idNumber);
      // match->size = 0;
      if (tob_buy->second.head_order == tob_buy->second.tail_order) {
        /*auto it = tob_buy.base();
        --it;
        buy_map.erase(it); */
        // to convert between iterators, call .base() and adjust for offset
        buy_map.erase(--(tob_buy.base()));
      }
      tob_buy = buy_map.rbegin();
    }
  }

  void market_order_buy(order_node *new_order) {
    while (tob_sell != sell_map.end()) {
      auto match = tob_sell->second.head_order;
      // std::cout << match->size << endl;

      // std::cout << match->idNumber << endl;
      if (match->size >= new_order->size) {
        if (match->size == new_order->size) {
          new_order->size = 0;
          match->filled = true;
          limit *curr_limit = match->parent;
          //  remove_order(curr_limit);

          cancel_order(match->idNumber);
          match->size = 0;
          return;
        } else {
          match->size -= new_order->size;

          new_order->size = 0;
          new_order->filled = true;
          return;
        }
      }

      new_order->size -= match->size;
      match->filled = true;
      limit *curr_limit = match->parent;
      // remove_order(curr_limit);
      cancel_order(match->idNumber);
      //  std::cout << match->idNumber;
      if (tob_sell->second.num_orders == 0) {
        sell_map.erase(tob_sell);
        tob_sell = sell_map.begin();
      }
    }
  }

  void create_market_order(int id, int size, int time, bool buyOrSell) {
    order_node *new_order = new order_node(id, size, time, buyOrSell);
    if (new_order->buyOrSell == true) {
      market_order_buy(new_order);
    } else {
      market_order_sell(new_order);
    }
  }

  bool has_order(int id) { return (order_cache.find(id) != order_cache.end()); }

  bool limit_contains(int id) {
    auto it = order_cache.find(id);
    if (it == order_cache.end()) {
      return false;
    }
    order_node *curr_order = it->second;
    if (!curr_order || !curr_order->parent) {
      return false;
    }
    limit *curr_limit = curr_order->parent;
    order_node *curr = curr_limit->head_order;
    while (curr) {
      if (curr == curr_order) {
        return true;
      }
      curr = curr->next;
      if (curr == curr_limit->tail_order->next) {
        break;
      }
    }
    return false;
  }

  void display_size(int id) {
    order_node *curr_order = order_cache[id];
    std::cout << curr_order->size << endl;
  }

  void display_limit_size(double price) {
    limit *curr_limit = limits[price];
    std::cout << curr_limit->volume << "....." << curr_limit->num_orders
              << endl;
  }
};
