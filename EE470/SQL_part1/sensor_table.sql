-- 1. Create sensor_register table (Primary Key: node_name)
CREATE TABLE sensor_register (
    node_name VARCHAR(10) PRIMARY KEY,
    manufacturer VARCHAR(10) NOT NULL,
-- 15 total characters, 6 decimal places
    longitude DECIMAL(15, 6) NOT NULL,
    latitude DECIMAL(15, 6) NOT NULL
);

-- 2. Create sensor_data table (Foreign Key references sensor_register)
CREATE TABLE sensor_data (
    node_name VARCHAR(10) NOT NULL,
    time_received DATETIME NOT NULL,
    temperature DECIMAL(6, 2) NOT NULL CHECK (temperature BETWEEN -10.00 AND 100.00),
    humidity DECIMAL(6, 2) NOT NULL CHECK (humidity BETWEEN 0.00 AND 100.00),
    PRIMARY KEY (node_name, time_received),
    CONSTRAINT fk_sensor_register
        FOREIGN KEY (node_name) 
        REFERENCES sensor_register(node_name)
        ON UPDATE CASCADE 
        ON DELETE RESTRICT
);

INSERT INTO sensor_register (node_name, manufacturer, longitude, latitude) VALUES
('node_1', 'Bosch',     -122.419416,  37.774929),
('node_2', 'Honeywell',  -73.935242,  40.730610),
('node_3', 'Sensirion',    8.541694,  47.376887),
('node_4', 'TexasInst',  -96.797000,  32.776700),
('node_5', 'Siemens',     11.581981,  48.135125);

INSERT INTO sensor_data (node_name, time_received, temperature, humidity) VALUES
-- Node 1 readings
('node_1', '2022-10-01 11:00:00', 21.50, 45.20),
('node_1', '2022-10-01 11:30:00', 22.10, 44.80),
('node_1', '2022-10-01 12:00:00', 23.00, 43.50),
('node_1', '2022-10-01 12:30:00', 23.40, 42.90),

-- Node 2 readings
('node_2', '2022-10-01 11:00:00', 18.20, 55.40),
('node_2', '2022-10-01 11:30:00', 18.90, 54.10),
('node_2', '2022-10-01 12:00:00', 19.50, 53.00),
('node_2', '2022-10-01 12:30:00', 20.00, 52.20),

-- Node 3 readings
('node_3', '2022-10-01 11:00:00', 14.30, 60.10),
('node_3', '2022-10-01 11:30:00', 14.80, 59.50),
('node_3', '2022-10-01 12:00:00', 15.20, 58.70),
('node_3', '2022-10-01 12:30:00', 15.70, 58.00),

-- Node 4 readings
('node_4', '2022-10-01 11:00:00', 28.60, 38.30),
('node_4', '2022-10-01 11:30:00', 29.20, 37.80),
('node_4', '2022-10-01 12:00:00', 30.10, 36.90),
('node_4', '2022-10-01 12:30:00', 30.50, 36.00),

-- Node 5 readings
('node_5', '2022-10-01 11:00:00', 16.00, 50.00),
('node_5', '2022-10-01 11:30:00', 16.50, 49.30),
('node_5', '2022-10-01 12:00:00', 17.10, 48.60),
('node_5', '2022-10-01 12:30:00', 17.40, 48.00);

-- create virtual table
CREATE VIEW sensor_combined AS
SELECT 
    d.node_name,
    r.manufacturer,
    r.latitude,
    r.longitude,
    d.time_received,
    d.temperature,
    d.humidity
FROM sensor_data d
INNER JOIN sensor_register r ON d.node_name = r.node_name;


SELECT * FROM sensor_combined;
