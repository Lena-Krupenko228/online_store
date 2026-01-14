#include "../include/DatabaseConnection.hpp"
#include <iostream>
#include <memory>

using namespace std;
using namespace pqxx;

// -------- Платежи --------
class PaymentStrategy {
public:
    virtual ~PaymentStrategy() = default;
    virtual void pay(double amount) = 0;
};

class CardPayment : public PaymentStrategy {
public:
    void pay(double amount) override {
        cout << "Оплата " << amount << " по банковской карте.\n";
    }
};

class EWalletPayment : public PaymentStrategy {
public:
    void pay(double amount) override {
        cout << "Оплата " << amount << " через электронный кошелёк.\n";
    }
};

class SBPPayment : public PaymentStrategy {
public:
    void pay(double amount) override {
        cout << "Оплата " << amount << " через СБП.\n";
    }
};

class Payment {
private:
    double amount;
    unique_ptr<PaymentStrategy> strategy;

public:
    Payment(double amount, unique_ptr<PaymentStrategy> strategy)
        : amount(amount), strategy(move(strategy)) {}

    void process() {
        if (strategy) strategy->pay(amount);
    }
};

// -------- Заказы --------
class OrderItem {
private:
    int productId;
    int quantity;
    double price;

public:
    OrderItem(int productId, int quantity, double price)
        : productId(productId), quantity(quantity), price(price) {}
};

class Order {
private:
    int orderId;
    string status;
    vector<unique_ptr<OrderItem>> items;
    unique_ptr<Payment> payment;

public:
    Order(int id, const string &status)
        : orderId(id), status(status) {}

    void addItem(unique_ptr<OrderItem> item) {
        items.push_back(move(item));
    }

    void setPayment(unique_ptr<Payment> p) {
        payment = move(p);
    }

    void processPayment() {
        if (payment) payment->process();
    }
};

// -------- Пользователи --------
class User {
protected:
    int userId;
    string name;
    string email;
    string role;
    DatabaseConnection<string> &db;

public:
    User(int id, const string &name, const string &email,
         const string &role, DatabaseConnection<string> &db)
        : userId(id), name(name), email(email), role(role), db(db) {}

    virtual ~User() = default;

    virtual void createOrder() = 0;
    virtual void viewOrderStatus(int orderId) = 0;
    virtual void cancelOrder(int orderId) = 0;
    virtual void menu() = 0;
};

class Admin : public User {
public:
    Admin(int id, const string &name, const string &email,
          DatabaseConnection<string> &db)
        : User(id, name, email, "admin", db) {}

    void addProduct() {
        string pname;
        double price;
        int stock;
        cout << "Введите название товара: ";
        cin >> pname;
        cout << "Введите цену: ";
        cin >> price;
        cout << "Введите количество: ";
        cin >> stock;

        db.executeNonQuery(
            "INSERT INTO products (name, price, stock_quantity) VALUES ('" +
            pname + "', " + to_string(price) + ", " + to_string(stock) + ")"
        );
        cout << "✓ Товар добавлен.\n";
    }

    void showAllOrders() {
        auto res = db.executeQuery(
            "SELECT order_id, user_id, status, total_price FROM orders"
        );
        for (auto row : res) {
            cout << "Заказ " << row[0].as<int>()
                 << ", пользователь " << row[1].as<int>()
                 << ", статус " << row[2].c_str()
                 << ", сумма " << row[3].c_str() << "\n";
        }
    }

    void updateOrderStatus() {
        int oid;
        string st;
        cout << "ID заказа: ";
        cin >> oid;
        cout << "Новый статус: ";
        cin >> st;
        db.executeNonQuery(
            "CALL updateOrderStatus(" + to_string(oid) + ", '" + st + "', " + to_string(userId) + ")"
        );
        cout << "✓ Статус изменён.\n";
    }

    void createOrder() override {}
    void viewOrderStatus(int orderId) override {
        auto res = db.executeQuery("SELECT status FROM orders WHERE order_id = " + to_string(orderId));
        if (!res.empty())
            cout << "Статус: " << res[0][0].c_str() << "\n";
    }
    void cancelOrder(int orderId) override {}

    void menu() override {
        while (true) {
            cout << "\nМЕНЮ АДМИНИСТРАТОРА:\n"
                 << "1. Добавить товар\n"
                 << "2. Показать заказы\n"
                 << "3. Изменить статус\n"
                 << "0. Выход\n>> ";
            char ch; cin >> ch;
            if (ch == '0') break;
            if (ch == '1') addProduct();
            else if (ch == '2') showAllOrders();
            else if (ch == '3') updateOrderStatus();
        }
    }
};

class Manager : public User {
public:
    Manager(int id, const string &name, const string &email,
            DatabaseConnection<string> &db)
        : User(id, name, email, "manager", db) {}

    void approveOrder() {
        int oid; cout << "ID заказа: "; cin >> oid;
        db.executeNonQuery(
            "CALL updateOrderStatus(" + to_string(oid) + ", 'completed', " + to_string(userId) + ")"
        );
        cout << "✓ Утверждено.\n";
    }

    void createOrder() override {}
    void viewOrderStatus(int) override {}
    void cancelOrder(int) override {}

    void menu() override {
        while (true) {
            cout << "\nМЕНЮ МЕНЕДЖЕРА:\n"
                 << "1. Утвердить заказ\n"
                 << "0. Выход\n>> ";
            char ch; cin >> ch;
            if (ch == '0') break;
            if (ch == '1') approveOrder();
        }
    }
};

class Customer : public User {
public:
    Customer(int id, const string &name, const string &email,
             DatabaseConnection<string> &db)
        : User(id, name, email, "customer", db) {}

    void makePayment() {
        int oid, method;
        double sum;
        cout << "ID заказа: "; cin >> oid;
        cout << "Сумма: "; cin >> sum;
        cout << "Способ (1-карта, 2-кошелёк, 3-СБП): "; cin >> method;

        unique_ptr<PaymentStrategy> s;
        if (method == 1) s = make_unique<CardPayment>();
        else if (method == 2) s = make_unique<EWalletPayment>();
        else s = make_unique<SBPPayment>();

        Payment p(sum, move(s));
        p.process();

        db.executeNonQuery(
            "CALL updateOrderStatus(" + to_string(oid) + ", 'completed', " + to_string(userId) + ")"
        );
    }

    void createOrder() override {
        cout << "✓ Заказ создан.\n";
    }
    void viewOrderStatus(int orderId) override {
        auto res = db.executeQuery("SELECT status FROM orders WHERE order_id = " + to_string(orderId));
        if (!res.empty())
            cout << "Статус: " << res[0][0].c_str() << "\n";
    }
    void cancelOrder(int orderId) override {}

    void menu() override {
        while (true) {
            cout << "\nМЕНЮ ПОКУПАТЕЛЯ:\n"
                 << "1. Создать заказ\n"
                 << "2. Оплатить\n"
                 << "0. Выход\n>> ";
            char ch; cin >> ch;
            if (ch == '0') break;
            if (ch == '1') createOrder();
            else if (ch == '2') makePayment();
        }
    }
};

// -------- MAIN --------
int main() {
    try {
        string connStr = "dbname=exam user=postgres password=123456 hostaddr=127.0.0.1 port=5432";
        DatabaseConnection<string> db(connStr);

        while (true) {
            cout << "\n===== ИНТЕРНЕТ-МАГАЗИН =====\n"
                 << "1. Администратор\n"
                 << "2. Менеджер\n"
                 << "3. Покупатель\n"
                 << "0. Выход\n>> ";
            char ch; cin >> ch;
            if (ch == '0') break;

            if (ch == '1') {
                auto admin = make_unique<Admin>(1, "Admin", "admin@test.com", db);
                admin->menu();
            } else if (ch == '2') {
                auto manager = make_unique<Manager>(2, "Manager", "mgr@test.com", db);
                manager->menu();
            } else if (ch == '3') {
                auto customer = make_unique<Customer>(3, "Customer", "cust@test.com", db);
                customer->menu();
            }
        }
    } catch (const exception &e) {
        cerr << "Ошибка: " << e.what() << "\n";
    }

    return 0;
}
