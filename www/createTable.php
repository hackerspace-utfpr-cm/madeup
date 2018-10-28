<?php
   require_once('database.php');
   $db = new MyDB();
   if(!$db) {
      //echo $db->lastErrorMsg();
   } else {
      //echo "Opened database successfully\n";
   }

   $sql =<<<EOF
   CREATE TABLE IF NOT EXISTS PLAYERS (
      ID INT NOT NULL,
      SESSIONID CHAR(50) NULL,
      SCORE INT NULL,
      PRIMARY KEY (ID));
    
    CREATE TABLE IF NOT EXISTS ACTIONS (
      ACTIONID INT NULL,
      TYPE CHAR(50) NULL,
      ISCOMPLETE INT NULL,
      PLAYERS_ID INT NOT NULL
        REFERENCES PLAYERS (ID)
        ON DELETE NO ACTION
        ON UPDATE NO ACTION);

EOF;

   $ret = $db->exec($sql);
   if(!$ret){
      //echo $db->lastErrorMsg();
   } else {
      //echo "Table created successfully\n";
   }
   $db->close();
   include 'selectInsert.php';
?>
