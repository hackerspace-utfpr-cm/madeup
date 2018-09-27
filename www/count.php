<?php	
	$qty_commands = array("move" => 0,"moveto" => 0,"yaw" => 0,"pitch" => 0,"roll" => 0,"if" => 0,"repeat" => 0,"while" => 0,"for" => 0,"box" => 0,"boxes" => 0,"dowel" => 0,"extrude" => 0,"revolve" => 0,"sphere" => 0,"spheres" => 0,"tube" => 0,"transform" => 0,".radius" => 0,".innerRadius" => 0,"outerRadius" => 0,".rgb" => 0,"nsides" => 0,"pi" => 0);
	$commands = array_keys($qty_commands);
	
	$string = file_get_contents($outpath);
	$json_a = json_decode($string, true);
  	$model_a = json_decode($json_a['model'], true);
  	$faces_a = $model_a['faces'];

	//echo "id: " . $json_a['id'] . "\n";
	//echo "source:{ \n" . $json_a['source'] . "\n}\n";
	//var_dump($faces_a);
	
	$i = 0;
	while($i < sizeof($commands)):
		$j = 0;
		while	($j < strlen($json_a['source'])):
			$aux = substr($json_a['source'], $j, strlen($commands[$i]));
			if($commands[$i] === $aux){
				$qty_commands[$commands[$i]]++;
			}
			$j++;
		endwhile;
		$i++;
	endwhile;
	
	//print_r($qty_commands);
	//echo "\n" . sizeof($faces_a) . "\n";

	$obj->id =  $json_a['id'];
	$obj->qty_commands =  $qty_commands;
	$obj->qty_faces =  sizeof($faces_a);

	$countpath = sprintf("/var/www/madeup/saves/%s_count.json",$timestamp);
	$json_b = json_encode($obj);
    file_put_contents($countpath, $json_b);
?>