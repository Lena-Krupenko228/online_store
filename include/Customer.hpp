#pragma once

#include "User.hpp"

class Customer : public User
{
public:
    Customer(int id,
             const string &name,
             const string &email,
             DatabaseConnection<string> &db)
        : User(id, name, email, "customer", db)
    {}

    // Специфичные методы покупателя
    void addToOrder(int orderId, int productId, int quantity);
    void removeFromOrder(int orderId, int productId);
    void makePayment(int orderId);
    void requestReturn(int orderId);

    // Реализация виртуальных из User
    void createOrder() override;
    void viewOrderStatus(int orderId) override;
    void cancelOrder(int orderId) override;
};
