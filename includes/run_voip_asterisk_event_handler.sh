#!/usr/bin/php -q
<?php

foreach (array('voip_api.inc') as $file) {
  require_once(dirname(__FILE__) . DIRECTORY_SEPARATOR . $file); 
}

$voip_server = 'http://localhost/d6/xmlrpc.php';
#$voip_server = 'http://localhost/voipdev/xmlrpc.php';
#$voip_server = 'http://whatsupserver.media.mit.edu/d6/xmlrpc.php';

echo("\n");
echo("-------\n");
echo("Testing xmlrpc infrastructure \n");
echo("-------\n");

echo("about to call system.listMethods\n");
$result = xmlrpc($voip_server, 'system.listMethods');
echo('result: ' . print_r($result, TRUE) . "\n");



echo("\n");
echo("-------\n");
echo("Testing Voip Asterisk event handler\n");
echo("-------\n");


echo("about to call voip_asterisk.runEventHandler\n");
$result = xmlrpc($voip_server, 'voip_asterisk.runEventHandler');
echo('result: ' . print_r($result, TRUE) . "\n");


echo("\n");
echo("-------\n");
echo("End of tests\n");
echo("-------\n");

?>
