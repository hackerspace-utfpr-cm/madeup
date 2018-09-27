<?php
$json = file_get_contents("php://input");
$in = json_decode($json, true);

date_default_timezone_set('America/Sao_Paulo');
$timestamp = date('d_m_Y_H_i_s');
$id = $in['id'];
$source = $in['source'];
#$lesson = $in['lesson'];

$outpath = sprintf("/var/www/madeup/saves/%s_%s_snap", $timestamp, $id);
//file_put_contents($outpath, $source);
//echo "Arquivo escrito em $outpath.";
?>
