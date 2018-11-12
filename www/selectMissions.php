<?php

require_once('database.php');

$db = new MyDB();
if (!$db) {
    //echo $db->lastErrorMsg();
} else {
    //echo "Opened database successfully\n";
}

$smt = $db->prepare('SELECT * from PLAYERS where SESSIONID=:sessionid;');
$smt->bindValue(':sessionid', $SESSIONID, SQLITE3_TEXT);

$ret = $smt->execute();
$row = $ret->fetchArray(SQLITE3_ASSOC);
//echo "ID = ". $row['ID'] . "\n";
//echo "SCORE = ". $row['SCORE'] ."\n";
$PLAYER_ID = $row['ID'];
//echo "Operation done successfully\n";
$db->close();
include 'missions.php'
?>