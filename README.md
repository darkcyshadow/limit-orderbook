WORK IN PROGRESS

This is a limit orderbook implementation in c++. 
The basic idea is to use 2 maps (buy and sell) representing the opposite sides of the book. 
We can store pointers to limit objects in a seperate unordered_map<int, limit*>, thus resulting in o(1) lookup for limits. 
Each limit objects has a double linked list representing the limit orders associated with each price. 
We can also store pointers to each node in the double linked list in a seperate unordered_map<int, order_node*>, resulting in o(1) lookup for each order. 
