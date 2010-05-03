#!/usr/bin/php -q
<?php

foreach (array('voip_asterisk.inc') as $file) {
  require_once(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'includes' . DIRECTORY_SEPARATOR . $file); 
}

require_once('../../includes/voip_error.inc');



/**
 * Global variables
 */

//$ami_host = 'localhost';
$ami_host = 'whatsupserver.media.mit.edu';
$ami_port = '5038';
$ami_user = 'voip_drupal_ami';
$ami_pass = 'vd123';
// $ami_pass = 'XXXX';
$server_config = array(
  'ami_host' => $ami_host,
  'ami_port' => $ami_port,
  'ami_user' => $ami_user,
  'ami_password' => $ami_pass,
  'pstn_channel_string' => 'SIP/%number@outbound',
  'system_caller_id' => 'Voip Drupal <123456789>',
);

echo("\n");
echo("-------\n");
echo("Testing PHP AMI infrastructure \n");
echo("-------\n");


echo("about to call _voip_asterisk_ping($server_config)\n");
$result = _voip_asterisk_ping($server_config);
echo('voip_error: ' . voip_error_message() . "\n");
echo('result: ' . print_r($result, TRUE) . "\n\n");


//$number = '16172130536'; // Leo's Google Voice number
//$number = '16174890192';
$number = '6177920995'; // Leo's cell phone
//$number = '2674962399'; // Bryan's cell phone
//$number = '6171231234'; // invalid number
//$number = '6176888319'; // Danielle's cell phone
//$number = '6174525510'; // media lab office
//$number = 'gizmo17476461031'; // leo's gizmo account
$script_name = 'hello_world';
$variables['VD_XMLRPC_URL'] = 'http://localhost/drupal6/xmlrpc.php';
$variables['VD_USER_NAME'] = 'test_user';
$options['variables'] = $variables;
$options['unique_id'] = uniqid();

echo("about to call _voip_asterisk_dial_out($server_config, $number, $script_name, $options)\n");
$result = _voip_asterisk_dial_out($server_config, $number, $script_name, $options);
echo('voip_error: ' . voip_error_message() . "\n");
echo('result: ' . print_r($result, TRUE) . "\n\n");

// process return value from AMI Originate
// TODO: status reporting is dependent on active connection to AMI server... how to do that?
$q = voip_asterisk_dial_manager('dump');
echo("queue: " . print_r($q,TRUE) . "\n");



echo("\n");
echo("-------\n");
echo("End of tests\n");
echo("-------\n");

?>
