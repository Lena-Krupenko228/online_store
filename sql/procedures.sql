-- sql/procedures.sql
-- Хранимые процедуры

-- 1. createOrder - создание заказа с элементами (с транзакцией)
CREATE OR REPLACE PROCEDURE createOrder(
    p_user_id INTEGER,
    p_items JSONB,  -- [{"product_id":1,"quantity":2},{"product_id":2,"quantity":1}]
    OUT p_order_id INTEGER,
    OUT p_total_price DECIMAL
)
LANGUAGE plpgsql
AS $$
DECLARE
    item_rec RECORD;
    prod_price DECIMAL;
    prod_stock INTEGER;
    item_total DECIMAL;
BEGIN
    -- Начинаем транзакцию (автоматически)
    
    -- Создаём заказ
    INSERT INTO orders (user_id, status, total_price) 
    VALUES (p_user_id, 'pending', 0) 
    RETURNING order_id, total_price INTO p_order_id, p_total_price;
    
    -- Добавляем элементы
    FOR item_rec IN SELECT * FROM jsonb_array_elements(p_items)
    LOOP
        SELECT price, stock_quantity INTO prod_price, prod_stock
        FROM products WHERE product_id = (item_rec->>'product_id')::INTEGER;
        
        IF prod_stock < (item_rec->>'quantity')::INTEGER THEN
            RAISE EXCEPTION 'Недостаточно товара % на складе', (item_rec->>'product_id')::INTEGER;
        END IF;
        
        item_total := prod_price * (item_rec->>'quantity')::INTEGER;
        p_total_price := p_total_price + item_total;
        
        -- Обновляем заказ и склад
        UPDATE orders SET total_price = p_total_price WHERE order_id = p_order_id;
        INSERT INTO order_items (order_id, product_id, quantity, price) 
        VALUES (p_order_id, (item_rec->>'product_id')::INTEGER, (item_rec->>'quantity')::INTEGER, prod_price);
        UPDATE products SET stock_quantity = stock_quantity - (item_rec->>'quantity')::INTEGER 
        WHERE product_id = (item_rec->>'product_id')::INTEGER;
    END LOOP;
    
    -- Коммит автоматический, если нет ошибки
END;
$$;

-- 2. updateOrderStatus - смена статуса с логом в audit (вызовет триггер истории)
CREATE OR REPLACE PROCEDURE updateOrderStatus(
    p_order_id INTEGER,
    p_new_status VARCHAR,
    p_user_id INTEGER
)
LANGUAGE plpgsql
AS $$
BEGIN
    UPDATE orders 
    SET status = p_new_status 
    WHERE order_id = p_order_id;
    
    -- Лог в audit_log
    INSERT INTO audit_log (entity_type, entity_id, operation, performed_by)
    VALUES ('order', p_order_id, 'update', p_user_id);
END;
$$;

