<?php
date_default_timezone_set('America/Los_Angeles');
require_once 'db.php';

$nodes = $pdo->query('SELECT * FROM sensor_register2 ORDER BY node_name')->fetchAll();
$sensorData = $pdo->query('SELECT * FROM sensor_data2 ORDER BY node_name, time_received')->fetchAll();

$selectedNode = $_GET['node'] ?? ($nodes[0]['node_name'] ?? '');
$filteredData = array_values(array_filter($sensorData, fn($r) => $r['node_name'] === $selectedNode));

$gaussValues = array_column($filteredData, 'gauss');
$presenceValues = array_column($filteredData, 'presence_detected');

$hasData = !empty($gaussValues);
$avgGauss = $hasData ? round(array_sum($gaussValues) / count($gaussValues), 2) : 0;
$maxGauss = $hasData ? max($gaussValues) : 0;
$minGauss = $hasData ? min($gaussValues) : 0;
$totalMotion = $hasData ? array_sum($presenceValues) : 0;
$totalCount = $hasData ? count($gaussValues) : 0;
?>
<!DOCTYPE html>
<html>
<head>
    <title>Sensor Data</title>
</head>
<body>

    <h2>Registered Sensor Nodes</h2>
    <table border="1">
        <tr><th>Node Name</th><th>Manufacturer</th><th>Longitude</th><th>Latitude</th></tr>
        <?php foreach ($nodes as $n): ?>
            <tr>
                <td><?= htmlspecialchars($n['node_name']) ?></td>
                <td><?= htmlspecialchars($n['manufacturer']) ?></td>
                <td><?= htmlspecialchars($n['longitude']) ?></td>
                <td><?= htmlspecialchars($n['latitude']) ?></td>
            </tr>
        <?php endforeach; ?>
    </table>

    <h2>Data Received</h2>
    <table border="1" id="dataTable">
        <tr><th>Node Name</th><th>Time Received</th><th>Gauss (Field Strength)</th><th>Presence (1=Yes, 0=No)</th></tr>
        <?php foreach ($sensorData as $d): ?>
            <tr>
                <td><?= htmlspecialchars($d['node_name']) ?></td>
                <td><?= htmlspecialchars($d['time_received']) ?></td>
                <td><?= htmlspecialchars($d['gauss']) ?></td>
                <td><?= htmlspecialchars($d['presence_detected']) ?></td>
            </tr>
        <?php endforeach; ?>
    </table>

    <h2>Node Statistics</h2>
    <form method="GET">
        <label for="node">Select Node:</label>
        <select name="node" id="node" onchange="this.form.submit()">
            <?php foreach ($nodes as $n): ?>
                <option value="<?= htmlspecialchars($n['node_name']) ?>" <?= $n['node_name'] === $selectedNode ? 'selected' : '' ?>>
                    <?= htmlspecialchars($n['node_name']) ?>
                </option>
            <?php endforeach; ?>
        </select>
        <noscript><button type="submit">Filter</button></noscript>
    </form>

    <?php if ($hasData): ?>
        <p>Total Measurements Received: <?= $totalCount ?></p>
        <p>Average Gauss: <?= $avgGauss ?> G</p>
        <p>Max Gauss: <?= $maxGauss ?> G</p>
        <p>Min Gauss: <?= $minGauss ?> G</p>
        <p>Total Motion Events Detected: <?= $totalMotion ?></p>

        <h2>Sensor Graphs (<?= htmlspecialchars($selectedNode) ?>)</h2>
        
        <!-- Graph Containers Stacked -->
        <div id="gaussContainer" style="width: 640px; margin-bottom: 40px;">
            <canvas id="canvasGauss"></canvas>
        </div>
        
        <div id="presenceContainer" style="width: 640px;">
            <canvas id="canvasPresence"></canvas>
        </div>

        <!-- Scripts for Chart.js -->
        <script src="https://ajax.googleapis.com/ajax/libs/jquery/3.6.0/jquery.min.js"></script>
        <script src="https://cdnjs.cloudflare.com/ajax/libs/Chart.js/3.7.1/chart.min.js"></script>
        
        <!-- Updated to load app2.js -->
        <script src="Chartjs/app2.js?v=<?= time() ?>"></script>
    <?php else: ?>
        <p>No telemetry recorded yet for <?= htmlspecialchars($selectedNode) ?>.</p>
    <?php endif; ?>

</body>
</html>
