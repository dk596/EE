<?php
require_once 'db.php';

// If base64 is present, decode it and merge into $_GET
if (isset($_GET['data'])) {
    $_GET += (array) json_decode(base64_decode($_GET['data']), true);
}

// Ensure required fields exist
if (!isset($_GET['node_name'], $_GET['temperature'], $_GET['humidity'])) {
    exit('Missing parameters.');
}

// Insert into database (uses time_received if passed, otherwise NOW())
$sql = isset($_GET['time_received'])
    ? "INSERT INTO sensor_data (node_name, time_received, temperature, humidity) VALUES (?, ?, ?, ?)"
    : "INSERT INTO sensor_data (node_name, time_received, temperature, humidity) VALUES (?, NOW(), ?, ?)";

$params = isset($_GET['time_received'])
    ? [$_GET['node_name'], $_GET['time_received'], $_GET['temperature'], $_GET['humidity']]
    : [$_GET['node_name'], $_GET['temperature'], $_GET['humidity']];

$pdo->prepare($sql)->execute($params);

echo "Data inserted successfully.";
