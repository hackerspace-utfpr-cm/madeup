<?php
   require_once('database.php');
   
   $db = new MyDB();
   if(!$db) {
      //echo $db->lastErrorMsg();
   } else {
      //echo "Opened database successfully\n";
   }

   $smt = $db->prepare('SELECT * from PLAYERS where SESSIONID=:sessionid');
   $smt->bindValue(':sessionid', $SESSIONID, SQLITE3_TEXT);

   $ret = $smt->execute();
   $row = $ret->fetchArray(SQLITE3_ASSOC); 
   $var_score = $row['SCORE'] + 250;
   $players_id = $row['ID'];

   $smt = $db->prepare('UPDATE PLAYERS set SCORE=:score WHERE SESSIONID=:sessionid');
   $smt->bindValue(':sessionid', $SESSIONID, SQLITE3_TEXT);
   $smt->bindValue(':score', $var_score, SQLITE3_INTEGER);

   $ret = $smt->execute();
   if(!$ret) {
      //echo $db->lastErrorMsg();
   } else {
      //echo $db->changes(), " Record updated successfully\n";
   }
   $db->close();

   $db = new MyDB();   
   
   $smt = $db->prepare('UPDATE ACTIONS set ISCOMPLETE=1 WHERE ACTIONID=6 AND TYPE="mission" AND PLAYERS_ID=:players_id');
   $smt->bindValue(':players_id', $players_id, SQLITE3_TEXT);
   $ret = $smt->execute();
   
   $db->close();
?>