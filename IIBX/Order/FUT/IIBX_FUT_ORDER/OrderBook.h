#ifndef ORDER_BOOK_H
#define ORDER_BOOK_H

#include <unordered_map>
#include <string>
#include <iostream>

//This is for mentain Order book hole session -> manage every order stage
struct Order {
    int ordertype;
    int price;
    int originalqty;
    int pendingqty;
    int tradeqty;

    // Default constructor with default values
    Order()
        : ordertype(0), price(0), originalqty(0), pendingqty(0), tradeqty(0) {}
};

// Define the map globally (if needed across files)
inline std::unordered_map<std::string, Order> orderBook;            //orderid, Order

// Function to add an order
inline void addPendingOrderDetail(std::string orderid, const Order& order) {
    auto it = orderBook.find(orderid);
    if (it != orderBook.end())
    {
        it->second.ordertype = order.ordertype;
        it->second.price = order.price;
        it->second.originalqty = order.originalqty;
        it->second.pendingqty = order.pendingqty;
        it->second.tradeqty = order.tradeqty;
    }
    else
    {
        orderBook[orderid] = order;
    }
}

inline bool isAvailableOrderId(std::string orderid)
{
    auto it = orderBook.find(orderid);
    if (it != orderBook.end())
        return true;
    else
        return false;
}

inline void changeParticularValue(std::string orderid, int value, std::string field)
{
    auto it = orderBook.find(orderid);
    if (it != orderBook.end())
    {
        if (field == "ordertype") {
            it->second.ordertype = value;
        }
        else if (field == "price") {
            it->second.price = value;
        }
        else if (field == "originalqty") {
            it->second.originalqty = value;
        }
        else if (field == "pendingqty") {
            it->second.pendingqty = value;
        }
        else if (field == "tradeqty") {
            it->second.tradeqty = value;
        }
        else {
            std::cout << "Invalid field!" << std::endl;
        }
    }
    else
    {
        std::cout << "Order not found!1" << std::endl;
    }
}

inline Order& getOrder(std::string orderid)
{
    auto it = orderBook.find(orderid);
    if (it != orderBook.end())
        return it->second;
    else {
        std::cout << "Order not found!" << std::endl;
        static Order defaultOrder;  
        return defaultOrder;
    }
}

inline void newEntry(std::string orderid, const Order& order)
{
    orderBook[orderid] = order;
}

// Function to delete an order
inline void deleteEntry(std::string orderid) {
    orderBook.erase(orderid);
}


#endif // ORDER_BOOK_H
