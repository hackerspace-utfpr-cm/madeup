<?php

error_reporting(E_ALL);

function to_bytes($val) {
  $val = trim($val);
  $last = strtolower($val[strlen($val) - 1]);
  $val_bytes = intval(substr($val , 0, strlen($val) - 1));

  switch($last) {
    case 'g':
      $val_bytes *= 1024;
    case 'm':
      $val_bytes *= 1024;
    case 'k':
      $val_bytes *= 1024;
  }

  return $val_bytes;
}

if (isset($_REQUEST['source'])) {
  $in = $_REQUEST;
} else {
  $json = file_get_contents("php://input");
  $in = json_decode($json, true);
}

// $in is a dictionary we get from the client with the following allowable 
// key-value pairs:
//   source: The Madeup source code to interpret.
//   shading_mode: One of the shading modes recognized by the interpreter.
//   geometry_mode: One of the geometry modes recognized by the interpreter.
//   extension: One of json, stl, or obj.
//   tag: The name of the model used for exporting.
//   timestamp: Time request submitted.

// $out is a dictionary we send back to the client with the following 
// key-value pairs:
//   exit_status: The exit value of the interpreter.
//   stdout: The interpreter's stdout.
//   model: The JSON representation of the model if in.extension is json,
//      otherwise undefined.
//   timestamp: Time request submitted.
$out = array();
$out['timestamp'] = $in['timestamp'];

// The interpreter checks that shading_mode and geometry_mode are legal.
// But we should check that the output path is legal -- that the extension
// we are given hasn't been tampered with.
if (strcmp($in['extension'], 'json') != 0 &&
    strcmp($in['extension'], 'obj') != 0 &&
    strcmp($in['extension'], 'stl') != 0) {
  $out['exit_status'] = 1;  
  $out['stdout'] = "I don't know {$in['extension']}. I only know how to output JSON and OBJ.";
  echo json_encode($out);
} else {
  // The interpreter will write the geometry out to a file when it finishes 
  // interpreting. Let's make that file a temporary one.
  $out_path = tempnam(sys_get_temp_dir(), 'mupout');

  // The client may be requesting json output (for Three.JS) or obj output. The 
  // interpreter determines what kind of output to give based on the output 
  // file's extension, which we have to append manually.
  rename($out_path, $out_path . '.' . $in['extension']);
  $out_path = "$out_path.{$in['extension']}";

  // Write uploaded Madeup source to a file, because that's how the interpreter 
  // expects to get input.
  $in_path = tempnam(sys_get_temp_dir(), 'mupin');
  file_put_contents($in_path, $in['source']);

  error_log("Madeup source: {$in['source']}");
  error_log("Input file: $in_path");
  error_log("Output file: $out_path");

  if (preg_match('/^--\s*timeout\s*=\s*(\d+)/m', $in['source'], $timeout_matches)) {
    $timeout = $timeout_matches[1]; 
    if ($timeout < 1) {
      $timeout = 15;
    }
  } else {
    $timeout = 15;
  }

  error_log("Timeout: $timeout");
  $forn_timeout = $timeout + 5;

  // Execute the interpreter and collect its output. Forn requires 
  // Process.spawn, which isn't available in older Rubies.
  $lines = array();
  /* $command = sprintf("./merp --timeout $timeout --shading %s -o %s --geometry %s %s 2>&1", */
  $command = sprintf("bash --login -c \"rvm use 2.1 2>/dev/null >/dev/null; ./forn $forn_timeout ./merp --timeout $timeout --shading %s -o %s --geometry %s %s\" 2>&1",
                     escapeshellarg($in['shading_mode']),
                     $out_path,
                     escapeshellarg($in['geometry_mode']),
                     $in_path);
  exec($command, $lines, $out['exit_status']);
  $out['stdout'] = implode("\n", $lines);
  $out['geometry_mode'] = $in['geometry_mode'];

  $lines = array();
  $command = sprintf("./merp --tree -q %s", $in_path);
  exec($command, $lines, $tree_status);
  $out['tree'] = implode("\n", $lines);

  if (($in['extension'] === 'obj' || $in['extension'] === 'stl') && $out['exit_status'] == 0) {
    // We generate a file named after the model. But let's play it
    // safe and remove all the alphanumeric junk they try to feed
    // us.
    $tag = preg_replace("/\W/", '', $in['tag']);

    // If nothing's left, we default to madeup.extension.
    if (strcmp($tag, '') == 0) {
      $tag = 'madeup';
    }

    header($_SERVER["SERVER_PROTOCOL"] . " 200 OK");
    header("Cache-Control: public"); // needed for IE
    header("Content-Type: application/x-please-download-me");
    header("Content-Transfer-Encoding: Binary");
    header("Content-Length:" . filesize($out_path));
    header("Content-Disposition: attachment; filename=$tag.{$in['extension']}");
    readfile($out_path);
  } else {
    if (memory_get_usage() + 5 * filesize($out_path) > to_bytes(ini_get('memory_limit'))) {
      $out['exit_status'] = 1;  
      $out['stdout'] = "Whoa! I don't have enough memory for this.";
      $out['model'] = '';
    } else if ($out['exit_status'] == 0) {
      $out['model'] = file_get_contents($out_path);
    }
    $json = json_encode($out);
    echo $json;
  }

  // Clean up the temporary files we created for the interpreter.
  if (file_exists($out_path)) {
    $ok = unlink($out_path);
  }

  if (file_exists($in_path)) {
    $ok = unlink($in_path);
  }
}

error_log("Results: " . json_encode($out));
?>
