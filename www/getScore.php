<?php
require_once('database.php');
$db = new MyDB();

$smt = $db->prepare('SELECT * from PLAYERS where SESSIONID=:sessionid');
$smt->bindValue(':sessionid', $_POST['name'], SQLITE3_TEXT);

$ret = $smt->execute();
$row = $ret->fetchArray(SQLITE3_ASSOC); 

echo $row['SCORE'];
?>