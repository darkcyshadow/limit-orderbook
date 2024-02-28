#include "dll.hpp"
#include <iostream>
#include <list>

/* TODO:
- execution time for order matching
- callbacks if order has filled
- object pool for limit objects might be faster rather than creating/destroying
- iceberg orders
*/

struct limit;

struct order_node : DLL ::node {
  int idNumber;
  double price;
  int size;
  int time;
  bool buyOrSell;
  bool filled;
  order_node *next;
  order_node *prev;
  // include pointer to parent limit for order cancellation
  limit *parent;

  order_node()
      : idNumber(0), price(0.0), size(0), time(0), buyOrSell(false),
        parent(nullptr), next(nullptr), prev(nullptr), filled(false) {}

  order_node(int id, double p, int s, int t, bool bs)
      : idNumber(id), price(p), size(s), time(t), buyOrSell(bs), next(nullptr),
        prev(nullptr), filled(false) {}

  order_node(int id, int s, int t, bool bs)
      : idNumber(id), price(0.0), size(s), time(0), buyOrSell(bs),
        next(nullptr), prev(nullptr), filled(false) {}
};

// limit struct includes head and tail pointers to store order objects
/* we use reintrepret_cast f and tail pointers because there variabels are
pointers to an object of type 'OrderNode' however, the DLL:: function calls
expect objects of type DLL::node, since 'OrderNode" inherits from DLL::node,
their layouts in memory are compatible we can use reinterpret cast as a
low-level reinterpretation of the pointer types static cast is used on the
new_order object because it directly inherits from the DLL::node object */

struct limit {
  int volume;
  int num_orders;
  double price;
  order_node *head_order;
  order_node *tail_order;

  limit()
      : volume(0), price(0.0), num_orders(0), head_order(nullptr),
        tail_order(nullptr) {}

  limit(order_node *new_order)
      : volume(new_order->size), price(new_order->price), num_orders(1),
        head_order(new_order), tail_order(new_order) {}

  void add_order(order_node *new_order) {
    if (new_order) {
      if (head_order == nullptr && tail_order == nullptr) {
        head_order = tail_order = new_order;
      } else {
        DLL::Append(reinterpret_cast<DLL::node **>(&tail_order),
                    static_cast<DLL::node *>(new_order));
      }
      volume += new_order->size;
      num_orders++;
    }
  }

  void remove_order_front() {
    if (head_order != nullptr) {
      auto front = head_order;
      volume -= front->size;
      num_orders--;
      head_order = front->next;

      if (head_order != nullptr) {
        head_order->prev = nullptr;
      } else {
        tail_order = nullptr;
      }
      delete front;
    }
  }

  void remove_cancelled_order(order_node *cancelled_order) {
    if (cancelled_order) {
      // std::cout << head_order->idNumber;
      DLL::Remove(reinterpret_cast<DLL::node **>(&head_order),
                  reinterpret_cast<DLL::node **>(&tail_order),
                  static_cast<DLL::node *>(cancelled_order));
      // std::cout << head_order->idNumber << std ::endl;
      volume -= cancelled_order->size;
      num_orders--;
    }
  }
};
