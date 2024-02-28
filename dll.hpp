/* this is an implementation of a doubly linked list with o(1) removal
the std::list version iterates through the whole list til it finds a match,
resulting in worst case o(n) this implementation directly manipulates the
pointers of the node that is to be removed we pass in the head and tail pointers
using a double pointer to directly manipulate the  pointer if we passed it using
a single pointer, any modifications would result in only the local copies of the
head and tail pointers to be changed, instead we want to manipulate the pointers
of the data structure itself this implementation assumes we have a map that
stores pointers to the nodes in the link so we can pass it in the remove
function */

namespace DLL {
struct node {
  node *next;
  node *prev;
  node() : prev(nullptr), next(nullptr){};
};

inline void PushBack(node **head, node *new_node) {
  if (*head == nullptr) {
    *head = new_node;
    return;
  }

  auto current = *head;
  while (current->next != nullptr) {
    current = current->next;
  }

  current->next = new_node;
  new_node->prev = current;
}

inline void Append(node **tail, node *new_node) {
  (*tail)->next = new_node;
  new_node->prev = *tail;
  *tail = new_node;
}

inline void Remove(node **head, node **tail, node *node) {
  if (node->prev != nullptr) {
    node->prev->next = node->next;
  }
  if (node->next != nullptr) {
    node->next->prev = node->prev;
  }
  if (*head == node) {
    *head = node->next;
  }
  if (*tail == node) {
    *tail = node->prev;
  }
  node->next = nullptr;
  node->prev = nullptr;
}
} // namespace DLL

// namespace DLL
// namespace DLL
