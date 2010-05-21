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

/*
 * Process incoming requests and responses from Twilio
 *
 * @param string $call_id with the call's unique identifier.  If NULL, the
 *   call is coming from Twilio and 'CallGuid' should be used as the unique id.
 *
 * @param string $response_action set to the action to be executed in response
 *  to a previously issued request to Twilio.  Is only valid with $call_id
 *  is not NULL.  It can assume any of the following values: process_gather
 *  and process_record.
 *
 */
function _voiptwilio_call_handler($call_id=NULL, $response_action=NULL) {

$msg = "in voiptwilio_call_handler() with call_id: $call_id, response_action: $response_action, and _REQUEST: " . print_r($_REQUEST, TRUE);
$vars = array();
watchdog('voiptwilio', $msg, $vars, WATCHDOG_INFO);
dsm($msg);

  // validate $_REQUEST['AccountSid'] to avoid hack attempts
  $server = voip_servers('server', 'twilio');
  $account_sid = $server['configuration']['voiptwilio_account_sid'];
  $incoming_sid = $_REQUEST['AccountSid'];
  if ($account_sid != $incoming_sid) {
     $msg = '_voip_twilio_call_handler called with invalid AccountSid: @sid';
     $params = array('@sid' => $incoming_sid);
     watchdog('voiptwilio', $msg, $params, WATCHDOG_WARNING);
     return FALSE;  // TODO: should we return an error response to the server?
  }

  // load the variables already associated with the call, if any
  if($call_id) {
    $call_nid = voipcall_get_nid_from_call_id($call_id);
    $call = (array)node_load($call_nid);
  }
  else {
    $call = array();
    $call_guid = $_REQUEST['CallGuid']; // unique identifier of the call, 34 char
    $call['call_id'] = $call_guid;
    $call['source'] = 'twilio';
    $call['status'] = VOIPCALL_NOT_CALLED;
    $call['start_time'] = time();

    // for the sake of consistency, init the new node before updating it
    $call = (array)voipcall_save($call);
  }

  // update common call variables
  if(!isset($call['caller_number'])) $call['field_voipcall_caller_number'][0]['value'] = $_REQUEST['Caller'];
  if(!isset($call['caller_name'])) $call['field_voipcall_caller_name'][0]['value'] = $_REQUEST['CallerName'];
  if(!isset($call['dest_number'])) $call['field_voipcall_dest_number'][0]['value'] = $_REQUEST['Called'];

  $hangup = FALSE;

  // handle 'dial' requests and responses
  $dial_status = isset($_REQUEST['DialStatus'])?$_REQUEST['DialStatus']:NULL;
  if($dial_status) {

    // store call location, if available
    $call_location = array();
    if(isset($_REQUEST['CallerCity'])) $call_location['caller_city'] = $_REQUEST['CallerCity'];
    if(isset($_REQUEST['CallerState'])) $call_location['caller_state'] = $_REQUEST['CallerState'];
    if(isset($_REQUEST['CallerZip'])) $call_location['caller_zip'] = $_REQUEST['CallerZip'];
    if(isset($_REQUEST['CallerCountry'])) $call_location['caller_country'] = $_REQUEST['CallerCountry'];
    
    if(isset($_REQUEST['CalledCity'])) $call_location['called_city'] = $_REQUEST['CalledCity'];
    if(isset($_REQUEST['CalledState'])) $call_location['called_state'] = $_REQUEST['CalledState'];
    if(isset($_REQUEST['CalledZip'])) $call_location['calldr_zip'] = $_REQUEST['CalledZip'];
    if(isset($_REQUEST['CalledCountry'])) $call_location['called_country'] = $_REQUEST['CalledCountry'];
    if($call_location) {
      // TODO: what to do about serialize/unserialize?
      $variables = unserialize($call['field_voipcall_script_commands'][0]['value']); 
      $variables['call_location'] = $call_location;
      $call['script_variables'] = $variables;
    }

    switch($dial_status){
      default:
        $call['status'] = VOIP_CALL_ERROR;
        $error_message = 'Invalid DialStatus: ' . $dial_status;
        $call['error_message'] = $error_message;
        watchdog('voiptwilio', $error_msg, array(), WATCHDOG_ERROR);
        voipcall_save($call);
        return FALSE;
        break;
      case 'failed':
        $call['status'] = VOIP_CALL_ERROR;
        $error_message = 'Twilio processing error';
        $call['error_message'] = $error_message;
        watchdog('voiptwilio', $error_msg, array(), WATCHDOG_ERROR);
        $hangup = TRUE;
        break;
      case 'answered':
        $call['status'] = VOIP_CALL_ANSWERED;
        break;
      case 'busy':
        $call['status'] = VOIP_CALL_BUSY;
        $hangup = TRUE;
        break;
      case 'no-answer':
        $call['status'] = VOIP_CALL_NO_ANSWER;
        $hangup = TRUE;
        break;
      case 'answered-machine':
        $call['status'] = VOIP_CALL_ANSWERED_MACHINE;
        break;
      case 'answered-human':
        $call['status'] = VOIP_CALL_ANSWERED_HUMAN;
        break;
      case 'hangup-machine':
        $call['status'] = VOIP_CALL_MACHINE_HANGUP;
        $hangup = TRUE;
        break;
    }
  }

  // process requests/responses from ongoing calls
  else {
    $call_status = $_REQUEST['CallStatus']; 
    switch($call_status) {
      default:
        $call['status'] = VOIP_CALL_ERROR;
        $error_message = 'Invalid CallStatus: ' . $call_status;
        $call['error_message'] = $error_message;
        watchdog('voiptwilio', $error_msg, array(), WATCHDOG_ERROR);
        voipcall_save($call);
        return FALSE;
        break;
      case 'failed':
        $call['status'] = VOIP_CALL_ERROR;
        $error_message = 'Twilio processing error';
        $call['error_message'] = $error_message;
        watchdog('voiptwilio', $error_msg, array(), WATCHDOG_ERROR);
        $hangup = TRUE;
        break;
      case 'in-progress':
        $call['status'] = VOIP_CALL_ANSWERED;
        break;
      case 'completed':
        $call['status'] = VOIP_CALL_COMPLETED;
        $hangup = TRUE;
        break;
      case 'busy':
        $call['status'] = VOIP_CALL_BUSY;
        $hangup = TRUE;
        break;
      case 'no-answer':
        $call['status'] = VOIP_CALL_NO_ANSWER;
        $hangup = TRUE;
        break;
    }
  }

  if(!$hangup && $response_action) {
    // process response actions, if any
    switch($response_action) {
      default:
        $call['status'] = VOIP_CALL_ERROR;
        $error_message = 'Invalid response_action: ' . $response_action;
        $call['error_message'] = $error_message;
        watchdog('voiptwilio', $error_msg, array(), WATCHDOG_ERROR);
        voipcall_save($call);
        return FALSE;
        break;
      case 'process_record':
        $variables = unserialize($call['field_voipcall_script_commands'][0]['value']); 
        $variables['recording_url'] = $_REQUEST['RecordingUrl'];
        $variables['recording_duration'] = $_REQUEST['Duration'];
        $variables['recording_digits'] = $_REQUEST['Digits'];
        $call['script_variables'] = $variables;
        break;
      case 'process_gather':
        $variables = unserialize($call['field_voipcall_script_commands'][0]['value']); 
        $variables['gather_digits'] = $_REQUEST['Digits'];
        $call['script_variables'] = $variables;
        break;
// TODO: don't forget to pass action to script_handler. it should only advance index after processing the response action.
    }
  }

  if($hangup) {
    // update basic hangup variables
    $call['end_time'] = time();
    $duration = isset($_REQUEST['Duration'])?$_REQUEST['Duration']:NULL;
    if(!$duration) {
      if(isset($call['start_time'])) {
        $duration  = $call['end_time'] - $call['start_time'];
      }
    }
    $call['duration'] = $duration;
  }

  // update the voipcall node with the new values
  $call = voipcall_save($call);

  // process the call script
  $rc = _voiptwilio_script_handler($call);

  return;  // NOTE: this function cannot return any value.  If it does, the output to Twilio will get messed up with Drupal hmtl...
}

/**
 * Process voipcall scripts
 *
 * @param $voipcall, node with the call information
 *
 * @return boolean.  TRUE on success or FALSE on failure.  Check voip_error() for processing failure.
 */
function _voiptwilio_script_handler($voipcall) {
dsm('entering voiptwilio_script_handler()');
dsm($voipcall);

  if(is_object($voipcall)){
    $voipcall = (array)$voipcall;
  }

  $rc = TRUE;

  // retrieve script context
  $context = _voiptwilio_load_context($voipcall);

  $name = $context['script_name'];
  $commands = $context['script_commands'];
  $index = $context['script_index'];
  $variables = $context['script_variables'];
    
  // process script commands
  $processing = TRUE;
  while($processing) {
/*********

    // process response requested by current command
// TODO: with VOIPCALL_SET, this won't be needed anymore... vars will be set automatically by commands... and then renamed/copied if needed
    $process_response = $context['process_response'];
    if($process_response) {
      $cmd_id = $commands[$index]['id'];
      $cmd_arg = $commands[$index]['arg'];
      switch($cmd_id){
        default:
          $msg = 'Invalid command id @cmd_id in @script_name';
          $params = array('@cmd_id' => $cmd_id, '@script_name' => $script_name);
          watchdog('voiptwilio',$msg, $params, WATCHDOG_ERROR);
          $msg2 = "Invalid command $cmd_id in $script_name";
          voip_error($msg2);
          $voipcall['error_message'] = $msg2;
          $voipcall['status'] = VOIPCALL_ERROR;
          $voipcall_save($voipcall);
          $rc = FALSE;
          $processing = FALSE;
          break;
        case 'VOIPCALL_RECORD':
        case 'VOIPCALL_GATHER':
          break;
      }
    }
**********/
   
    if($processing) {
      // process next command, if any
      $index++;

      if($index >= count($commands)) {
        // no more commands to be processed
        $processing = FALSE;
      }
      else {

        $cmd_id = $commands[$index]['id'];
        $cmd_arg = $commands[$index]['arg'];
   
        switch($cmd_id) {
          default:
            $msg = 'Invalid command id @cmd_id in @script_name';
            $params = array('@cmd_id' => $cmd_id, '@script_name' => $script_name);
            watchdog('voiptwilio',$msg, $params, WATCHDOG_ERROR);
            $msg2 = "Invalid command $cmd_id in $script_name";
            voip_error($msg2);
            $voipcall['error_message'] = $msg2;
            $voipcall['status'] = VOIPCALL_ERROR;
            $voipcall_save($voipcall);
            $rc = FALSE;
            $processing = FALSE;
            break;
          case 'VOIPCALL_SAY':
            if($cmd_arg[0] == 'var') {
              $var_name = $cmd_arg[1];
              $text = $variables[$var_name];
            }
            else {
              $text = $cmd_arg[1];
            }
            break;

          case 'VOIPCALL_PLAY':
            break;
          case 'VOIPCALL_RECORD':
            break;
          case 'VOIPCALL_HANGUP':
            break;
          case 'VOIPCALL_GATHER':
            break;
          case 'VOIPCALL_GET_SCRIPT':
            break;
          case 'VOIPCALL_SET';
          break;
        }
      }
    }
  }

  // update voipcall with current context values
// TODO: update $context with $index, variables, etc. and do the above with _voiptwilio_save_context($voipcall, $context);
  // send response back to Twilio
  $response = '<!--' . "voipcall: " . print_r($voipcall,true) .  "\n" . '-->';
  $response .= '<!--' . "script_context: " . print_r($context, true) .  "\n" . '-->';
  $response .= '<!--' . "script_variables: " . print_r($script_variables,true) .  "\n" . '-->';
  $response .= '
      <Response> 
        <Say>Hello World</Say>
        <Play>http://api.twilio.com/Cowbell.mp3</Play>
        <dance>heyhey</dance>
      </Response> ';
  _voiptwilio_send_response($response);

  return $rc;
}

/**
 * Send response back to Twilio
 */
function _voiptwilio_send_response($twiml_response) {
  $output = '<?xml version="1.0" encoding="UTF-8" ?>';
  $output .= '<Response>'; 
  $output .= $twiml_response;
  $output .= '</Response>';

  drupal_set_header('Content-Type: text/xml; charset=utf-8');
  print $output;

  return TRUE;
}


/**
 * Load script context associated with the given call
 *
 * @param array $voipcall instance with the script info
 *
 * @return array with the new context. Check voip_error() for processing errors.
 */
function _voiptwilio_load_context($voipcall) {

  $context = array();

  $context['status'] = $voipcall['field_voipcall_status'][0]['value'];

  $context['script_name'] = $voipcall['field_voipcall_script_name'][0]['value'];
  $context['script_commands'] = voipcall_get_script($script_name, $voipcall);
  $context['script_index'] = $voipcall['field_voipcall_script_index'][0]['value'];
  $context['script_variables'] = unserialize($voipcall['field_voipcall_script_variables'][0]['value']);
  $context['process_response'] = $voipcall['field_voipcall_process_response'][0]['value'];

  // add call variables to script variables

  $context['script_variables']['uid'] = $voipcall['uid'];

  $context['script_variables']['status'] = $status;

  $context['script_variables']['call_id'] = $voipcall['field_voipcall_call_id'][0]['value'];
  $context['script_variables']['dest_number'] = $voipcall['field_voipcall_dest_number'][0]['value'];
  $context['script_variables']['caller_number'] = $voipcall['field_voipcall_caller_number'][0]['value'];
  $context['script_variables']['caller_name'] = $voipcall['field_voipcall_caller_name'][0]['value'];

  $context['script_variables']['source'] = $voipcall['field_voipcall_source'][0]['value'];

  $context['script_variables']['start_time'] = $voipcall['field_voipcall_start_time'][0]['value'];
  $context['script_variables']['end_time'] = $voipcall['field_voipcall_end_time'][0]['value'];
  $context['script_variables']['duration'] = $voipcall['field_voipcall_duration'][0]['value'];

  $context['script_variables']['error_message'] = $voipcall['field_voipcall_error_message'][0]['value'];

  return $context;
}

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
