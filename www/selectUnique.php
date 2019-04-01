<?php
   require_once('database.php');
   
   $db = new MyDB();

   $smt = $db->prepare('SELECT * from PLAYERS where SESSIONID=:sessionid');
   $smt->bindValue(':sessionid', $Player_ID, SQLITE3_TEXT);

   $ret = $smt->execute();
   $row = $ret->fetchArray(SQLITE3_ASSOC); 
   $scorePlayer = $row['SCORE'];

   $aux_id = $row['ID'];

   $db->close();

   $db = new MyDB();

   $smt = $db->prepare('SELECT * from ACTIONS where PLAYERS_ID=:playersid');
   $smt->bindValue(':playersid', $aux_id, SQLITE3_TEXT);


   $missions_array = array(0=>0, 1=>0, 2=>0, 3=>0, 4=>0, 5=>0, 6=>0);
   $ret = $smt->execute();
   while ($row = $ret->fetchArray(SQLITE3_ASSOC)) {
        if($row['TYPE'] == "mission" && $row['ISCOMPLETE']==1 && $row['ACTIONID']==1){
           $missions_array[0] = 1;
        }
        if($row['TYPE'] == "mission" && $row['ISCOMPLETE']==1 && $row['ACTIONID']==2){
            $missions_array[1] = 1;
        }
        if($row['TYPE'] == "mission" && $row['ISCOMPLETE']==1 && $row['ACTIONID']==3){
           $missions_array[2] = 1;
        }
        if($row['TYPE'] == "mission" && $row['ISCOMPLETE']==1 && $row['ACTIONID']==4){
            $missions_array[3] = 1;
        }
        if($row['TYPE'] == "mission" && $row['ISCOMPLETE']==1 && $row['ACTIONID']==5){
           $missions_array[4] = 1;
        }
        if($row['TYPE'] == "mission" && $row['ISCOMPLETE']==1 && $row['ACTIONID']==6){
           $missions_array[5] = 1;
        }
   }
   
   $db->close();
?>