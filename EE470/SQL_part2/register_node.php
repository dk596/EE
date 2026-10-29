<?php
require_once 'db.php';

// Ensure all 4 required fields exist
if (!isset($_GET['node_name'], $_GET['manufacturer'], $_GET['longitude'], $_GET['latitude'])) {
    exit('Missing parameters.');
}

// Prepare and execute in one chained line
$sql = "INSERT INTO sensor_register (node_name, manufacturer, longitude, latitude) VALUES (?, ?, ?, ?)";
$pdo->prepare($sql)->execute([
    $_GET['node_name'],
    $_GET['manufacturer'],
    $_GET['longitude'],
    $_GET['latitude']
]);

echo "Node registered successfully.";
