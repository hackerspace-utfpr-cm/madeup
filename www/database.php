<?php
   class MyDB extends SQLite3 { 
      private $sqlCreate = <<<EOF
      CREATE TABLE IF NOT EXISTS PLAYERS (
         ID INT NOT NULL
         PRIMARY KEY,
         SESSIONID CHAR(50) NULL,
         SCORE INT NULL);
         
      CREATE TABLE IF NOT EXISTS ACTIONS (
         ACTIONID INT NOT NULL
         PRIMARY KEY,
         TYPE CHAR(50) NULL,
         ISCOMPLETE INT NULL,
         MISSIONNUMBER INT NULL,
         PLAYERS_ID INT NOT NULL
         REFERENCES PLAYERS (ID));
         
      CREATE TABLE IF NOT EXISTS OBJECTS (
         OBJECTTID INT NOT NULL,
         SESSIONID CHAR(50) NULL,
         IMAGEADD CHAR(50) NULL,
         PRIMARY KEY (OBJECTTID));	
         
      CREATE TABLE IF NOT EXISTS ACTION_has_OBJECTS (
         ACTIONS_ACTIONID INT NOT NULL
         REFERENCES ACTIONS (ACTIONID),
         OBJECTS_OBJECTID INT NOT NULL
         REFERENCES OBJECTS (OBJECTTID));
EOF;
      private $sqlSelectPlayers = <<<EOF
      SELECT * from PLAYERS;
EOF;
      private $sqlSelectActions = <<<EOF
      SELECT * from ACTIONS;
EOF;
      public $ID = -1;
      public $AID = -1;
      public $flag = 1;
      public $aflag = 1;

      function __construct() {
         $this->open('test.db');
         $this->busyTimeout(5000);
         $this->exec('PRAGMA journal_mode = wal;');
         if(!$this->exec($this->sqlCreate)){
            //echo $this->lastErrorMsg();
         } else {
            //echo "Table created successfully\n";
         }
      }

      function insertAction($number){
         $smt = $this->prepare("insert into ACTIONS (ACTIONID, TYPE, ISCOMPLETE, MISSIONNUMBER, PLAYERS_ID) values (:actionid, :type, :iscomplete, :missionnumber, :players_id)");
         $smt->bindValue(':actionid', $this->AID, SQLITE3_INTEGER);
         $smt->bindValue(':type', "mission", SQLITE3_TEXT);
         $smt->bindValue(':iscomplete', 0, SQLITE3_INTEGER);
         $smt->bindValue(':missionnumber', $number, SQLITE3_INTEGER);
         $smt->bindValue(':players_id', $this->ID, SQLITE3_INTEGER);

         $ret = $smt->execute();
      }

      function insertPlayer($SESSIONID){
         $smt = $this->prepare("insert into PLAYERS (ID, SESSIONID, SCORE) values (:id, :sessionid, :score)");
         $smt->bindValue(':id', $this->ID, SQLITE3_INTEGER);
         $smt->bindValue(':sessionid', $SESSIONID, SQLITE3_TEXT);
         $smt->bindValue(':score', 0, SQLITE3_INTEGER);

         if (!$smt) {
            die($this->errorInfo());
         }

         $ret = $smt->execute();
         if (!$ret) {
            //echo $this->lastErrorMsg();
         } else {
            //echo "Records created successfully\n";
         }

         $ret = $this->query($this->sqlSelectActions);
         while($row = $ret->fetchArray(SQLITE3_ASSOC) ) {
            //echo "\nID = ". $row['ACTIONID'] . "\n";
            $this->AID= $row['ACTIONID'];
         }
         $this->AID = $this->AID + 1;

         for($i = 1; $i < 7; $i++){
            $this->insertAction($i);
            $this->AID = $this->AID + 1;
         }
      }

      function selectPlayer($SESSIONID){
         $ret = $this->query($this->sqlSelectPlayers);
         while($row = $ret->fetchArray(SQLITE3_ASSOC) ) {
            //echo "\nID = ". $row['ID'] . "\n";
            //echo "SCORE = ". $row['SCORE'] ."\n";
            $this->ID= $row['ID'];
            if($SESSIONID == $row['SESSIONID']){
               $this->flag = 0;
            }
         }
         $this->ID = $this->ID + 1;
         //echo "\nOperation done successfully\n";
         if($this->flag == 1){
            $this->insertPlayer($SESSIONID);
         }
      }
   }
?>
