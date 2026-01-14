#pragma once

#include <string>
#include <memory>

using std::string;
using std::unique_ptr;

// Абстрактная стратегия оплаты
class PaymentStrategy
{
public:
    virtual ~PaymentStrategy() = default;
    virtual void pay(double amount) = 0;   // чисто виртуальная
};

// Конкретные стратегии
class CardPayment : public PaymentStrategy
{
public:
    void pay(double amount) override;
};

class EWalletPayment : public PaymentStrategy
{
public:
    void pay(double amount) override;
};

class SBPPayment : public PaymentStrategy
{
public:
    void pay(double amount) override;
};

// Класс Payment, который использует стратегию (композиция)
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
};
