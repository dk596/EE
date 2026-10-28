<?php
require_once 'db.php';

try {
    // 1. Registered nodes sorted alphabetically by node_name
    $stmtNodes = $pdo->query('SELECT node_name, manufacturer, longitude, latitude FROM sensor_register ORDER BY node_name ASC');
    $nodes = $stmtNodes->fetchAll(PDO::FETCH_ASSOC);

    // 2. Incoming data ordered with newest readings first
    $stmtData = $pdo->query('SELECT node_name, time_received, temperature, humidity FROM sensor_data ORDER BY node_name ASC, time_received ASC LIMIT 100');
    $sensorData = $stmtData->fetchAll(PDO::FETCH_ASSOC);
} catch (PDOException $e) {
    error_log($e->getMessage());
    die('Failed to query tables: ' . htmlspecialchars($e->getMessage()));
}
?>
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Sensor Network Telemetry</title>
    <style>
        :root {
            --bg: #f8fafc;
            --surface: #ffffff;
            --border: #e2e8f0;
            --text-main: #0f172a;
            --text-muted: #64748b;
            --header-bg: #f1f5f9;
            --accent: #2563eb;
        }

        body {
            font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, Oxygen, Ubuntu, Cantarell, monospace;
            background-color: var(--bg);
            color: var(--text-main);
            margin: 0;
            padding: 2.5rem 1.5rem;
            line-height: 1.5;
        }

        .container {
            max-width: 1200px;
            margin: 0 auto;
        }

        .card {
            background: var(--surface);
            border: 1px solid var(--border);
            border-radius: 8px;
            box-shadow: 0 1px 3px rgba(0, 0, 0, 0.05);
            margin-bottom: 2.5rem;
            overflow: hidden;
        }

        .card-header {
            padding: 1.25rem 1.5rem;
            border-bottom: 1px solid var(--border);
            display: flex;
            align-items: center;
            justify-content: space-between;
        }

        h2 {
            margin: 0;
            font-size: 1.15rem;
            font-weight: 600;
            letter-spacing: -0.01em;
        }

        .badge {
            background-color: #eff6ff;
            color: var(--accent);
            font-size: 0.75rem;
            font-weight: 600;
            padding: 0.25rem 0.65rem;
            border-radius: 9999px;
            border: 1px solid #bfdbfe;
        }

        .table-responsive {
            width: 100%;
            overflow-x: auto;
        }

        table {
            width: 100%;
            border-collapse: collapse;
            text-align: left;
            font-size: 0.875rem;
        }

        th {
            background-color: var(--header-bg);
            color: var(--text-muted);
            font-weight: 600;
            text-transform: uppercase;
            font-size: 0.75rem;
            letter-spacing: 0.05em;
            padding: 0.75rem 1.25rem;
            border-bottom: 1px solid var(--border);
            white-space: nowrap;
        }

        td {
            padding: 0.85rem 1.25rem;
            border-bottom: 1px solid var(--border);
            color: var(--text-main);
            font-variant-numeric: tabular-nums;
        }

        tr:last-child td {
            border-bottom: none;
        }

        tbody tr:hover {
            background-color: #f8fafc;
        }

        .empty-state {
            padding: 2.5rem;
            text-align: center;
            color: var(--text-muted);
            font-size: 0.875rem;
        }
    </style>
</head>
<body>

<div class="container">

    <!-- Table 1: Registered Sensor Nodes -->
    <section class="card">
        <div class="card-header">
            <h2>Registered Sensor Nodes</h2>
            <span class="badge"><?= count($nodes) ?> Nodes</span>
        </div>
        <div class="table-responsive">
            <?php if (!empty($nodes)): ?>
                <table>
                    <thead>
                        <tr>
                            <th>Node Name</th>
                            <th>Manufacturer</th>
                            <th>Longitude</th>
                            <th>Latitude</th>
                        </tr>
                    </thead>
                    <tbody>
                        <?php foreach ($nodes as $node): ?>
                            <tr>
                                <td><strong><?= htmlspecialchars((string)$node['node_name']) ?></strong></td>
                                <td><?= htmlspecialchars((string)$node['manufacturer']) ?></td>
                                <td><?= htmlspecialchars((string)$node['longitude']) ?></td>
                                <td><?= htmlspecialchars((string)$node['latitude']) ?></td>
                            </tr>
                        <?php endforeach; ?>
                    </tbody>
                </table>
            <?php else: ?>
                <div class="empty-state">No registered nodes found.</div>
            <?php endif; ?>
        </div>
    </section>

    <!-- Table 2: Data Received -->
    <section class="card">
        <div class="card-header">
            <h2>Data Received</h2>
            <span class="badge"><?= count($sensorData) ?> Records</span>
        </div>
        <div class="table-responsive">
            <?php if (!empty($sensorData)): ?>
                <table>
                    <thead>
                        <tr>
                            <th>Node Name</th>
                            <th>Time Received</th>
                            <th>Temperature (°C)</th>
                            <th>Humidity (%)</th>
                        </tr>
                    </thead>
                    <tbody>
                        <?php foreach ($sensorData as $entry): ?>
                            <tr>
                                <td><strong><?= htmlspecialchars((string)$entry['node_name']) ?></strong></td>
                                <td><?= htmlspecialchars((string)$entry['time_received']) ?></td>
                                <td><?= htmlspecialchars((string)$entry['temperature']) ?></td>
                                <td><?= htmlspecialchars((string)$entry['humidity']) ?></td>
                            </tr>
                        <?php endforeach; ?>
                    </tbody>
                </table>
            <?php else: ?>
                <div class="empty-state">No incoming sensor data logged yet.</div>
            <?php endif; ?>
        </div>
    </section>

</div>

</body>
</html>
