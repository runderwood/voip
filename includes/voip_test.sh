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
echo("Testing services infrastructure\n");
echo("-------\n");

$services_server = 'http://localhost/d6/services/xmlrpc';

$request = 'system.getServices';
echo("about to call $request\n");
$result = xmlrpc($services_server, $request);
echo('result: ' . print_r($result, TRUE) . "\n\n");

$request = 'voip.echo';
$param = 'good times';
echo("about to call $request\n");
$result = xmlrpc($services_server, $request, $param);
$result = xmlrpc($services_server, 'voip.echo', 'good times');
echo('result: ' . print_r($result, TRUE) . "\n\n");

$request_id = 'invalid_request';
echo("about to call voip_process_request($request_id)\n");
$options = array('arg1' => '1', 'arg2' => 'blue');
$result = voip_process_request($services_server, $request_id, $options);
echo('voip_api_error: ' . print_r(voip_api_error_message(), TRUE) . "\n");
echo('result: ' . print_r($result, TRUE) . "\n\n");

$request_id = 'voip_get_script';
echo("about to call voip_process_request($request_id)\n");
$options['script_name'] = 'invalid script name';
$result = voip_process_request($services_server, $request_id, $options);
echo('voip_api_error: ' . print_r(voip_api_error_message(), TRUE) . "\n");
echo('result: ' . print_r($result, TRUE) . "\n\n");

$request_id = 'voip_get_script';
echo("about to call voip_process_request($request_id)\n");
$options['script_name'] = 'hello_world';
$result = voip_process_request($services_server, $request_id, $options);
echo('voip_api_error: ' . print_r(voip_api_error_message(), TRUE) . "\n");
echo('result: ' . print_r($result, TRUE) . "\n\n");

exit;


echo("\n");
echo("-------\n");
echo("Testing voip_process_request()\n");
echo("-------\n");

$request_id = 'invalid_request';
echo("about to call voip_process_request($request_id)\n");
$options = array('arg1' => '1', 'arg2' => 'blue');
$result = voip_process_request($voip_server, $request_id, $options);
echo('voip_api_error: ' . print_r(voip_api_error_message(), TRUE) . "\n");
echo('result: ' . print_r($result, TRUE) . "\n\n");

$request_id = 'voip_get_script';
echo("about to call voip_process_request($request_id)\n");
$options['script_name'] = 'invalid script name';
$result = voip_process_request($voip_server, $request_id, $options);
echo('voip_api_error: ' . print_r(voip_api_error_message(), TRUE) . "\n");
echo('result: ' . print_r($result, TRUE) . "\n\n");

$request_id = 'voip_get_script';
echo("about to call voip_process_request($request_id)\n");
$options['script_name'] = 'hello_world';
$result = voip_process_request($voip_server, $request_id, $options);
echo('voip_api_error: ' . print_r(voip_api_error_message(), TRUE) . "\n");
echo('result: ' . print_r($result, TRUE) . "\n\n");


$numbers = array('6174525549', '6177920995','7777777777', '123');
$request_id = 'voip_dial_out';
foreach($numbers as $number){
  echo("about to call voip_process_request($request_id) for $number\n");
  $options['number'] = $number;
  $options['script_name'] = 'hello_world';
  $variables['VD_XMLRPC_URL'] = $voip_server;
  $variables['VD_USER_NAME'] = 'test_user';
  $options['variables'] = $variables;
  $options['unique_id'] = uniqid();
  $result = voip_process_request($voip_server, $request_id, $options);
  echo('voip_api_error: ' . print_r(voip_api_error_message(), TRUE) . "\n");
  echo('result: ' . print_r($result, TRUE) . "\n\n");
}



echo("\n");
echo("-------\n");
echo("End of tests\n");
echo("-------\n");

?>
