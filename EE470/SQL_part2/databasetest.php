<?php
date_default_timezone_set('America/Los_Angeles');
require_once 'db.php';

$nodes = $pdo->query('SELECT * FROM sensor_register ORDER BY node_name ASC')->fetchAll(PDO::FETCH_ASSOC);
$sensorData = $pdo->query('SELECT * FROM sensor_data ORDER BY node_name ASC, time_received ASC')->fetchAll(PDO::FETCH_ASSOC);

// Get selected node from dropdown (defaults to the first registered node if none chosen)
$selectedNode = $_GET['node'] ?? ($nodes[0]['node_name'] ?? '');

// Filter data for the selected node
$filteredData = array_filter($sensorData, fn($r) => $r['node_name'] === $selectedNode);
$temps  = array_column($filteredData, 'temperature');
$humids = array_column($filteredData, 'humidity');

// Compute stats if readings exist for this node
$hasData = !empty($temps);
$avgTemp = $hasData ? round(array_sum($temps) / count($temps), 2) : 0;
$avgHum  = $hasData ? round(array_sum($humids) / count($humids), 2) : 0;
$maxTemp = $hasData ? max($temps) : 0;
$minHum  = $hasData ? min($humids) : 0;
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
            <th>Temperature (°F)</th>
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

    <h2>Node Statistics</h2>
    <form method="GET">
        <label for="node">Select Node:</label>
        <select name="node" id="node" onchange="this.form.submit()">
            <?php foreach ($nodes as $node): ?>
                <option value="<?= htmlspecialchars($node['node_name']) ?>" <?= $node['node_name'] === $selectedNode ? 'selected' : '' ?>>
                    <?= htmlspecialchars($node['node_name']) ?>
                </option>
            <?php endforeach; ?>
        </select>
        <noscript><button type="submit">Filter</button></noscript>
    </form>

    <?php if ($hasData): ?>
        <p>Average Temperature: <?= $avgTemp ?> °F</p>
        <p>Average Humidity: <?= $avgHum ?> %</p>
        <p>Max Temperature: <?= $maxTemp ?> °F</p>
        <p>Minimum Humidity: <?= $minHum ?> %</p>

        <h2>Temperature vs Time (<?= htmlspecialchars($selectedNode) ?>)</h2>
        <div style="width: 640px;">
            <canvas id="mycanvas"></canvas>
        </div>

        <script src="https://ajax.googleapis.com/ajax/libs/jquery/3.6.0/jquery.min.js"></script>
        <script src="https://cdnjs.cloudflare.com/ajax/libs/Chart.js/3.7.1/chart.min.js"></script>
        <script src="Chartjs/app.js?v=<?= time() ?>"></script>
    <?php else: ?>
        <p>No telemetry recorded yet for <?= htmlspecialchars($selectedNode) ?>.</p>
    <?php endif; ?>
    <h2>Node Data (JSON Format)</h2>
    <pre><?= json_encode(array_values($filteredData), JSON_PRETTY_PRINT) ?></pre>
</body>
</html>
