-- sql/functions.sql
-- Хранимые функции для интернет-магазина

-- 1. getOrderStatus(order_id) - статус заказа
CREATE OR REPLACE FUNCTION getOrderStatus(p_order_id INTEGER)
RETURNS VARCHAR AS $$
BEGIN
    RETURN (SELECT status FROM orders WHERE order_id = p_order_id);
END;
$$ LANGUAGE plpgsql;

-- 2. getUserOrderCount(user_id) - количество заказов пользователя
CREATE OR REPLACE FUNCTION getUserOrderCount(p_user_id INTEGER)
RETURNS INTEGER AS $$
BEGIN
    RETURN (SELECT COUNT(*) FROM orders WHERE user_id = p_user_id);
END;
$$ LANGUAGE plpgsql;

-- 3. getTotalSpentByUser(user_id) - общая сумма трат пользователя
CREATE OR REPLACE FUNCTION getTotalSpentByUser(p_user_id INTEGER)
RETURNS DECIMAL(10,2) AS $$
BEGIN
    RETURN COALESCE((SELECT SUM(total_price) FROM orders WHERE user_id = p_user_id AND status = 'completed'), 0);
END;
$$ LANGUAGE plpgsql;

-- 4. getOrderStatusHistory(order_id) - история статусов заказа
CREATE OR REPLACE FUNCTION getOrderStatusHistory(p_order_id INTEGER)
RETURNS TABLE(history_id INT, order_id INT, old_status VARCHAR, new_status VARCHAR, changed_at TIMESTAMP, changed_by INT) AS $$
BEGIN
    RETURN QUERY
    SELECT h.history_id, h.order_id, h.old_status, h.new_status, h.changed_at, h.changed_by
    FROM order_status_history h
    WHERE h.order_id = p_order_id
    ORDER BY h.changed_at;
END;
$$ LANGUAGE plpgsql;

-- 5. canReturnOrder(order_id) - можно ли вернуть (completed + <30 дней)
CREATE OR REPLACE FUNCTION canReturnOrder(p_order_id INTEGER)
RETURNS BOOLEAN AS $$
DECLARE
    order_status VARCHAR;
    order_date TIMESTAMP;
BEGIN
    SELECT status, order_date INTO order_status, order_date
    FROM orders WHERE order_id = p_order_id;
    
    RETURN (order_status = 'completed' AND (CURRENT_TIMESTAMP - order_date) < INTERVAL '30 days');
END;
$$ LANGUAGE plpgsql;

-- 6. getAuditLogByUser(user_id) - аудит по пользователю
CREATE OR REPLACE FUNCTION getAuditLogByUser(p_user_id INTEGER)
RETURNS TABLE(log_id INT, entity_type VARCHAR, entity_id INT, operation VARCHAR, performed_at TIMESTAMP) AS $$
BEGIN
    RETURN QUERY
    SELECT al.log_id, al.entity_type, al.entity_id, al.operation, al.performed_at
    FROM audit_log al
    WHERE al.performed_by = p_user_id
    ORDER BY al.performed_at DESC;
END;
$$ LANGUAGE plpgsql;

