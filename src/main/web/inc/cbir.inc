<?php

function merge_query_result($arg) {

  $ret = array();

  $flickr_photos = $arg['results'];
  $weights = $arg['weights'];

  $ret['data'] = array();

  $weight_count = 0;
  foreach ($flickr_photos as $search_type => $photos) {
    //if (!isset($photos['results']['query']['index']))
    //  continue;

    $query_index = -1;
    if (isset($photos['results']['query']['index'])) {
      $query_index = $photos['results']['query']['index'];
    }

    $items = $photos['results']['item'];
    $mean = 0;
    $std = 0;
    $count = 0; 

    foreach ($items as $key => $item) {
      if ($item['index'] == $query_index || ($key == 0 && $query_index == -1 && $item['distance'] < 0.0001))
        continue;

      $mean += $item['distance'];
      $count++;
    }

    if ($count > 0)
      $mean /= $count;
    
    foreach ($items as $key => $item) {
      if ($item['index'] == $query_index || ($key == 0 && $query_index == -1 && $item['distance'] < 0.0001))
        continue;

      $std = $std + pow(($item['distance'] - $mean), 2);
    }

    if ($count > 0) {
      $std /= $count;
      $std = sqrt($std);
    }

    foreach ($items as $key => $item) {
      if ($item['index'] == $query_index || ($key == 0 && $query_index == -1 && $item['distance'] < 0.0001))
        continue;

      $photo_id = $item['id'];
      if(preg_match_all("/(.*)\.jpg/", $photo_id, $matches))      
        $photo_id = $matches[1][0];

      if ($photo_id == '') // || $photo_id == $item['id'])
        continue;

      $index = $item['index'];
      $distance = $item['distance'];

      //if (isset($ret['data'][$photo_id]) && $index != $ret['data'][$photo_id]['index'])
      //  continue;

//      if (isset($ret['data'][$photo_id])) {
//        print "$photo_id\n";
//      }

      $ret['data'][$photo_id]['score'] += (1 - (1 / (1 + exp(-(($distance - $mean) / $std))))) * $weights[$weight_count];
      $ret['data'][$photo_id]['id'] = $photo_id; //$item['id'];
      $ret['data'][$photo_id]['index'] = $index;
    }

    $weight_count++;

  } 

  usort($ret['data'], "cmp");


  return $ret;

}

function cmp($a, $b) {
  if ($a['score'] == $b['score']) {
    return 0;
  }
  //return ($a < $b) ? -1 : 1;
  return ($a['score'] < $b['score']) ? 1 : -1;
}

function query($args) {

  $k = 10;
  if (isset($args['k']))
    $k = $args['k'];

  $query_id = '';
  if (isset($args['query_id']))
    $query_id = $args['query_id'];

  $flickr_id = '';
  if (isset($args['flickr_id']))
    $flickr_id = $args['flickr_id'];

  $feature = '';
  if (isset($args['feature']))
    $feature = $args['feature'];

  $url = "http://cml5.csie.ntu.edu.tw:5000/form_handler";
  if (isset($args['url']))
    $url = $args['url'];

  $post_fix = ".jpg";
  if (isset($args['postfix']))
    $post_fix = $args['postfix'];

  $query_args = array(
               "url" => $url,
               "params" => array(
                  "k" => $k,
                  "query_id" => $query_id,
                  "flickr_id" => $flickr_id,
                  "postfix" => $post_fix,
                  "feature" => $feature
                 )
               );

  if (isset($args['sample']) && $args['sample'] == 'yes') 
    $query_args['params']['sample'] = 'yes';

  //print_r($query_args);

  $xml = query_cbir_service($query_args);
  $parser = new xml_parser();
  $ret = $parser->parse($xml);
  //print_r($ret);

  return $ret;

}

function query_cbir_service($args) {

  $post_args = '';

  $count = 0;

  foreach ($args['params'] as $key => $value) {
    if ($count++ > 0)
      $post_args = $post_args . '&'; 
    $post_args .= $key . "=" . $value;
  }

  #print_r($args);
  #print($post_args);

  // create a new cURL resource
  $ch = curl_init();

  // set URL and other appropriate options
  if (isset($args['post'])) 
    curl_setopt($ch, CURLOPT_URL, $args['url']);
  else
    curl_setopt($ch, CURLOPT_URL, $args['url'] . '?' . $post_args);    
  curl_setopt($ch, CURLOPT_HEADER, 0);
  curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
  curl_setopt($ch, CURLOPT_POSTFIELDS, $post_args);
  curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
  curl_setopt($ch, CURLOPT_FOLLOWLOCATION, 1);
  if (isset($args['post'])) {
    curl_setopt($ch, CURLOPT_POST, 1); 
    curl_setopt($ch, CURLOPT_POSTFIELDS, $post_args);
  }

  // grab URL and pass it to the browser
  $ret = curl_exec($ch);

  //print_r($output);

  // close cURL resource, and free up system resources
  curl_close($ch);

  return $ret;

}

?>
