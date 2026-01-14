#pragma once

#include "User.hpp"

class Manager : public User
{
public:
    Manager(int id,
            const string &name,
            const string &email,
            DatabaseConnection<string> &db)
        : User(id, name, email, "manager", db)
    {}

    // Специфичные методы менеджера
    void approveOrder(int orderId);
    void updateStock(int productId, int deltaQuantity);
    void viewPendingOrders();

    // Реализация виртуальных из User
    void createOrder() override;
    void viewOrderStatus(int orderId) override;
    void cancelOrder(int orderId) override;
};
