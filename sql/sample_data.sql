-- sql/sample_data.sql
-- Тестовые данные

-- Пользователи
INSERT INTO users (name, email, role, password_hash, loyalty_level) VALUES
('Admin User', 'admin@mail.ru', 'admin', 'hashed_pass_admin', 1),
('Manager Ivanov', 'manager@mail.ru', 'manager', 'hashed_pass_mgr', 0),
('Lena', 'customer@mail.ru', 'hashed_pass_cust', 1);

-- Продукты
INSERT INTO products (name, price, stock_quantity) VALUES
('Laptop Dell', 50000.00, 10),
('Mouse Logitech', 1500.00, 50),
('Keyboard', 3000.00, 20);

-- Проверка
SELECT * FROM users;
SELECT * FROM products;
