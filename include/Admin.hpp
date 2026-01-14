#pragma once

#include "User.hpp"

class Admin : public User
{
public:
    Admin(int id,
          const string &name,
          const string &email,
          DatabaseConnection<string> &db)
        : User(id, name, email, "admin", db)
    {}

    // Специфичные действия администратора
    void addProduct(const string &name, double price, int stock);
    void updateProduct(int productId, const string &name, double price, int stock);
    void deleteProduct(int productId);
    void viewAllOrders();
    void updateOrderStatus(int orderId, const string &newStatus);

    // Реализация виртуальных из User
    void createOrder() override;
    void viewOrderStatus(int orderId) override;
    void cancelOrder(int orderId) override;
};
