<?php
require_once 'database.php';
$db = new MyDB();
if (!$db) {
    echo $db->lastErrorMsg();
} else {
    echo "Opened database successfully\n";
}

$sql = <<<EOF
      SELECT * from PLAYERS;
EOF;
$ret = $db->query($sql);
echo "Players: \n";
while ($row = $ret->fetchArray(SQLITE3_ASSOC)) {
    echo " -ID = " . $row['ID'] . "\n";
    echo "  SESSIONID = " . $row['SESSIONID'] . "\n";
    echo "  SCORE = " . $row['SCORE'] . "\n";
}
echo "Operation done successfully\n";
$db->close();
?>
