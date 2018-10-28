<?php

require_once('database.php');
 
  $db = new MyDB();
   if(!$db) {
      //echo $db->lastErrorMsg();
   } else {
      //echo "Opened database successfully\n";
   }

   $sql =<<<EOF
      SELECT * from PLAYERS;
EOF;
   $ID = -1;
   $flag = 1;

   $ret = $db->query($sql);
   while($row = $ret->fetchArray(SQLITE3_ASSOC) ) {
      //echo "ID = ". $row['ID'] . "\n";
      //echo "SCORE = ". $row['SCORE'] ."\n";
      $ID = $row['ID'];
      if($SESSIONID == $row['SESSIONID']){
         $flag = 0;
      }
   }
   $ID = $ID + 1;
   //echo "Operation done successfully\n";
   $db->close();
   if($flag == 1){
      include "insert.php";
   }
?>
