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
//$ami_host = 'whatsupserver.media.mit.edu';
$ami_port = '5038';
$ami_user = 'voip_drupal_ami';
$ami_pass = 'vd123';
// $ami_pass = 'XXXX';
$server_config = array(
  'ami_host' => $ami_host,
  'ami_port' => $ami_port,
  'ami_user' => $ami_user,
  'ami_password' => $ami_pass,
);

echo("\n");
echo("-------\n");
echo("Testing PHP AMI infrastructure \n");
echo("-------\n");


echo("about to call _voip_asterisk_ping($server_config)\n");
$result = _voip_asterisk_ping($server_config);
echo('voip_error: ' . voip_error_message() . "\n");
echo('result: ' . print_r($result, TRUE) . "\n\n");


$event_handler = '_voip_asterisk_event_handler';
echo("about to call _voip_asterisk_process_events($server_config, $event_handler)\n");
$result = _voip_asterisk_process_events($server_config, $event_handler);
echo('voip_error: ' . voip_error_message() . "\n");
echo('result: ' . print_r($result, TRUE) . "\n\n");


echo("\n");
echo("-------\n");
echo("End of tests\n");
echo("-------\n");

?>
