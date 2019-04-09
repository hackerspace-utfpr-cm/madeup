<?php
   class MyDB extends SQLite3 { 
      private $sqlCreate = <<<EOF
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
      private $sqlSelectPlayers = <<<EOF
      SELECT * from PLAYERS;
EOF;
      public $ID = -1;
      public $flag = 1;

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

      function insertAction($ACTIONID){
         $smt = $this->prepare("insert into ACTIONS (ACTIONID, TYPE, ISCOMPLETE, PLAYERS_ID) values (:actionid, :type, :iscomplete, :players_id)");
         $smt->bindValue(':actionid', $ACTIONID, SQLITE3_INTEGER);
         $smt->bindValue(':type', "mission", SQLITE3_TEXT);
         $smt->bindValue(':iscomplete', 0, SQLITE3_INTEGER);
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
            // Or you could throw an exception, or otherwise handle the error...
         }

         $ret = $smt->execute();
         if (!$ret) {
            //echo $this->lastErrorMsg();
         } else {
            //echo "Records created successfully\n";
         }

         for($i = 1; $i < 7; $i++){
            $this->insertAction($i);
         }
      }

      function selectPlayer($SESSIONID){
         $ret = $this->query($this->sqlSelectPlayers);
         while($row = $ret->fetchArray(SQLITE3_ASSOC) ) {
            //echo "ID = ". $row['ID'] . "\n";
            //echo "SCORE = ". $row['SCORE'] ."\n";
            $this->ID= $row['ID'];
            if($SESSIONID == $row['SESSIONID']){
               $this->flag = 0;
            }
         }
         $this->ID = $this->ID + 1;
         //echo "Operation done successfully\n";
         if($this->flag == 1){
            $this->insertPlayer($SESSIONID);
         }
      }
   }
?>
