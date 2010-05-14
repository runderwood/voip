<?php
// $Id$

/**
 * @file
 * Implementation of a simple log server for Voip Drupal
 */

foreach (array('twilio.php') as $file) {
  require_once(dirname(__FILE__) . DIRECTORY_SEPARATOR . $file); 
}


/**
 * Internal functions
 */

/**
 * Test the connection with the Twilio server
 */
function _voiptwilio_ping($server_config) {

  // Twilio configuration
  $api_version = '2008-08-01';
  $account_sid = $server_config['voiptwilio_account_sid'];
  $auth_token = $server_config['voiptwilio_auth_token'];
    
  // Instantiate a new Twilio Rest Client
  $client = new TwilioRestClient($account_sid, $auth_token);
    
  // Request a call log as a way to test the server connectivity
  voip_error_reset();
  $path = "/$api_version/Accounts/$account_sid/Calls";
  $vars = array();
  $response = $client->request($path, "GET", $vars);

  if($response->IsError){
    voip_error("Twilio command Ping failed: " . $response->ErrorMessage);
    return FALSE;
  }
  return TRUE;
}


/**
 * Establish a new call by dialing the given number and connecting it to the given script
 *
 *   @param array $options is an array containing, among other things, 'variables' to be set in the new channels
 * 
 *   @return XXXXXX the call. Check voip_error() for processing errors.
 */
function _voiptwilio_dial_out($server_config, $call_node) {

/****
dsm('In twilio dial_out');
dsm($server_config);

  // Twilio configuration
  $api_version = '2008-08-01';
  $account_sid = $server_config['voiptwilio_account_sid'];
  $auth_token = $server_config['voiptwilio_auth_token'];
    
  // Instantiate a new Twilio Rest Client
  $client = new TwilioRestClient($account_sid, $auth_token);
    
  // Initiate a new outbound call by POST'ing to the Calls resource 
  voip_error_reset();
  $path = "/$api_version/Accounts/$account_sid/Calls";
  $caller = $server_config['voiptwilio_cid_number'];
  $vars =    array(
      "Caller" => $caller,
      "Called" => "617-489-0192",  // TODO: change this!!
      "Url" => "http://demo.twilio.com/helloworld/"
    );

dsm("About to call client->request($path, 'POST', " . print_r($vars,true));
  $response = $client->request($path, "POST", $vars);
dsm($response);

  if($response->IsError){
    voip_error("Twilio command Ping failed: " . $response->ErrorMessage);
    return FALSE;
  }

dsm("Started call: " . $response->ResponseXml->Call->Sid);
******/


//TODO: set start and end times

  $ami_host = $server_config['voip_asterisk_host'];
  $ami_port = $server_config['voip_asterisk_port'];
  $ami_host = ($ami_port)? $ami_host.':'.$ami_port : $ami_host;
  $ami_user = $server_config['voip_asterisk_user'];
  $ami_pass = $server_config['voip_asterisk_password'];

  // determine the number to be dialed
  $dest_number = $call_node->field_voipcall_dest_number[0]['value'];

  // determine the caller id to be used
  if(!is_null($call_node->field_voipcall_caller_name[0]['value'])){
    $caller_name = $call_node->field_voipcall_caller_name[0]['value'];
  }
  else {
    $caller_name = $server_config['voip_asterisk_cid_name']?$server_config['voip_asterisk_cid_name']:'';
  }
  if(!is_null($call_node->field_voipcall_caller_number[0]['value'])){
    $caller_number = $call_node->field_voipcall_caller_number[0]['value'];
  }
  else {
    $caller_number = $server_config['voip_asterisk_cid_number']?$server_config['voip_asterisk_cid_number']:'';
  }
  $cid = $caller_name . '<' . $caller_number . '>';

  // determine the unique id of the call
  $call_id = $call_node->field_voipcall_call_id[0]['value'];

  // determine the script name to be used
  $script_name = $call_node->field_voipcall_script_name[0]['value'];

  voip_error_reset();

  $ami = _voip_asterisk_manager();
dsm("about to call: connect($ami_host, $ami_user, $ami_pass);");
  $result = $ami->connect($ami_host, $ami_user, $ami_pass);
  if(voip_error()) {
    voip_error("Connection to Asterisk manager failed: " . voip_error_message());
    return FALSE;
  }

  // update the status of the call
  $call_node->call_status = 'dialing';
  $n = voipcall_save($call_node);
dsm('node w/ dialing: ');
dsm($n);

  // The following channel string should match the one in voip_drupal.conf
//  $channel_string = 'Local/%number@vd-dial_out/n'; // This is the goal.  But it's not working...
  $channel_string = 'Local/%number@vd-dial_out'; // This seems to be working... (note the lack of /n at the end)
  $channel = str_replace("%number", $dest_number, $channel_string); // Channel from which to originate the call

  $exten = 's'; // Extension to use on connect (must use Context & Priority with it) 
  $context = 'vd-bridge'; // Context to use on connect (must use Exten & Priority with it) 
  $priority = 1; // Priority to use on connect (must use Context & Exten with it) 
  $application = NULL; // Application to use on connect (use Data for parameters) 
  $application_data = NULL; // Data if Application parameter is used 

  $timeout = 30000; // Timeout (in milliseconds) for the originating connection to happen
                    // (defaults to 30000 milliseconds) 

  $caller_id = $cid;  // CallerID to be used for both ends of the call.  Note that some
                      // voicemail systems might be automatically activated if the
                      // caller id is the same as the number being dialed.

  // Channel variables to set (max 32). Variables will be set for both channels
  // (local and connected). Note that the syntax for Asterisk 1.4+ and 1.6+ is
  // different from the previous versions 
  $variables = $options['variables']?$options['variables']:array(); // NOTE: This might not be supported by non-Asterisk voip servers...
  $variables['VD_SCRIPT_NAME'] = $script_name;
  $variables['VD_CALL_ID'] = $call_id;
  $numOfItems = count($variables);
  $counter = 0;
  $var_string = NULL;
  foreach($variables as $name => $value){
    $counter += 1;
    if ($counter <> $numOfItems){
      $var_string .= $name . '=' . $value . "\r\nVariable: ";
    } else {
      $var_string .= $name . '=' . $value;
    }
  } 

  // Account code for the call. Used for call data record (CDR) management
  $account = $call_id; 

  // Origination mode. Use 0 for the origination to be synchronous and 1 for
  // the origination to be asynchronous (allows multiple calls to be generated
  // without waiting for a response). Using Async leads to an OriginateResponse
  // event which contains the failure reason if any. Reasons may be one of the
  // following: 0 = no such extension or number; 1 = no answer; 4 = answered;
  // and 8 = congested or not available 
  $async = 1; // Note: the async parameter is likely to be removed in future 
              //   versions of Asterisk.  By default Originate calls will be
              //   all asynchronous.

  // The request identifier. It allows you to identify the response to this
  // request. You may use a number or a string. Useful when you make several
  // simultaneous requests. 
  $action_id = $call_id;

dsm("About to call: Originate( $channel, $exten, $context, $priority, $application, $application_data, $timeout, $caller_id, $var_string, $account, $async, $action_id);");
  $result = $ami->Originate(
     $channel, $exten, $context, $priority, $application, $application_data,
     $timeout, $caller_id, $var_string, $account, $async, $action_id);
dsm('result: ' . print_r($result,true));


  if($result['Response'] != 'Success'){
    $call_node->field_voipcall_status[0]['value'] = 'processing error';
    voipcall_save($call_node);
    voip_error("AMI command Originate failed: " . print_r($result, TRUE));
    return FALSE;
  }

  // wait for event confirming result of the originate execution
  $event_handler = '_voip_asterisk_event_handler';
  $event_code = 'OriginateResponse';
dsm("About to call_voip_asterisk_process_events(ami, $event_handler, $event_code, $action_id)");
  $r =  _voip_asterisk_process_events($ami, $event_handler, $event_code, $action_id);

  $ami->disconnect();
  return $call_id;
}
