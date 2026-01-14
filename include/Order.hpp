#pragma once

#include <memory>
#include <vector>
#include <string>

using std::string;
using std::unique_ptr;
using std::vector;

class OrderItem;
class Payment;  // из Payment.hpp

// Заказ: набор позиций + статус + оплата
class Order
{
private:
    int orderId;
    string status;                       // pending/completed/canceled/returned
    vector<unique_ptr<OrderItem>> items; // композиция: Order владеет OrderItem
    unique_ptr<Payment> payment;         // композиция: оплата живёт вместе с заказом

public:
    Order(int id, const string &status)
        : orderId(id), status(status) {}

    int getId() const { return orderId; }
    const string &getStatus() const { return status; }
    void setStatus(const string &s) { status = s; }

    // Добавление позиции в заказ
    void addItem(unique_ptr<OrderItem> item)
    {
        items.push_back(std::move(item));
    }

    const vector<unique_ptr<OrderItem>> &getItems() const
    {
        return items;
    }

    // Установка оплаты
    void setPayment(unique_ptr<Payment> p)
    {
        payment = std::move(p);
    }

    Payment *getPayment() const
    {
        return payment.get();
    }
};
