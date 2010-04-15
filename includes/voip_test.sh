#!/usr/bin/php -q
<?php

foreach (array('voip_api.inc') as $file) {
  require_once(dirname(__FILE__) . DIRECTORY_SEPARATOR . $file); 
}

$voip_server = 'http://localhost/d6/xmlrpc.php';
#$voip_server = 'http://localhost/voipdev/xmlrpc.php';

echo("\n");
echo("-------\n");
echo("Testing xmlrpc infrastructure \n");
echo("-------\n");

echo("about to call system.listMethods\n");
$result = xmlrpc($voip_server, 'system.listMethods');
echo('result: ' . print_r($result, TRUE) . "\n");



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

$request_id = 'voip_dial_out';
echo("about to call voip_process_request($request_id)\n");
$options['number'] = '6177920995'; // leo's cell number
$options['script_name'] = 'hello_world';
$variables['VD_XMLRPC_URL'] = 'http://localhost/drupal6/xmlrpc.php';
$variables['VD_USER_NAME'] = 'test_user';
$options['variables'] = $variables;
$options['unique_id'] = uniqid();
$result = voip_process_request($voip_server, $request_id, $options);
echo('voip_api_error: ' . print_r(voip_api_error_message(), TRUE) . "\n");
echo('result: ' . print_r($result, TRUE) . "\n\n");



echo("\n");
echo("-------\n");
echo("End of tests\n");
echo("-------\n");

?>
