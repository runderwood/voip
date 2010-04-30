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
 * Internal functions
 */

/**
 * Return current instance of the Asterisk Manager. 
 * Note: This function was written to prevent the creation of a
 *       $voip_asterisk_manager global variable
 * TODO: Is this the best way to handle variables such as this one?
 */
function _voip_asterisk_manager()
{
  // by default, initialize the manager with the voip_asterisk.module logger
  static $manager = NULL;
  if(is_null($manager)) {
    $manager = new AGI_AsteriskManager(NULL, array('log_method' => '_voip_asterisk_ami_log'));
  }

  return $manager;
}


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

  $ami_port = $server_config['ami_port'];
  $ami_host = $server_config['ami_host'];
  $ami_host = ($ami_port)? $ami_host.':'.$ami_port : $ami_host;
  $ami_user = $server_config['ami_user'];
  $ami_pass = $server_config['ami_password'];

  voip_error_reset();

  $result = _voip_asterisk_manager()->connect($ami_host, $ami_user, $ami_pass);
  if(voip_error()) {
    voip_error("Connection to Asterisk manager failed: " . voip_error_message());
    return FALSE;
  }

  $result = _voip_asterisk_manager()->Ping();
  if($result['Response'] != 'Success'){
    voip_error("AMI command Ping failed: " . print_r($result, TRUE));
    return FALSE;
  }

  _voip_asterisk_manager()->disconnect();
  return TRUE;
}

/*
 * Keeps track of the "dial out' requests
 */
function voip_asterisk_dial_manager($action='reset', $id=NULL, $status=NULL) {
  static $_queue = array();

  switch($action){
    default:
      return NULL;
    case 'reset':
      $_queue = array();
      break;
    case 'set':
      $_queue[$id] = $status;
      break;
    case 'delete':
      unset($_queue[$id]);
      break;
    case 'get':
      $status = isset($_queue[$id])?$_queue['id']:NULL;
      return $status;
      break;
    case 'dump':
      return $_queue;
      break;
  }
}


/**
 * Callback function that handles OriginateResponse events
_voip_asterisk_event_originate(originateresponse, Array
(
    [Event] => OriginateResponse
    [Privilege] => call,all
    [ActionID] => 4bc53a5dd850c
    [Response] => Failure
    [Channel] => Local/6177920995@vd-dial_out
    [Context] => vd-bridge
    [Exten] => s
    [Reason] => 1
    [Uniqueid] => <null>
    [CallerIDNum] => 123456789
    [CallerIDName] => Voip Drupal
)
reason can be any of:
0 = no such extension or number
1 = no answer
4 = answered
8 = congested or not available 
 */
function _voip_asterisk_event_originate($ecode, $data, $server, $port) {
  $rc = TRUE;
  $reason = $data['Reason'];
  $id = $data['ActionID'];
  switch($reason){
    default:
      $status = 'unknown';
      $rc = FALSE;
      break;
    case 0:
      $status = 'invalid number';
      break;
    case 1:
      $status = 'no answer';
      break;
    case 4:
      $status = 'answered';
      break;
    case 8:
      $status = 'busy';
      break;
  }
  voip_asterisk_dial_manager('set', $id, $status);

  return $rc;
}

/**
 * Establish a new call by dialing the given number and connecting it to the given script
 *
 *  @param array $options is an array containing, among other things, 'variables' to be set in the new channels and a 'unique_id' to be associated with call being made.
 */
function _voip_asterisk_dial_out($server_config, $number, $script_name, $options) {

  $ami_port = $server_config['ami_port'];
  $ami_host = $server_config['ami_host'];
  $ami_host = ($ami_port)? ($ami_host.':'.$ami_port) : $ami_host;
  $ami_user = $server_config['ami_user'];
  $ami_pass = $server_config['ami_password'];

  voip_error_reset();

  $result = _voip_asterisk_manager()->connect($ami_host, $ami_user, $ami_pass);
  if(voip_error()) {
//    voip_error("Connection to Asterisk manager failed: " . voip_error_message());
    voip_error("Connection to Asterisk manager failed: " . voip_error_message() . "with server_config: ".print_r($server_config, TRUE). "and with the following params: connect($ami_host, $ami_user, $ami_pass)");
    return FALSE;
  }

  // add event handler to handle AMI Originate responses
  // (http://www.voip-info.org/wiki/view/Asterisk+Manager+API+Action+Originate)
  _voip_asterisk_manager()->add_event_handler('OriginateResponse','_voip_asterisk_event_originate');

  // The following channel string should match the one in voip_drupal.conf
//  $channel_string = 'Local/%number@vd-dial_out/n'; // This is the goal.  But it's not working...
  $channel_string = 'Local/%number@vd-dial_out'; // This seems to be working... (note the lack of /n at the end
  $channel = str_replace("%number", $number, $channel_string); // Channel from which to originate the call

  $exten = 's'; // Extension to use on connect (must use Context & Priority with it) 
  $context = 'vd-bridge'; // Context to use on connect (must use Exten & Priority with it) 
  $priority = 1; // Priority to use on connect (must use Context & Exten with it) 
  $application = NULL; // Application to use on connect (use Data for parameters) 
  $application_data = NULL; // Data if Application parameter is used 

  $timeout = 30000; // Timeout (in milliseconds) for the originating connection to happen
                    // (defaults to 30000 milliseconds) 

  $cid = $server_config['system_caller_id']?$server_config['system_caller_id']:NULL;
  $caller_id = $cid;  // CallerID to be used for both ends of the call.  Note that some
                      // voicemail systems might be automatically activated if the
                      // caller id is the same as the number being dialed.

  $variable = NULL; // Channel variables to set (max 32). Variables will be set for both
                    // channels (local and connected). Note that the syntax for Asterisk 1.4+
                    // and 1.6+ are different from the previous versions 
  $variables = $options['variables']?$options['variables']:array();
  $variables['VD_SCRIPT_NAME'] = $script_name;
  $numOfItems = count($variables);
  $counter = 0;
  foreach($variables as $name => $value){
    $counter += 1;
    if ($counter <> $numOfItems){
      $variable .= $name . '=' . $value . "\r\nVariable: ";
    } else {
      $variable .= $name . '=' . $value;
    }
  } 

  $account = NULL; // Account code for the call 

  $async = 1; 
//  $async = 0; // Use 1 for the origination to be asynchronous (allows multiple calls to be
              // generated without waiting for a response). Using Async leads to an
              // OriginateResponse event which contains the failure reason if any. Reason
              // may be one of the following: 0 = no such extension or number; 1 = no answer;
              // 4 = answered; and 8 = congested or not available 

  $action_id = NULL; // The request identifier. It allows you to identify the response to
                     // this request. You may use a number or a string. Useful when you make
                     // several simultaneous requests. 
  $action_id = $options['unique_id']?$options['unique_id']:NULL;

  $result = _voip_asterisk_manager()->Originate(
     $channel, $exten, $context, $priority, $application, $application_data,
     $timeout, $caller_id, $variable, $account, $async, $action_id);

  // keep track of the dial attempt
  if($action_id){
    voip_asterisk_dial_manager('set', $action_id, 'processing');
  }

  if($result['Response'] != 'Success'){
    voip_error("AMI command Originate failed: " . print_r($result, TRUE));
    return FALSE;
  }

// wait a little bit for the events to arrive before disconnecting
for($i=0;$i<30;$i++){
    sleep(1);
}

  _voip_asterisk_manager()->disconnect();
  return TRUE;
}

