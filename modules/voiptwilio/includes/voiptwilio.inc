<?php
// $id$

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
//dsm($msg);

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
    $call = array();
    $call_guid = $_REQUEST['CallGuid']; // unique identifier of the call, 34 char
    $call['call_id'] = $call_guid;
    $call['source'] = 'twilio';
    $call['call_status'] = VOIPCALL_IN_PROGRESS;
    $call['start_time'] = time();

    // create a new node for the call
    $call = (array)voipcall_save($call);
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

  // update dial status, if available
  // Note: dial status is only available in case of a previously made
  //        outbound call request
  $dial_status = $_REQUEST['DialStatus'];
  if($dial_status) {
    switch($dial_status) {
      default:
        $call['call_status'] = VOIPCALL_ERROR;
        $error_message = 'Invalid DialStatus for outbound dial request: ' . $dial_status;
        $call['error_message'] = $error_message;
        watchdog('voiptwilio', $error_msg, array(), WATCHDOG_ERROR);
        voipcall_save($call);
        return FALSE;
        break;
      case 'fail': // From: REST API - making calls
      case 'failed': // From: TwiML Reference - Dial verb
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
      // NOTE: apparently, Twilio does not detect answering machines with the TwiML Dial verb.  The following error codes are only valid for making calls via REST API
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
  }

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
        // nothing special to be done here. dial_status has already been 
        // updated a few lines above
        break;
      case 'process_gather':
        $call['script_variables']['gather_digits'] = $_REQUEST['Digits'];
        break;
      case 'process_record':
        // Note: I'm adding an '.mp3' at the end of the url to make sure we use the mp3 file (check http://www.twilio.com/docs/demos/voicerecorder)
        $call['script_variables']['recording_url'] = $_REQUEST['RecordingUrl'] . '.mp3';
        $call['script_variables']['recording_duration'] = $_REQUEST['Duration'];
        $call['script_variables']['recording_file_size'] = $_REQUEST['RecordingFileSize'];
        $call['script_variables']['recording_digits'] = $_REQUEST['Digits'];
        break;
    }
  }

  if($hangup) {
    // update core hangup variables
    $call['end_time'] = time();
    if(!$duration) {
      $start_time = $call['field_voipcall_start_time'][0]['value'];
      if($start_time) {
        $duration  = $call['end_time'] - $start_time;
      }
      else {
        $duration = isset($_REQUEST['Duration'])?$_REQUEST['Duration']:NULL;
      }
    }
    $call['duration'] = $duration;
  }

  // update the voipcall node with the new values
  $call = voipcall_save($call);

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
//dsm('entering voiptwilio_script_handler()');
//dsm($voipcall);
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
    $reset_index = TRUE;
    // Stop processing if no hangup_script defined
    if(!$script_name) {
      return(TRUE);
    }
  }
  else {
    $script_name = $voipcall['field_voipcall_script_name'][0]['value'];
    $reset_index = FALSE;
  }

  // retrieve script context
  $context = voipcall_load_script_context($voipcall, $script_name, $reset_index);
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
      $msg = 'Script @name ended without a "hangup" command. Current commands: @commands';
      $params = array('@name' => $script_name, '@commands' => print_r($commands, TRUE));
      watchdog('voiptwilio', $msg, $params, WATCHDOG_WARNING);
      $processing = FALSE;
    }

    // process current command
    else {

      $command = $commands[$index];
      $cmd_id = $command[VOIPCALL_CMD_ID];
   
      switch($cmd_id) {
        default:
          $msg = "Invalid command id '@cmd_id' (index: @index) in script '@script_name': @script";
          $params = array('@cmd_id' => $cmd_id, '@index' => $index, '@script_name' => $script_name, '@script' => print_r($commands,true));
          voipcall_processing_error('voiptwilio', $voipcall, $msg, $params);
          $rc = FALSE;
          $processing = FALSE;
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

        case VOIPCALL_GATHER:
          $menu_path = 'voip/twilio/callhandler';
          $call_id = $variables['call_id'];
          $response_url = $menu_path . '/' . $call_id . '/process_gather';
          $action = url($response_url, array('absolute' => TRUE));
          $method = 'POST';
          $timeout = voipcall_script_get_parameter_value($command, 'timeout', $variables, 5); 
          $end_key = voipcall_script_get_parameter_value($command, 'end_key', $variables, ''); 
          $num_digits = voipcall_script_get_parameter_value($command, 'num_digits', $variables); 
          $text = voipcall_script_get_parameter_value($command, 'text', $variables); 
          $url = voipcall_script_get_parameter_value($command, 'url', $variables); 
          $response .= '<Gather action="' . $action
                            . '" method="' . $method
                            . '" timeout="' . $timeout
                            . '" finishOnKey="' . $end_key
                            . '" numDigits="' . $num_digits
                            . '" >';
          if($text) {
            $response .= '<Say>' . $text . '</Say>';
          }
          else if ($url) {
            $response .= '<Play>' . $url . '</Play>';
          }
          $response .= '</Gather>';
          // in case of timeout or empty input, set Digits to VOIPCALL_NO_INPUT
          $action_timeout = $action . '&amp;Digits=' . VOIPCALL_NO_INPUT;
          $response .= '<Redirect method="GET">'
                         . $action_timeout
                         . '</Redirect>';
          $rc = TRUE;
          $processing = FALSE;
          break;

        case VOIPCALL_HANGUP:
          // send the hangup request and stop processing script commands
          // Note: this will trigger a response from Twilio with CallStatus = complete
          $response .= '<Hangup/>';
          $processing = FALSE;
          break;

        case VOIPCALL_GOTO;
          $label_name = voipcall_script_get_parameter_value($command, 'label_name', $variables, '');
          $found = FALSE;
          $i = 0;
          $i_max = count($commands);
          while(!$found) {
            if($i >= $i_max) {
              // label nor found
              $msg = "Label '@label_name' not found in script '@script_name': @script";
              $params = array('@label_name' => $label_name, '@script_name' => $script_name, '@script' => print_r($commands, true));
              voipcall_processing_error('voiptwilio', $voipcall, $msg, $params);
              $rc = FALSE;
              $processing = FALSE;
              $found = TRUE;
              $response .= "<Say>Error: label $label_name not found in script $script_name</Say>";
            }
            else {
              $cmd_tmp = $commands[$i];
              $cmd_id_tmp = $cmd_tmp[VOIPCALL_CMD_ID];
              $label_name_tmp = voipcall_script_get_parameter_value($cmd_tmp, 'label_name', $variables, '');
              if(($cmd_id_tmp == VOIPCALL_LABEL) && ($label_name_tmp == $label_name)) {
                $index = $i;
                $found = TRUE;
              }
            }
            // move to the next command in the script
            $i++;
          }
          break;

        case VOIPCALL_GOTOIF;
          // evaluate expression
          $expression = voipcall_script_get_parameter_value($command, 'expression', $variables, '');
          if($expression) {
            $label_name = voipcall_script_get_parameter_value($command, 'label_name', $variables, '');
            $found = FALSE;
            $i = 0;
            $i_max = count($commands);
            while(!$found) {
              if($i >= $i_max) {
                // label nor found
                $msg = "Label '@label_name' not found in script '@script_name': @script";
                $params = array('@label_name' => $label_name, '@script_name' => $script_name, '@script' => print_r($commands, true));
                voipcall_processing_error('voiptwilio', $voipcall, $msg, $params);
                $rc = FALSE;
                $processing = FALSE;
                $found = TRUE;
                $response .= "<Say>Error: label $label_name not found in script $script_name</Say>";
              }
              else {
                $cmd_tmp = $commands[$i];
                $cmd_id_tmp = $cmd_tmp[VOIPCALL_CMD_ID];
                $label_name_tmp = voipcall_script_get_parameter_value($cmd_tmp, 'label_name', $variables, '');
                if(($cmd_id_tmp == VOIPCALL_LABEL) && ($label_name_tmp == $label_name)) {
                  $index = $i;
                  $found = TRUE;
                }
              }
              // move to the next command in the script
              $i++;
            }
          }
          break;

        case VOIPCALL_LABEL:
          // don't do anything. Labels are just placeholders that mark a position within the script.
          break;

        case VOIPCALL_LOAD:
// TODO: implement load
          break;

        case VOIPCALL_PLAY:
          $url = voipcall_script_get_parameter_value($command, 'url', $variables); 
          $loop = voipcall_script_get_parameter_value($command, 'loop', $variables, 1); 
          $response .= '<Play loop="' . $loop . '" >' . $url . '</Play>';
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

        case VOIPCALL_SAY:
          $text = voipcall_script_get_parameter_value($command, 'text', $variables); 
          $response .= "<Say>$text</Say>";
          break;

        case VOIPCALL_SET;
          $var_name = voipcall_script_get_parameter_value($command, 'var_name', $variables);
          $var_value = voipcall_script_get_parameter_value($command, 'var_value', $variables);
          $variables[$var_name] = $var_value;
          break;

      }

      // go to next command
      $index++;
    }
  }

  // store context for next script iteration
  $context['script_index'] = $index;
  $context['script_variables'] = $variables;
  $voipcall = voipcall_save_script_context($voipcall, $context);

$msg = 'Leaving script_handler with voipcall: ' . print_r($voipcall,true) . ", and response: $response";
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
function _voiptwilio_dial_out($server_config, $voipcall) {
//dsm('In twilio dial_out');
//dsm($server_config, '$server_config');
//dsm($voipcall, '$voipcall');

  $rc = FALSE;
  voip_error_reset();

  if(is_object($voipcall)) {
    $voipcall = (array)$voipcall;
  }

  // Twilio configuration
  $api_version = '2008-08-01';
  $account_sid = $server_config['voiptwilio_account_sid'];
  $auth_token = $server_config['voiptwilio_auth_token'];
    
  // Update call variables
  $voipcall['call_status'] = VOIPCALL_DIALING;
  $voipcall['source'] = 'drupal';
  $voipcall = (array)voipcall_save($voipcall);
//dsm($voipcall,'right after save. everything is ok');

  // Instantiate a new Twilio Rest Client
  $client = new TwilioRestClient($account_sid, $auth_token);
    
  // Initiate a new outbound call by POST'ing to the Calls resource 
  $twilio_path = "/$api_version/Accounts/$account_sid/Calls";
  $caller_number = ($voipcall['field_voipcall_caller_number'][0]['value'])
                     ? $voipcall['field_voipcall_caller_number'][0]['value']
                     : variable_get('voipcall_cid_number', NULL);
  $dest_number = $voipcall['field_voipcall_dest_number'][0]['value'];

  $menu_path = 'voip/twilio/callhandler';
  $call_id = $voipcall['field_voipcall_call_id'][0]['value'];
  $response_url = $menu_path . '/' . $call_id;
  $call_handler_url = url($response_url, array('absolute' => TRUE));
 
  $vars =    array(
      "Caller" => $caller_number,
      "Called" => $dest_number,
      "Url" => $call_handler_url,
      'Method' => 'POST',
      'SendDigits' => '',  // TODO: shall we encode extra digits with dest_number?
// TODO: ideally, IfMachine should be set via field 'check_machine' with options such as 'do not check', 'continue' and 'hangup'
      'IfMachine' => 'Continue', // Note: adding any value in here -- even 'Continue' -- adds a 5 sec delay to the call. To prevent that, use NULL
      'Timeout' => 60 // TODO: Set to 15 in order to avoid answering machines?
    );

  $response = $client->request($twilio_path, "POST", $vars);

  // update call based on response
  if($response->IsError){
    $msg = "Twilio dial command failed: @error";
    $params = array('@error' => $response->ErrorMessage);
    voipcall_processing_error('voiptwilio', $voipcall, $msg, $params);
    $rc = FALSE;
  }
  else {
    // Note: call_status will be updated by call_handler
    $voipcall['start_time'] = time();
    $voipcall = voipcall_save($voipcall);
    $rc = $call_id;
  }
 
//dsm($voipcall, "Dial result");

  return $rc;
}


