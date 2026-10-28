<?php
require_once 'db.php';

$nodes = $pdo->query('SELECT * FROM sensor_register ORDER BY node_name ASC')->fetchAll(PDO::FETCH_ASSOC);
$sensorData = $pdo->query('SELECT * FROM sensor_data ORDER BY node_name ASC, time_received ASC')->fetchAll(PDO::FETCH_ASSOC);

$node1 = array_filter($sensorData, fn($r) => $r['node_name'] === 'node_1');
$avgTemp = round(array_sum(array_column($node1, 'temperature')) / count($node1), 2);
$avgHum  = round(array_sum(array_column($node1, 'humidity')) / count($node1), 2);
?>
<!DOCTYPE html>
<html>
<head>
    <title>Sensor Data</title>
</head>
<body>

    <h2>Registered Sensor Nodes</h2>
    <table border="1">
        <tr>
            <th>Node Name</th>
            <th>Manufacturer</th>
            <th>Longitude</th>
            <th>Latitude</th>
        </tr>
        <?php foreach ($nodes as $node): ?>
            <tr>
                <td><?= htmlspecialchars($node['node_name']) ?></td>
                <td><?= htmlspecialchars($node['manufacturer']) ?></td>
                <td><?= htmlspecialchars($node['longitude']) ?></td>
                <td><?= htmlspecialchars($node['latitude']) ?></td>
            </tr>
        <?php endforeach; ?>
    </table>

    <h2>Data Received</h2>
    <table border="1">
        <tr>
            <th>Node Name</th>
            <th>Time Received</th>
            <th>Temperature (°C)</th>
            <th>Humidity (%)</th>
        </tr>
        <?php foreach ($sensorData as $entry): ?>
            <tr>
                <td><?= htmlspecialchars($entry['node_name']) ?></td>
                <td><?= htmlspecialchars($entry['time_received']) ?></td>
                <td><?= htmlspecialchars($entry['temperature']) ?></td>
                <td><?= htmlspecialchars($entry['humidity']) ?></td>
            </tr>
        <?php endforeach; ?>
    </table>

    <p>Node 1 Avg Temp: <?= $avgTemp ?> °C</p>
    <p>Node 1 Avg Humidity: <?= $avgHum ?> %</p>

</body>
</html>
