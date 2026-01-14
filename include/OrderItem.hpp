#pragma once

#include <string>

using std::string;

// Элемент заказа: какой товар, сколько и по какой цене
class OrderItem
{
private:
    int productId;   // ID товара из таблицы products
    int quantity;    // количество
    double price;    // цена за штуку на момент заказа

public:
    OrderItem(int productId, int quantity, double price)
        : productId(productId), quantity(quantity), price(price) {}

    int getProductId() const { return productId; }
    int getQuantity() const { return quantity; }
    double getPrice() const { return price; }

    void setQuantity(int q) { quantity = q; }
    void setPrice(double p) { price = p; }
};
