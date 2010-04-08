#!/usr/bin/php -q
<?php

foreach (array('voip_asterisk.inc') as $file) {
  require_once(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'includes' . DIRECTORY_SEPARATOR . $file); 
}

require_once('../../includes/voip_error.inc');



/**
 * Global variables
 */

$ami_host = 'localhost';
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

$destination = '16172130536'; // Leo's Google Voice number
//$origin = '6177920995'; // Leo's cell phone
//$destination = '6176888319'; // Danielle's cell phone
//$origin = '16177920995';
$origin = '6174525510'; // media lab office

echo("about to call _voip_asterisk_dial_out($server_config, $origin, $destination)\n");
$result = _voip_asterisk_dial_out($server_config, $origin, $destination);
echo('voip_error: ' . voip_error_message() . "\n");
echo('result: ' . print_r($result, TRUE) . "\n\n");

echo("\n");
echo("-------\n");
echo("End of tests\n");
echo("-------\n");

?>
