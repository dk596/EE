<?php
require_once 'db.php';

if (isset($_GET['data'])) {
    $_GET += (array) json_decode(base64_decode($_GET['data']), true);
}

if (!isset($_GET['node_name'], $_GET['time_received'], $_GET['presence_detected'], $_GET['gauss'])) {
    exit('Missing parameters. node_name, time_received, presence_detected, and gauss are required.');
}

$sql = "INSERT INTO sensor_data2 (node_name, time_received, presence_detected, gauss) VALUES (?, ?, ?, ?)";
$params = [
    $_GET['node_name'], 
    $_GET['time_received'], 
    $_GET['presence_detected'], 
    $_GET['gauss']
];

try {
    // Attempt to insert the data
    $pdo->prepare($sql)->execute($params);
    echo "Data inserted successfully.";
} catch (PDOException $e) {
    // Error 23000 is the SQL code for a Primary Key Duplicate constraint violation
    if ($e->getCode() == 23000) {
        echo "Duplicate entry: Already stored.";
    } else {
        http_response_code(500);
        echo "Database error: " . $e->getMessage();
    }
}
