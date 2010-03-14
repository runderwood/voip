#!/usr/bin/php -q
<?php

foreach (array('voip_api.inc') as $file) {
  require_once(dirname(__FILE__) . DIRECTORY_SEPARATOR . $file); 
}

$voip_server = 'http://localhost/d6/xmlrpc.php';

echo("about to call system.listMethods\n");
$result = xmlrpc($voip_server, 'system.listMethods');
echo('result: ' . print_r($result, TRUE) . "\n");

echo("about to call voip.hello\n");
$result = xmlrpc($voip_server, 'voip.hello', 'John');
echo('result: ' . print_r($result, TRUE) . "\n");


$request_id = 'invalid';
echo("about to call voip_process_request($request_id)\n");
$options = array('arg1' => '1', 'arg2' => 'blue');
$result = voip_process_request($voip_server, $request_id, $options);
echo('result: ' . print_r($result) . "\n");

$request_id = 'echo';
echo("about to call voip_process_request($request_id)\n");
$options = array('arg1' => '1', 'arg2' => 'blue');
$result = voip_process_request($voip_server, $request_id, $options);
echo('result: ' . print_r($result) . "\n");

$request_id = 'non existent';
echo("about to call voip_process_request($request_id)\n");
$options = array('arg1' => '1', 'arg2' => 'blue');
$result = voip_process_request($voip_server, $request_id, $options);
echo('result: ' . print_r($result) . "\n");

$request_id = 'test';
echo("about to call voip_process_request($request_id)\n");
$options = array('arg1' => '1', 'arg2' => 'blue');
$result = voip_process_request($voip_server, $request_id, $options);
echo('result: ' . print_r($result) . "\n");

/****
echo("about to call voip_send_request()\n");
$request_id = "seat!";
$options = array('time' => 'now', 'bark' => 'loud');
$result = voip_send_request($request_id, $options);
echo('result: ' . print_r($result) . "\n");
****/
?>
