<?php	

$filename = get_convert_upload_image();
extract_features($filename);


function get_convert_upload_image() {

    $query_file = '';

    if ($_FILES["userfile"]["size"] != 0) {
	    $query_file = $_FILES["userfile"]["tmp_name"];
    	$tmp = explode( '/', $query_file );
    	move_uploaded_file($query_file, 'upload/' . $tmp[count($tmp) - 1]);
        $query_file = 'upload/' . $tmp[count($tmp) - 1];
    }
 
    $queryimage_size = getimagesize($query_file);

    if ($queryimage_size[0] * $queryimage_size[1] > 360000 ) {	
        $ratio = sqrt( 360000 / ($queryimage_size[0] * $queryimage_size[1]) );
    	$scaled_width = $queryimage_size[0] * $ratio;
    	$scaled_height = $queryimage_size[1] * $ratio;
    	exec('convert -resize ' . $scaled_width . 'x' . $scaled_height . ' ' . $query_file . $query_file);
    }

    $query_filename = explode(".", $query_file);
    if (count($query_filename) >= 2)
        unset($query_filename[count($query_filename) - 1]);
    $pgm_file = join(".", $query_filename);
    $convert_pgm = 'convert ' . $query_file . ' ' . $pgm_file . '.pgm';
    $ret = shell_exec($convert_pgm);

    return $pgm_file; 
}

function extract_features($filename) {

    exec('./bin/extract_features_64bit.ln -hesaff -sift -i ' . $filename . '.pgm -o1 ' . $filename . '.hes');

}

?>
