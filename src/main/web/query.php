<?php	

include_once("inc/cbir.inc");
include_once("inc/xml_parser.inc");

$filename = get_convert_upload_image();
extract_features($filename);
$quantized_ret = quantize($filename);
$retrieved_ret = image_retrieval($quantized_ret, 100);
display_image($retrieved_ret);
print_r($retrieved_ret);


function display_image($images) {

    $base_url = "/~itv/thumbnails/";

    $image_urls = array();

    foreach ($images['results']['image'] as $image) {
        $image_id = $image['id'];
        $image_url = $base_url . $image_id . '_t.jpg';
        $image_urls[] = $image_url;
    }

    foreach ($image_urls as $image_url) {
        print "<img width=\"100\" height=\"100\" src=\"$image_url\" />";
    }

}

function image_retrieval($features, $max = 100) {

    $ret = '';

    $query_args = array(
                "url" => "http://cml10.csie.ntu.edu.tw:5000/",
                "post" => true,
                "params" => array(
                    "feature" => $features,
                    "max" => $max
                )
    );


    $xml_ret = query_cbir_service($query_args);
    if (!empty($xml_ret)) {
        $parser = new xml_parser();
        $ret = $parser->parse($xml_ret);
    }

    return $ret;
}

function quantize($filename) {

    $features = implode('', file($filename . '.hes'));

    $query_args = array(
                "url" => "http://cml11.csie.ntu.edu.tw:5001/",
                "post" => true,
                "params" => array(
                    "feature" => $features
                )
    );


    $quantized_ret = query_cbir_service($query_args);
    #print_r($quantized_ret);
    return $quantized_ret;

}


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
