#pragma once

#include <memory>
#include <string>
#include <iostream>

using std::string;
using std::unique_ptr;

// Абстрактная стратегия оплаты
class PaymentStrategy
{
public:
    virtual ~PaymentStrategy() = default;
    virtual void pay(double amount) = 0;   // чисто виртуальная
};

// Оплата банковской картой
class CardPayment : public PaymentStrategy
{
public:
    void pay(double amount) override
    {
        std::cout << "Оплата " << amount << " по банковской карте.\n";
    }
};

// Электронный кошелёк
class EWalletPayment : public PaymentStrategy
{
public:
    void pay(double amount) override
    {
        std::cout << "Оплата " << amount << " через электронный кошелёк.\n";
    }
};

// Система быстрых платежей (СБП)
class SBPPayment : public PaymentStrategy
{
public:
    void pay(double amount) override
    {
        std::cout << "Оплата " << amount << " через СБП.\n";
    }
};

// Класс Payment, использующий стратегию
class Payment
{
private:
    double amount;
    unique_ptr<PaymentStrategy> strategy;

public:
    Payment(double amount, unique_ptr<PaymentStrategy> strategy)
        : amount(amount), strategy(std::move(strategy)) {}

    void process()
    {
        if (strategy)
            strategy->pay(amount);
    }

    void setAmount(double a) { amount = a; }
    double getAmount() const { return amount; }
};
