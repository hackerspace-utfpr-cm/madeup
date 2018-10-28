<?php
require_once 'database.php';
$db = new MyDB();
if (!$db) {
    //echo $db->lastErrorMsg();
} else {
    //echo "Opened database successfully\n";
}
$smt = $db->prepare("insert into PLAYERS (ID, SESSIONID, SCORE) values (:id, :sessionid, :score)");
$smt->bindValue(':id', $ID, SQLITE3_INTEGER);
$smt->bindValue(':sessionid', $SESSIONID, SQLITE3_TEXT);
$smt->bindValue(':score', 0, SQLITE3_INTEGER);

if (!$smt) {
    die($db->errorInfo());
    // Or you could throw an exception, or otherwise handle the error...
}

$ret = $smt->execute();
if (!$ret) {
    //echo $db->lastErrorMsg();
} else {
    //echo "Records created successfully\n";
}
$db->close();

$db = new MyDB();
$smt2 = $db->prepare("insert into ACTIONS (ACTIONID, TYPE, ISCOMPLETE, PLAYERS_ID) values (:actionid, :type, :iscomplete, :players_id)");
$smt2->bindValue(':actionid', 1, SQLITE3_INTEGER);
$smt2->bindValue(':type', "mission", SQLITE3_TEXT);
$smt2->bindValue(':iscomplete', 0, SQLITE3_INTEGER);
$smt2->bindValue(':players_id', $ID, SQLITE3_INTEGER);

$ret2 = $smt2->execute();
if (!$ret2) {
    //echo $db->lastErrorMsg();
} else {
    //echo "Records created successfully\n";
}
$db->close();

$db = new MyDB();
$smt2 = $db->prepare("insert into ACTIONS (ACTIONID, TYPE, ISCOMPLETE, PLAYERS_ID) values (:actionid, :type, :iscomplete, :players_id)");
$smt2->bindValue(':actionid', 2, SQLITE3_INTEGER);
$smt2->bindValue(':type', "mission", SQLITE3_TEXT);
$smt2->bindValue(':iscomplete', 0, SQLITE3_INTEGER);
$smt2->bindValue(':players_id', $ID, SQLITE3_INTEGER);

$ret2 = $smt2->execute();
if (!$ret2) {
    //echo $db->lastErrorMsg();
} else {
    //echo "Records created successfully\n";
}
$db->close();

$db = new MyDB();
$smt2 = $db->prepare("insert into ACTIONS (ACTIONID, TYPE, ISCOMPLETE, PLAYERS_ID) values (:actionid, :type, :iscomplete, :players_id)");
$smt2->bindValue(':actionid', 3, SQLITE3_INTEGER);
$smt2->bindValue(':type', "mission", SQLITE3_TEXT);
$smt2->bindValue(':iscomplete', 0, SQLITE3_INTEGER);
$smt2->bindValue(':players_id', $ID, SQLITE3_INTEGER);

$ret2 = $smt2->execute();
if (!$ret2) {
    //echo $db->lastErrorMsg();
} else {
    //echo "Records created successfully\n";
}
$db->close();

$db = new MyDB();
$smt2 = $db->prepare("insert into ACTIONS (ACTIONID, TYPE, ISCOMPLETE, PLAYERS_ID) values (:actionid, :type, :iscomplete, :players_id)");
$smt2->bindValue(':actionid', 4, SQLITE3_INTEGER);
$smt2->bindValue(':type', "mission", SQLITE3_TEXT);
$smt2->bindValue(':iscomplete', 0, SQLITE3_INTEGER);
$smt2->bindValue(':players_id', $ID, SQLITE3_INTEGER);

$ret2 = $smt2->execute();
if (!$ret2) {
    //echo $db->lastErrorMsg();
} else {
    //echo "Records created successfully\n";
}
$db->close();

$db = new MyDB();
$smt2 = $db->prepare("insert into ACTIONS (ACTIONID, TYPE, ISCOMPLETE, PLAYERS_ID) values (:actionid, :type, :iscomplete, :players_id)");
$smt2->bindValue(':actionid', 5, SQLITE3_INTEGER);
$smt2->bindValue(':type', "mission", SQLITE3_TEXT);
$smt2->bindValue(':iscomplete', 0, SQLITE3_INTEGER);
$smt2->bindValue(':players_id', $ID, SQLITE3_INTEGER);

$ret2 = $smt2->execute();
if (!$ret2) {
    //echo $db->lastErrorMsg();
} else {
    //echo "Records created successfully\n";
}
$db->close();

$db = new MyDB();
$smt2 = $db->prepare("insert into ACTIONS (ACTIONID, TYPE, ISCOMPLETE, PLAYERS_ID) values (:actionid, :type, :iscomplete, :players_id)");
$smt2->bindValue(':actionid', 6, SQLITE3_INTEGER);
$smt2->bindValue(':type', "mission", SQLITE3_TEXT);
$smt2->bindValue(':iscomplete', 0, SQLITE3_INTEGER);
$smt2->bindValue(':players_id', $ID, SQLITE3_INTEGER);

$ret2 = $smt2->execute();
if (!$ret2) {
    //echo $db->lastErrorMsg();
} else {
    //echo "Records created successfully\n";
}
$db->close();

?>