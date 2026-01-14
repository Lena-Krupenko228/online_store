#pragma once

#include <pqxx/pqxx>
#include <memory>
#include <string>

using std::string;
using std::unique_ptr;
using namespace pqxx;

// Шаблонный класс для подключения к PostgreSQL
template<typename T>
class DatabaseConnection
{
private:
    unique_ptr<connection> conn;
    unique_ptr<work> transaction;

public:
    explicit DatabaseConnection(const T &connectionString)
    {
        conn = std::make_unique<connection>(connectionString);
        if (!conn->is_open())
            throw std::runtime_error("Не удалось подключиться к БД");
    }

    void beginTransaction()
    {
        transaction = std::make_unique<work>(*conn);
    }

    void commitTransaction()
    {
        if (transaction)
        {
            transaction->commit();
            transaction.reset();
        }
    }

    void rollbackTransaction()
    {
        if (transaction)
        {
            transaction->abort();
            transaction.reset();
        }
    }

    bool getTransactionStatus() const
    {
        return transaction != nullptr;
    }

    pqxx::result executeQuery(const string &sql)
    {
        if (!transaction)
            beginTransaction();
        return transaction->exec(sql);
    }

    void executeNonQuery(const string &sql)
    {
        if (!transaction)
            beginTransaction();
        transaction->exec(sql);
    }

    void createFunction(const string &name, const string &body)
    {
        string sql = "CREATE OR REPLACE FUNCTION " + name + " " + body;
        executeNonQuery(sql);
    }

    void createTrigger(const string &triggerName,
                       const string &tableName,
                       const string &timing,
                       const string &events,
                       const string &functionName)
    {
        string sql =
            "CREATE OR REPLACE TRIGGER " + triggerName + " " +
            timing + " " + events + " ON " + tableName +
            " FOR EACH ROW EXECUTE FUNCTION " + functionName + "();";
        executeNonQuery(sql);
    }

    ~DatabaseConnection()
    {
        if (transaction)
            transaction->abort();
        if (conn && conn->is_open())
            conn->disconnect();
    }
};
