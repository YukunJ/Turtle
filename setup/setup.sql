-- For a clean start
DROP DATABASE  IF EXISTS test_db;
CREATE DATABASE test_db;
DROP USER IF EXISTS 'tester'@'localhost';
CREATE USER 'tester'@'localhost' IDENTIFIED BY 'tester';

-- pick the newly created test db
USE test_db;

-- create an empty new table
CREATE TABLE user(
    username char(50) NULL,
    passwd char(50) NULL
) ENGINE=InnoDB;

-- populate with initial data
INSERT INTO user(username, passwd) VALUES
    ('Jason', '123'),
    ('Tommy', '456'),
    ('Mary', '789');

-- grant access to the tester user
GRANT ALL PRIVILEGES ON test_db.user TO 'tester'@'localhost';    

