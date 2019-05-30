<?php 
$image_name = 'saves/' . $_POST['name']  . '.png';
if( isset($_FILES['image']) and !$_FILES['image']['error'] ){
  file_put_contents( $image_name, file_get_contents($_FILES['image']['tmp_name']) );
}
?>