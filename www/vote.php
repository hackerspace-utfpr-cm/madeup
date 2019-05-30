<?php 
require_once("database.php");
$db = New MyDB();

$sqlSelectActions = <<<EOF
SELECT * from ACTIONS;
EOF;

$arr = $_POST['name'];

echo $_POST['name'];


$smt = $db->prepare('SELECT * from PLAYERS where SESSIONID=:sessionid');
$smt->bindValue(':sessionid', $arr, SQLITE3_TEXT);

$ret = $smt->execute();
$row = $ret->fetchArray(SQLITE3_ASSOC); 
$var_score = $row['SCORE'] + 100;
$players_id = $row['ID'];

$smt = $db->prepare('UPDATE PLAYERS set SCORE=:score WHERE SESSIONID=:sessionid');
$smt->bindValue(':sessionid', $arr, SQLITE3_TEXT);
$smt->bindValue(':score', $var_score, SQLITE3_INTEGER);
$ret = $smt->execute();


$AID = 0;
$ret = $db->query($sqlSelectActions);
while($row = $ret->fetchArray(SQLITE3_ASSOC) ) {
      //echo "\nID = ". $row['ACTIONID'] . "\n";
      $AID = $row['ACTIONID'];
}
$AID = $AID + 1;

$smt = $db->prepare("insert into ACTIONS (ACTIONID, TYPE, ISCOMPLETE, MISSIONNUMBER, PLAYERS_ID) values (:actionid, :type, :iscomplete, :missionnumber, :players_id)");
$smt->bindValue(':actionid', $AID, SQLITE3_INTEGER);
$smt->bindValue(':type', "vote", SQLITE3_TEXT);
$smt->bindValue(':iscomplete', 1, SQLITE3_INTEGER);
$smt->bindValue(':missionnumber', 0, SQLITE3_INTEGER);
$smt->bindValue(':players_id', $players_id, SQLITE3_INTEGER);

$ret = $smt->execute();

$db->close();

?>