#pragma once

#include <string>

using std::string;

class Product
{
private:
    int productId;
    string name;
    double price;
    int stockQuantity;

public:
    Product(int id, const string &name, double price, int stock)
        : productId(id), name(name), price(price), stockQuantity(stock) {}

    int getId() const { return productId; }
    const string &getName() const { return name; }
    double getPrice() const { return price; }
    int getStockQuantity() const { return stockQuantity; }

    void setPrice(double p) { price = p; }
    void setStockQuantity(int q) { stockQuantity = q; }
};
