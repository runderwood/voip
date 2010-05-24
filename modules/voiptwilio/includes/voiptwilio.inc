<?php
// $Id$

/**
 * @file
 * Implementation of Twilio-related functionality
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
 *  is not NULL.  It can assume any of the following values: process_dial,
 *  process_gather, and process_record.
 *
 * @return void
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

  // handle incoming call requests
  if(!$call_id) {
dsm('no call_id.  initializing new voipcall node.');
    $call = array();
    $call_guid = $_REQUEST['CallGuid']; // unique identifier of the call, 34 char
    $call['call_id'] = $call_guid;
    $call['source'] = 'twilio';
    $call['call_status'] = VOIPCALL_IN_PROGRESS;
    $call['start_time'] = time();

    // create a new node for the call
    $call = (array)voipcall_save($call);
dsm("here is the newly created node:");
dsm($call);
  }
  else {
    // load the variables already associated with the call, if any
    $call_nid = voipcall_get_nid_from_call_id($call_id);
    $call = (array)node_load($call_nid);
  }

  // update common call variables
  if(!isset($call['field_voipcall_caller_number'][0]['value'])) $call['caller_number'] = $_REQUEST['Caller'];
  if(!isset($call['field_voipcall_caller_name'][0]['value'])) $call['caller_name'] = $_REQUEST['CallerName'];
  if(!isset($call['field_voipcall_dest_number'][0]['value'])) $call['dest_number'] = $_REQUEST['Called'];
dsm('updating caller and dest');
dsm($call);

  // extract contents from the script_variables field
  // TODO: what to do about serialize/unserialize?
  $variables = unserialize($call['field_voipcall_script_variables'][0]['value']); 
  $call['script_variables'] = $variables;

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
    $call['script_variables']['call_location'] = $call_location;
  }

  $hangup = FALSE;

  // update the status of the call
  $call_status = $_REQUEST['CallStatus']; 
  switch($call_status) {
    default:
      $msg = 'Invalid CallStatus: @status';
      $params = array('@status' => $call_status);
      voipcall_processing_error('voiptwilio', $call, $msg, $params);
      return FALSE;
      break;
    case 'failed':
      $msg = 'Twilio processing error';
      $params = array();
      voipcall_processing_error('voiptwilio', $call, $msg, $params);
      $hangup = TRUE;
      break;
    case 'in-progress':
      $call['call_status'] = VOIPCALL_IN_PROGRESS;
      break;
    case 'completed':
      $call['call_status'] = VOIPCALL_COMPLETED;
      $hangup = TRUE;
      break;
    case 'busy':
      $call['call_status'] = VOIPCALL_BUSY;
      $hangup = TRUE;
      break;
    case 'no-answer':
      $call['call_status'] = VOIPCALL_NO_ANSWER;
      $hangup = TRUE;
      break;
  }

dsm($call, 'about to process responses');
dsm($response_action, 'response_action');
dsm($hangup, 'hangup');
$msg = "about to process responses: $response_action, hangup: $hangup, call: " . print_r($call, TRUE);
watchdog('voiptwilio', $msg, array(), WATCHDOG_INFO);
  // process responses to requests previously-issued by the call script
  if(!$hangup && $response_action) {
    switch($response_action) {
      default:
        $msg = 'Invalid response_action: @action';
        $params = array('action' => $response_action);
        voipcall_processing_error('voiptwilio', $call, $msg, $params);
        return FALSE;
        break;
      case 'process_dial':
        $dial_status = $_REQUEST['DialStatus'];
        switch($dial_status) {
          default:
            $call['call_status'] = VOIPCALL_ERROR;
            $error_message = 'Invalid DialStatus for outbound dial request: ' . $dial_status;
            $call['error_message'] = $error_message;
            watchdog('voiptwilio', $error_msg, array(), WATCHDOG_ERROR);
            voipcall_save($call);
            return FALSE;
            break;
          case 'failed':
            $call['script_variables']['dial_status'] = VOIPCALL_ERROR;
            break;
          case 'answered':
            $call['script_variables']['dial_status'] = VOIPCALL_IN_PROGRESS;
            break;
          case 'busy':
            $call['script_variables']['dial_status'] = VOIPCALL_BUSY;
            break;
          case 'no-answer':
            $call['script_variables']['dial_status'] = VOIPCALL_NO_ANSWER;
            break;
          case 'answered-machine':
            $call['script_variables']['dial_status'] = VOIPCALL_ANSWERED_MACHINE;
            break;
          case 'answered-human':
            $call['script_variables']['dial_status'] = VOIPCALL_ANSWERED_HUMAN;
            break;
          case 'hangup-machine':
            $call['script_variables']['dial_status'] = VOIPCALL_MACHINE_HANGUP;
            break;
        }
        break;
      case 'process_gather':
        $call['script_variables']['gather_digits'] = $_REQUEST['Digits'];
        break;
      case 'process_record':
        $call['script_variables']['recording_url'] = $_REQUEST['RecordingUrl'];
        $call['script_variables']['recording_duration'] = $_REQUEST['Duration'];
        $call['script_variables']['recording_file_size'] = $_REQUEST['RecordingFileSize'];
        $call['script_variables']['recording_digits'] = $_REQUEST['Digits'];
dsm($call, 'in process_record:');
        break;
    }
  }

$msg = "post processing responses: $response_action, hangup: $hangup, call: " . print_r($call, TRUE);
watchdog('voiptwilio', $msg, array(), WATCHDOG_INFO);
  if($hangup) {
    // update core hangup variables
    $call['end_time'] = time();
    $duration = isset($_REQUEST['Duration'])?$_REQUEST['Duration']:NULL;
    if(!$duration) {
      if(isset($call['start_time'])) {
        $duration  = $call['end_time'] - $call['start_time'];
      }
    }
    $call['duration'] = $duration;
  }

dsm('About to save: ');
dsm($call);
  // update the voipcall node with the new values
  $call = voipcall_save($call);

dsm('About to call script handler: ');
dsm($call);
  // process the call script
  $rc = _voiptwilio_script_handler($call);

  return;  // NOTE: this function MUST not return any value.  If it does, the
           //       output to Twilio will get messed up with Drupal hmtl...
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
$msg = 'Entering script_handler with voipcall: ' . print_r($voipcall,true);
watchdog('voiptwilio', $msg, array(), WATCHDOG_INFO);

  if(is_object($voipcall)){
    $voipcall = (array)$voipcall;
  }

  $rc = TRUE;

  // check if call is in a hangup state
  $hangup = voipcall_is_hangup($voipcall);
  if($hangup) {
    $script_name = $voipcall['field_voipcall_hangup_script'][0]['value'];
    // Stop processing if no hangup_script defined
    if(!$script_name) {
      return(TRUE);
    }
  }
  else {
    $script_name = $voipcall['field_voipcall_script_name'][0]['value'];
  }

  // retrieve script context
  $context = voipcall_load_script($voipcall, $script_name);
  $name = $context['script_name'];
  $commands = $context['script_commands'];
  $index = $context['script_index'];
  $variables = $context['script_variables'];
    
  // process script commands
  $processing = TRUE;
  $response = '';
  while($processing) {
   
    if($index >= count($commands)) {
      // no more commands to be processed
      // send a final hangup request 
      $response .= '<Hangup/>';
      $msg = 'Script @name ended without a "hangup" command.';
      $params = array('@name' => $script_name);
      watchdog('voiptwilio', $msg, $params, WATCHDOG_WARNING);
      $processing = FALSE;
    }

    // process current command
    else {

      $command = $commands[$index];
      $cmd_id = $command[VOIPCALL_CMDID];
   
      switch($cmd_id) {
        default:
          $msg = "Invalid command id '@cmd_id' in '@script_name'";
          $params = array('@cmd_id' => $cmd_id, '@script_name' => $script_name);
          voipcall_processing_error('voiptwilio', $voipcall, $msg, $params);
          $rc = FALSE;
          $processing = FALSE;
          break;

        case VOIPCALL_SAY:
          $text = voipcall_script_get_parameter_value($command, 'text', $variables); 
          $response .= "<Say>$text</Say>";
          break;

        case VOIPCALL_PLAY:
          $url = voipcall_script_get_parameter_value($command, 'url', $variables); 
          $response .= "<Play>$url</Play>";
          break;

        case VOIPCALL_RECORD:
          $menu_path = 'voip/twilio/callhandler';
          $call_id = $variables['call_id'];
          $response_url = $menu_path . '/' . $call_id . '/process_record';
          $action = url($response_url, array('absolute' => TRUE));
          $method = 'POST';
          $timeout = voipcall_script_get_parameter_value($command, 'timeout', $variables, 5); 
          $end_key = voipcall_script_get_parameter_value($command, 'end_key', $variables, '#'); 
          $max_length = voipcall_script_get_parameter_value($command, 'max_length', $variables, 3600); 
          // TODO: implement the "transcribe" feature?"
          $play_beep = voipcall_script_get_parameter_value($command, 'play_beep', $variables, 'true'); 
          $response .= '<Record action="' . $action
                            . '" method="' . $method
                            . '" timeout="' . $timeout
                            . '" finishOnKey="' . $end_key
                            . '" maxLength="' . $max_length
                            . '" playBeep="' . $play_beep
                            . '" />';
          $rc = TRUE;
          $processing = FALSE;
          break;

        case VOIPCALL_HANGUP:
          // send the hangup request and stop processing script commands
          // Note: this will trigger a response from Twilio with CallStatus = complete
          $response .= '<Hangup/>';
          $processing = FALSE;
          break;

        case VOIPCALL_GATHER:
          $menu_path = 'voip/twilio/callhandler';
          $call_id = $variables['call_id'];
          $response_url = $menu_path . '/' . $call_id . '/process_gather';
          $action = url($response_url, array('absolute' => TRUE));
          $method = 'POST';
          $timeout = voipcall_script_get_parameter_value($command, 'timeout', $variables, 5); 
          $end_key = voipcall_script_get_parameter_value($command, 'end_key', $variables, '#'); 
          $num_digits = voipcall_script_get_parameter_value($command, 'num_digits', $variables); 
          $text = voipcall_script_get_parameter_value($command, 'text', $variables); 
          $response .= '<Gather action="' . $action
                            . '" method="' . $method
                            . '" timeout="' . $timeout
                            . '" finishOnKey="' . $end_key
                            . '" numDigits="' . $num_digits
                            . '" >';
          $response .= '<Say>' . $text . '</Say>';
          $response .= '</Gather>';
          $rc = TRUE;
          $processing = FALSE;
          break;

        case VOIPCALL_LOAD:
          break;
        case VOIPCALL_SET;
          break;
        case VOIPCALL_DIAL;
          $menu_path = 'voip/twilio/callhandler';
          $call_id = $variables['call_id'];
          $response_url = $menu_path . '/' . $call_id . '/process_dial';
          $action = url($response_url, array('absolute' => TRUE));
          $method = 'POST';
          $number = voipcall_script_get_parameter_value($command, 'number', $variables); 
          // TODO: implement the other features such as multi-dial, url and sendDigits?
          $timeout = voipcall_script_get_parameter_value($command, 'timeout', $variables, 30); 
          // TODO: implement the hangupOnStar feature?
          $time_limit = voipcall_script_get_parameter_value($command, 'time_limit', $variables, '14400'); 
          $caller_id = voipcall_script_get_parameter_value($command, 'caller_id', $variables, $variables['caller_number']); 
          $response .= '<Dial action="' . $action
                            . '" method="' . $method
                            . '" timeout="' . $timeout
                            . '" timeLimit="' . $time_limit
                            . '" callerId="' . $caller_id
                            . '" >';
          $response .= '<Number>' . $number . '</Number>';
          $response .= '</Dial>';
          $rc = TRUE;
          $processing = FALSE;
          break;
      }

      // go to next command
      $index++;
    }
  }

  // store context for next script iteration
  $context['script_index'] = $index;
  $context['script_variables'] = $variables;
$msg = 'before save script context: ' . print_r($voipcall,true);
watchdog('voiptwilio', $msg, array(), WATCHDOG_INFO);

  $voipcall = voipcall_save_script_context($voipcall, $context);

$msg = 'Leaving script_handler with voipcall: ' . print_r($voipcall,true);
watchdog('voiptwilio', $msg, array(), WATCHDOG_INFO);

  // send response back to Twilio
  if($response) {
$response .= '<!--' . "_REQUEST: " . print_r($_REQUEST,true) .  "\n" . '-->';
$response .= '<!--' . "voipcall: " . print_r($voipcall,true) .  "\n" . '-->';
$response .= '<!--' . "script_context: " . print_r($context, true) .  "\n" . '-->';
    _voiptwilio_send_response($response);
  }

  return $rc;
}

/**
 * Send responses to Twilio
 */
function _voiptwilio_send_response($response) {
  $output .= '<?xml version="1.0" encoding="UTF-8" ?>';
  $output .= '<Response>'; 
  $output .= $response;
  $output .= '</Response>';

  drupal_set_header('Content-Type: text/xml; charset=utf-8');

  print $output;

dsm('sending: ' . $output);
  return TRUE;
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
