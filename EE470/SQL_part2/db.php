<?php

$host    = 'localhost';
$dbname  = 'u130146560_DerekKan';
$user    = 'u130146560_db_DerekKan';
$pass    = '5e6B;V2tmZG#K?@';
$charset = 'utf8mb4';

$dsn = "mysql:host=$host;dbname=$dbname;charset=$charset";

$options = [
    PDO::ATTR_ERRMODE            => PDO::ERRMODE_EXCEPTION,
    PDO::ATTR_DEFAULT_FETCH_MODE => PDO::FETCH_ASSOC,
    PDO::ATTR_EMULATE_PREPARES   => false,
];

try {
    $pdo = new PDO($dsn, $user, $pass, $options);
    $pdo->exec("SET time_zone = '-07:00';"); // -07:00 for PDT, -08:00 for PST
} catch (PDOException $e) {
    error_log($e->getMessage());
    die('Database connection error.');
}

