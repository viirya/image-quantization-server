<?php
//phpinfo();

//xxxsample();
 
function xxxsample() {
 
  $xmlstg =<<<EOF
<?xml version="1.0" encoding="UTF-8"?>
<config>
<filetype>xls</filetype>
</config>
EOF;

  $parser = new xml_parser();
  $ret = $parser->parse($xmlstg);
  print_r($ret);
 
  echo "encoding: ".$parser->get_encoding()."\n";
 
}
 
class xml_parser {
 
   var $dom;
   var $root;
   var $ret;
   var $encoding;
 
   function convert_attrs($att) {
 
     $ret = array();
 
     foreach($att as $i)
       $ret[$i->name] = $i->value;
 
     return $ret;
 
   }
   function parse_node($node) {
       $ret = '';
       $node_name = $node->nodeName;
       if ($node->hasChildNodes()) {
           foreach($node->childNodes as $n) {
               if ($n->nodeName == '#text' || $n->nodeName == '#cdata-section') {
                   if (!is_array($ret)) {
                     $ret = $n->nodeValue;
                   }
                   $node_value = $n->nodeValue;
               }
               else {
                   if (isset($ret) && !is_array($ret))
                     $ret = array();
                   $tmp = $this->parse_node($n);
                   $attrs = $n->attributes;
                   //if (is_array($attrs) && !empty($attrs)) {
                   if ($attrs != NULL) {
                     $attrs = $this->convert_attrs($attrs);
                     if (!empty($attrs)) {
                       $tmp2 = $tmp;
                       $tmp = array();
                       $tmp['value'] = $tmp2;
                       $tmp['_attrs'] = $attrs;
                     }
                   }
                   if (!isset($ret[$n->nodeName])) {
                     $ret[$n->nodeName] = $tmp;
                   }
                   else {
                     if (is_array($ret[$n->nodeName]) && !isset($ret[$n->nodeName][0])) {
                       $switch = $ret[$n->nodeName];
                       $ret[$n->nodeName] = array();
                       $ret[$n->nodeName][0] = $switch;
                     }
                     else if (!is_array($ret[$n->nodeName]) && isset($ret[$n->nodeName])) {
                       $switch = $ret[$n->nodeName];
                       $ret[$n->nodeName] = array();
                       $ret[$n->nodeName][0] = $switch;
                     }
                     $ret[$n->nodeName][] = $tmp;
                   }
               }
           }
       }
       return $ret;
   }
 
   function parse($xml) {

     //$this->dom = domxml_open_mem($xml, DOMXML_LOAD_RECOVERING);
     $this->dom = new DOMDocument();
     $this->dom->loadXML($xml);
     $this->ret = array();

     if (!isset($this->dom) || $this->dom == false)
       return $this->ret;
 
     //$this->root = $this->dom->document_element();
     $this->root = $this->dom->documentElement;
     $this->ret[$this->root->nodeName] = $this->parse_node($this->root);
 
     $this->encoding = '';
     $matches = array();
     if (preg_match('/<\?xml.*encoding="(.*?)"\?>/', $xml, $matches))
       $this->encoding = $matches[1];
 

     return $this->ret;
   }
 
   function get_encoding() {
     return $this->encoding;
   }
 

}

?>

