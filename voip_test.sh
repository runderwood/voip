#!/usr/bin/php -q
<?php

foreach (array('voip_api.inc') as $file) {
  require_once($file);
}

$voip_server = 'http://localhost/drupal6/xmlrpc.php';

echo("about to call system.listMethods\n");
$result = xmlrpc($voip_server, 'system.listMethods');
echo('result: ' . print_r($result, TRUE) . "\n");

echo("about to call voip.hello\n");
$result = xmlrpc($voip_server, 'voip.hello', 'John');
echo('result: ' . print_r($result, TRUE) . "\n");


echo("about to call Voip Drupal process inbound requests\n");
$request['request_id'] = 'test';
$result = voip_process_inbound_request($voip_server, $request);
echo('result: ' . print_r($result, TRUE) . "\n");

?>
