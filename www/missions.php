<?php

require_once 'database.php';

$db = new MyDB();
if (!$db) {
    //echo $db->lastErrorMsg();
} else {
    //echo "Opened database successfully\n";
}
$smt = $db->prepare('SELECT * from ACTIONS where PLAYERS_ID=:id');
$smt->bindValue(':id', $PLAYER_ID, SQLITE3_TEXT);


$ret = $smt->execute();
while ($row = $ret->fetchArray(SQLITE3_ASSOC)) {
    if (($row['ACTIONID'] == 1) && ($row['ISCOMPLETE'] == 0)) {
        include 'mission1.php';
    } else {
        if (($row['ACTIONID'] == 2) && ($row['ISCOMPLETE'] == 0) && ($flag_loop == 1)) {
            include 'mission2.php';
        }
        if (($row['ACTIONID'] == 3) && ($row['ISCOMPLETE'] == 0) && ($flag_if == 1)) {
            include 'mission3.php';
        }
        if (($row['ACTIONID'] == 4) && ($row['ISCOMPLETE'] == 0) && ($flag_global == 1)) {
            include 'mission4.php';
        }
        if (($row['ACTIONID'] == 5) && ($row['ISCOMPLETE'] == 0) && ($flag_subtrac == 1)) {
            include 'mission5.php';
        }
        if (($row['ACTIONID'] == 6) && ($row['ISCOMPLETE'] == 0) && ($flag_full == 1)) {
            include 'mission6.php';
        }
    }
}
$db->close();
?>