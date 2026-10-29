<?php
require_once 'db.php';

// Check for the 4 required fields
if (!isset($_GET['node_name'], $_GET['manufacturer'], $_GET['longitude'], $_GET['latitude'])) {
    http_response_code(400);
    exit('Missing required query parameters.');
}

$nodeName     = trim($_GET['node_name']);
$manufacturer = trim($_GET['manufacturer']);
$longitude    = (float)$_GET['longitude'];
$latitude     = (float)$_GET['latitude'];

try {
    $stmt = $pdo->prepare('
        INSERT INTO sensor_register (node_name, manufacturer, longitude, latitude) 
        VALUES (:node_name, :manufacturer, :longitude, :latitude)
    ');

    $stmt->execute([
        'node_name'     => $nodeName,
        'manufacturer' => $manufacturer,
        'longitude'    => $longitude,
        'latitude'     => $latitude,
    ]);

    echo 'Node registered successfully.';
} catch (PDOException $e) {
    error_log($e->getMessage());
    http_response_code(500);
    echo 'Failed to register node: ' . htmlspecialchars($e->getMessage());
}
