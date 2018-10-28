<?php
require_once 'database.php';
$db = new MyDB();
if (!$db) {
    echo $db->lastErrorMsg();
} else {
    echo "Opened database successfully\n";
}

$sql = <<<EOF
      SELECT * from ACTIONS;
EOF;
$aux_id = -1;
$ret = $db->query($sql);
echo "Actions: ";
while ($row = $ret->fetchArray(SQLITE3_ASSOC)) {
    if($aux_id != $row['PLAYERS_ID']){
        $aux_id = $row['PLAYERS_ID'];
        echo "\n  PLAYERS_ID = " . $row['PLAYERS_ID'] . "\n";
    }
    echo "   -TYPE = " . $row['TYPE'] . "\n";
    echo "    ACTIONID = " . $row['ACTIONID'] . "\n";
    echo "    ISCOMPLETE = " . $row['ISCOMPLETE'] . "\n----------------------------\n";
}
echo "Operation done successfully\n";
$db->close();
?>