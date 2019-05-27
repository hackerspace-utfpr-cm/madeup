<?php
require_once('database.php');
$db = New MyDB();

$sql = <<<EOF
      SELECT * from PLAYERS;
EOF;
$ret = $db->query($sql);
$list = array();
$i = 0;
while ($row = $ret->fetchArray(SQLITE3_ASSOC)) {
    $list[$i] = $row['SESSIONID'];
    $i++;
}

$jsonList = json_encode($list);
echo $jsonList;

$db->close();
?>