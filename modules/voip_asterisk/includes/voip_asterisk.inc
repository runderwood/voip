<?php
// $Id$

/**
 * @file
 * Internal functions that handle interactions with the Asterisk server
 */

// TODO: connect via SSL
// TODO: define max number of simultaneous connections



foreach (array('phpagi-asmanager.php') as $file) {
  require_once(dirname(__FILE__) . DIRECTORY_SEPARATOR . $file); 
}


/**
 * Global variables
 */

$astManager = new AGI_AsteriskManager(NULL, array('log_method' =>  '_voip_asterisk_ami_log'));


/**
 * Internal functions
 */

/**
 * Define how AMI errors should be reported
 */
function _voip_asterisk_ami_log($message, $level=1)
{
  voip_error($message);
}

/**
 * Test the connection with the Asterisk server
 */
function _voip_asterisk_ping($server_config) {
  global $astManager;

  $ami_port = $server_config['ami_port'];
  $ami_host = $server_config['ami_host'];
  $ami_host = ($ami_port)? $ami_host.':'.$ami_port : $ami_host;
  $ami_user = $server_config['ami_user'];
  $ami_pass = $server_config['ami_password'];

  voip_error_reset();

  $result = $astManager->connect($ami_host, $ami_user, $ami_pass);
  if(voip_error()) {
    voip_error("Connection to Asterisk manager failed: " . voip_error_message());
    return FALSE;
  }

  $result = $astManager->Ping();
  if($result['Response'] != 'Success'){
    voip_error("AMI command Ping failed: " . print_r($result, TRUE));
    return FALSE;
  }

  $astManager->disconnect();
  return TRUE;
}

/**
 * Establish a new call connecting origin and destination numbers
 *
 */
function _voip_asterisk_dial_out($server_config, $origin, $destination) {
  global $astManager;

  $ami_port = $server_config['ami_port'];
  $ami_host = $server_config['ami_host'];
  $ami_host = ($ami_port)? $ami_host.':'.$ami_port : $ami_host;
  $ami_user = $server_config['ami_user'];
  $ami_pass = $server_config['ami_password'];

  voip_error_reset();

  $result = $astManager->connect($ami_host, $ami_user, $ami_pass);
  if(voip_error()) {
    voip_error("Connection to Asterisk manager failed: " . voip_error_message());
    return FALSE;
  }

  $channel_string = $server_config['pstn_channel_string'];
  $channel = str_replace("%number", $origin, $channel_string); // Channel from which to originate the call
/*********************
*****************/
$exten = $destination; // Extension to use on connect (must use Context & Priority with it) 
$context = 'voip_drupal_bridge'; // Context to use on connect (must use Exten & Priority with it) 
$priority = 1; // Priority to use on connect (must use Context & Exten with it) 
  $application = NULL; // Application to use on connect (use Data for parameters) 
  $application_data = NULL; // Data if Application parameter is used 
  $timeout = 30000; // Timeout (in milliseconds) for the originating connection to happen
                    // (defaults to 30000 milliseconds) 
  $cid = $server_config['system_caller_id']?$server_config['system_caller_id']:NULL;
  $caller_id = $cid;  // CallerID to be used for both ends of the call.  Note that some
                      // voicemail systems might be automatically activated if the
                      // caller id is the same as the number being dialed.
                      // TODO: Come up with a mechanism to set appropriate caller ids
                      // for each end of the call.  This might require the usage of special
                      // channel variables to be passed to the "voip drupal bridge" context
  $variable = NULL; // Channel variables to set (max 32). Variables will be set for both
                    // channels (local and connected). Note that the syntax for Asterisk 1.4+
                    // and 1.6+ are different from the previous versions 
  $account = NULL; // Account code for the call 
  $async = 0; // Use 1 for the origination to be asynchronous (allows multiple calls to be
              // generated without waiting for a response). Using Async leads to an
              // OriginateResponse event which contains the failure reason if any. Reason
              // may be one of the following: 0 = no such extension or number; 1 = no answer;
              // 4 = answered; and 8 = congested or not available 
  $action_id = NULL; // The request identifier. It allows you to identify the response to
                     // this request. You may use a number or a string. Useful when you make
                     // several simultaneous requests. 

echo("about to call Originate(
       channel: $channel, exten: $exten, context: $context,
       priority: $priority, application: $application, data: $application_data,
       timeout: $timeout, callerid: $caller_id, variable: $variable, account: $account, async: $async, action_id: $action_id)\n");
  $result = $astManager->Originate(
     $channel, $exten, $context, $priority, $application, $application_data,
     $timeout, $caller_id, $variable, $account, $async, $action_id);
echo('result: ' . print_r($result) . "\n");
  if($result['Response'] != 'Success'){
    voip_error("AMI command Originate failed: " . print_r($result, TRUE));
    return FALSE;
  }

  $astManager->disconnect();
  return TRUE;
}

