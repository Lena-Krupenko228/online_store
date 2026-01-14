#pragma once

#include "DatabaseConnection.hpp"
#include <memory>
#include <string>
#include <vector>
#include <algorithm>
#include <numeric>

using std::string;
using std::shared_ptr;
using std::vector;

class Order; // объявим позже

// Абстрактный пользователь
class User
{
protected:
    int userId;
    string name;
    string email;
    string role;

    // База данных – ссылка на внешний объект
    DatabaseConnection<string> &db;

    // Агрегация заказов
    vector<shared_ptr<Order>> orders;

public:
    User(int id,
         const string &name,
         const string &email,
         const string &role,
         DatabaseConnection<string> &db)
        : userId(id), name(name), email(email), role(role), db(db)
    {}

    virtual ~User() = default;

    // Чисто виртуальные (обязаны быть реализованы в наследниках)
    virtual void createOrder() = 0;
    virtual void viewOrderStatus(int orderId) = 0;
    virtual void cancelOrder(int orderId) = 0;

    const string &getRole() const { return role; }
    int getId() const { return userId; }

    // Пример: фильтрация заказов по статусу (лямбда + copy_if)
    vector<shared_ptr<Order>> getOrdersByStatus(const string &status);
};

