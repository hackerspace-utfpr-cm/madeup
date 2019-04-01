<?php
    class MissionsDB extends SQLite3 {
        function __construct() {
            $this->open('test.db');
            if(!$this->exec($this->sqlCreate)){
                //echo $this->lastErrorMsg();
            } else {
                //echo "Table created successfully\n";
            }
        }

        public $flag_loop;
        public $flag_if;
        public $flag_global;
        public $flag_subtrac;
        public $flag_full;

        function mission($SESSIONID, $POINTS){
            $smt = $this->prepare('SELECT * from PLAYERS where SESSIONID=:sessionid');
            $smt->bindValue(':sessionid', $SESSIONID, SQLITE3_TEXT);

            $ret = $smt->execute();
            $row = $ret->fetchArray(SQLITE3_ASSOC); 
            $var_score = $row['SCORE'] + $POINTS;
            $players_id = $row['ID'];

            $smt = $this->prepare('UPDATE PLAYERS set SCORE=:score WHERE SESSIONID=:sessionid');
            $smt->bindValue(':sessionid', $SESSIONID, SQLITE3_TEXT);
            $smt->bindValue(':score', $var_score, SQLITE3_INTEGER);

            $ret = $smt->execute();
            if(!$ret) {
            //echo $this->lastErrorMsg();
            } else {
            //echo $this->changes(), " Record updated successfully\n";
            }
            
            $smt = $this->prepare('UPDATE ACTIONS set ISCOMPLETE=1 WHERE ACTIONID=1 AND TYPE="mission" AND PLAYERS_ID=:players_id');
            $smt->bindValue(':players_id', $players_id, SQLITE3_TEXT);
            $ret = $smt->execute();
        }

        function missions($PLAYER_ID, $SESSIONID){
            $smt = $this->prepare('SELECT * from ACTIONS where PLAYERS_ID=:id');
            $smt->bindValue(':id', $PLAYER_ID, SQLITE3_TEXT);

            $ret = $smt->execute();
            while ($row = $ret->fetchArray(SQLITE3_ASSOC)) {
            if (($row['ACTIONID'] == 1) && ($row['ISCOMPLETE'] == 0)) {
                include 'mission1.php';
            } else {
                if (($row['ACTIONID'] == 2) && ($row['ISCOMPLETE'] == 0) && ($this->flag_loop == 1)) {
                        include 'mission2.php';
                }
                if (($row['ACTIONID'] == 3) && ($row['ISCOMPLETE'] == 0) && ($this->flag_if == 1)) {
                        include 'mission3.php';
                }
                if (($row['ACTIONID'] == 4) && ($row['ISCOMPLETE'] == 0) && ($this->flag_global == 1)) {
                        include 'mission4.php';
                }
                if (($row['ACTIONID'] == 5) && ($row['ISCOMPLETE'] == 0) && ($this->flag_subtrac == 1)) {
                        include 'mission5.php';
                }
                if (($row['ACTIONID'] == 6) && ($row['ISCOMPLETE'] == 0) && ($this->flag_full == 1)) {
                        include 'mission6.php';
                }
            }
            }
        }

        function selectMissions($SESSIONID, $flag_loop, $flag_if, $flag_global, $flag_subtrac, $flag_full){
            $this->flag_loop = $flag_loop;
            $this->flag_if = $flag_if;
            $this->flag_global = $flag_global;
            $this->flag_subtrac = $flag_subtrac;
            $this->flag_full = $flag_full;

            $smt = $this->prepare('SELECT * from PLAYERS where SESSIONID=:sessionid;');
            $smt->bindValue(':sessionid', $SESSIONID, SQLITE3_TEXT);

            $ret = $smt->execute();
            $row = $ret->fetchArray(SQLITE3_ASSOC);
            //echo "ID = ". $row['ID'] . "\n";
            //echo "SCORE = ". $row['SCORE'] ."\n";
            $PLAYER_ID = $row['ID'];
            include 'missions.php';
            //$this->missions($PLAYER_ID, $SESSIONID);
        }
    }
?>