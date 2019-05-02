<?php
    class MissionsDB extends SQLite3 {
        function __construct() {
            $this->open('test.db');
            //$this->busyTimeout(5000);
            //$this->exec('PRAGMA journal_mode = wal;');
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

        function mission($SESSIONID, $POINTS, $MISSION){
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
            
            $smt = $this->prepare('UPDATE ACTIONS set ISCOMPLETE=1 WHERE MISSIONNUMBER=:mission AND TYPE="mission" AND PLAYERS_ID=:players_id');
            $smt->bindValue(':players_id', $players_id, SQLITE3_TEXT);
            $smt->bindValue(':mission', $MISSION, SQLITE3_TEXT);
            $ret = $smt->execute();
        }

        function missions($PLAYER_ID, $SESSIONID){
            $smt = $this->prepare('SELECT * from ACTIONS where PLAYERS_ID=:id');
            $smt->bindValue(':id', $PLAYER_ID, SQLITE3_TEXT);

            $ret = $smt->execute();
            while ($row = $ret->fetchArray(SQLITE3_ASSOC)) {
                if (($row['MISSIONNUMBER'] == 1) && ($row['ISCOMPLETE'] == 0)) {
                    $this->mission($SESSIONID, 50, 1);
                    break;
                } else {
                    if (($row['MISSIONNUMBER'] == 2) && ($row['ISCOMPLETE'] == 0) && ($this->flag_global == 1)) {
                        $this->mission($SESSIONID, 100, 2);
                        break;
                    }
                    if (($row['MISSIONNUMBER'] == 3) && ($row['ISCOMPLETE'] == 0) && ($this->flag_if == 1)) {
                        $this->mission($SESSIONID, 150, 3);
                        break;
                    }
                    if (($row['MISSIONNUMBER'] == 4) && ($row['ISCOMPLETE'] == 0) && ($this->flag_loop == 1)) {
                        $this->mission($SESSIONID, 200, 4);
                        break;
                    }
                    if (($row['MISSIONNUMBER'] == 5) && ($row['ISCOMPLETE'] == 0) && ($this->flag_subtrac == 1)) {
                        $this->mission($SESSIONID, 250, 5);
                        break;
                    }
                    if (($row['MISSIONNUMBER'] == 6) && ($row['ISCOMPLETE'] == 0) && ($this->flag_full == 1)) {
                        $this->mission($SESSIONID, 250, 6);
                        break;
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
            $this->missions($PLAYER_ID, $SESSIONID);
        }
    }
?>