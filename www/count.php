<?php
include("config.php");
require_once('dbMissions.php');
$dbM = New MissionsDB();

$qty_commands = array("move" => 0, "moveto" => 0, "yaw" => 0, "pitch" => 0, "roll" => 0, "if" => 0, "repeat" => 0, "while" => 0, "for" => 0, "box" => 0, "boxes" => 0, "dowel" => 0, "extrude" => 0, "revolve" => 0, "sphere" => 0, "spheres" => 0, "tube" => 0, "transform" => 0, ".radius" => 0, ".innerRadius" => 0, "outerRadius" => 0, ".rgb" => 0, "nsides" => 0, "pi" => 0);
$commands = array_keys($qty_commands);

$string = file_get_contents($outpath);
$json_a = json_decode($string, true);
$model_a = json_decode($json_a['model'], true);
$faces_a = $model_a['faces'];

//echo "id: " . $json_a['id'] . "\n";
//echo "source:{ \n" . $json_a['source'] . "\n}\n";
//var_dump($faces_a);

$lines = explode("\n", $json_a['source']);

$i = 0;
while ($i < sizeof($commands)):
    $j = 0;
    while ($j < strlen($json_a['source'])):
        $aux = substr($json_a['source'], $j, strlen($commands[$i]));
        if ($commands[$i] === $aux) {
            $qty_commands[$commands[$i]]++;
        }
        $j++;
    endwhile;
    $i++;
endwhile;

$variable_subtrac = array();
$k = 0;
while ($k < sizeof($lines)) {
    $retval = preg_match(('/([a-zA-Z]+)\ ?\=\ ?(dowel|box|boxes|sphere|spheres|revolve|extrude|tube|transform)/'), $lines[$k], $regs);
    if ($retval) {
        array_push($variable_subtrac, $regs[1]);
    }
    $k++;
}

$i = 0;
$flag_subtrac = 0;
while ($i < sizeof($variable_subtrac)) {
    $j = 0;
    while ($j < sizeof($variable_subtrac)) {
        if ($variable_subtrac[$i] !== $variable_subtrac[$j]) {
            $aux_reg = $variable_subtrac[$i] . "\ ?\-\ ?" . $variable_subtrac[$j];
            $retval = preg_match("/$aux_reg/", $json_a['source'], $regs_final);
            //print_r($regs_final);
            if ($retval) {
                $flag_subtrac=1;
            }
        }
        $j++;
    }
    $i++;
}

//print_r($qty_commands);
//echo "\n" . sizeof($faces_a) . "\n";

$flag_loop = 0;
$flag_if = 0;
$flag_global = 0;
$flag_full = 0;

if ($qty_commands["for"] > 0 || $qty_commands["repeat"] > 0 || $qty_commands["while"] > 0) {
    $flag_loop = 1;
}
if ($qty_commands["if"] > 0) {
    $flag_if = 1;
}
if ($qty_commands[".radius"] > 0 || $qty_commands[".innerRadius"] > 0 || $qty_commands["outerRadius"] > 0 || $qty_commands[".rgb"] > 0 || $qty_commands["nsides"] > 0) {
    $flag_global = 1;
}
if($flag_loop==1 && $flag_if==1 && $flag_global==1 && $flag_subtrac==1){
    $flag_full = 1;
}

$obj->id = $json_a['id'];
$obj->qty_commands = $qty_commands;
$obj->qty_faces = sizeof($faces_a);

$countpath = sprintf($ROOT . "saves/%s_count.json", $timestamp);
$json_b = json_encode($obj);
file_put_contents($countpath, $json_b);

$dbM->selectMissions($SESSIONID, $flag_loop, $flag_if, $flag_global, $flag_subtrac, $flag_full);

$dbM->close();

?>