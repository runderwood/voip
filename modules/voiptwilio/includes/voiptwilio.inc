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
//watchdog('voiptwilio', $msg, $vars, WATCHDOG_INFO);
//dsm($msg);

  // validate $_REQUEST['AccountSid'] to avoid hack attempts
  $server = VoipServer::getServer('twilio');
  $server_config = $server->getConfig();
  $account_sid = $server_config['voiptwilio_account_sid'];
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

  // update script variables
  $script = unserialize($call['field_voipcall_script'][0]['value']); 
  $call['script'] = $script;

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
    $call['script']->setVar('call_location', $call_location);
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
        $call['script']->setVar('dial_status', VOIPCALL_ERROR);
        break;
      case 'answered':
        $call['script']->setVar('dial_status', VOIPCALL_IN_PROGRESS);
        break;
      case 'busy':
        $call['script']->setVar('dial_status', VOIPCALL_BUSY);
        break;
      case 'no-answer':
        $call['script']->setVar('dial_status', VOIPCALL_NO_ANSWER);
        break;
      // NOTE: apparently, Twilio does not detect answering machines with the TwiML Dial verb.  The following error codes are only valid for making calls via REST API
      case 'answered-machine':
        $call['script']->setVar('dial_status', VOIPCALL_ANSWERED_MACHINE);
        break;
      case 'answered-human':
        $call['script']->setVar('dial_status', VOIPCALL_ANSWERED_HUMAN);
        break;
      case 'hangup-machine':
        $call['script']->setVar('dial_status', VOIPCALL_MACHINE_HANGUP);
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
        $call['script']->setVar('input_digits', $_REQUEST['Digits']);
        break;
      case 'process_record':
        // Note: I'm adding an '.mp3' at the end of the url to make sure we use the mp3 file (check http://www.twilio.com/docs/demos/voicerecorder)
        $call['script']->setVar('recording_url', $_REQUEST['RecordingUrl'] . '.mp3');
        $call['script']->setVar('recording_duration', $_REQUEST['Duration']);
        $call['script']->setVar('recording_file_size', $_REQUEST['RecordingFileSize']);
        $call['script']->setVar('recording_digits', $_REQUEST['Digits']);
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
$msg = 'Entering script_handler with voipcall: ' . print_r($voipcall,true);
watchdog('voiptwilio', $msg, array(), WATCHDOG_INFO);

  if(is_object($voipcall)){
    $voipcall = (array)$voipcall;
  }

  $rc = TRUE;

  // check if call is in a hangup state
  $hangup = voipcall_is_hangup($voipcall);
  if($hangup) {
    $script = unserialize($voipcall['field_voipcall_hangup_script'][0]['value']);
    // Stop processing if no hangup_script defined
    if(!$script) {
      return(TRUE);
    }
  }
  else {
    $script = unserialize($voipcall['field_voipcall_script'][0]['value']);
  }

  // update script with current call variables
  $script = voipcall_update_script($voipcall, $script);
    
  // process script commands
  $processing = TRUE;
  $response = '';
  while($processing) {
   
    $cmd = $script->getNextCommand();

    if(!$cmd) {
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

      $cmd_id = $cmd->getId();
   
      switch($cmd_id) {
        default:
          $msg = "Invalid command id '@cmd_id' in script '@script_name': @script";
          $params = array('@cmd_id' => $cmd_id, '@script_name' => $script->getName(), '@script' => print_r($script,true));
          voipcall_processing_error('voiptwilio', $voipcall, $msg, $params);
          $rc = FALSE;
          $processing = FALSE;
          break;

        case 'VoipCmdDial';
          $menu_path = 'voip/twilio/callhandler';
          $call_id = $script->evalString($script->getVar('call_id'));
          $response_url = $menu_path . '/' . $call_id . '/process_dial';
          $action = url($response_url, array('absolute' => TRUE));
          $method = 'POST';
          $number = $script->evalString($cmd->getParam('number'));
          // TODO: implement the other features such as multi-dial, url and sendDigits?
          $timeout = $script->evalString($cmd->getParam('timeout', 30));
          // TODO: implement the hangupOnStar feature?
          $time_limit = $script->evalString($cmd->getParam('time_limit', '14400'));
          $caller_number = $script->evalString($script->getVar('caller_number'));
          $caller_id = $script->evalString($cmd->getParam('caller_id', $caller_number)); 
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

        case 'VoipCmdGetInput':
          $menu_path = 'voip/twilio/callhandler';
          $call_id = $script->evalString($script->getVar('call_id'));
          $response_url = $menu_path . '/' . $call_id . '/process_gather';
          $action = url($response_url, array('absolute' => TRUE));
          $method = 'POST';
          $timeout = $script->evalString($cmd->getParam('timeout', 5)); 
          $end_key = $script->evalString($cmd->getParam('end_key', '')); 
          $num_digits = $script->evalString($cmd->getParam('num_digits')); 
          $text = $script->evalString($cmd->getParam('text')); 
          $url = $script->evalString($cmd->getParam('url'));
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
          // in case of timeout or empty input, set Digits to VoipScript::NO_INPUT
          $action_timeout = $action . '&amp;Digits=' . VoipScript::NO_INPUT;
          $response .= '<Redirect method="GET">'
                         . $action_timeout
                         . '</Redirect>';
          $rc = TRUE;
          $processing = FALSE;
          break;

        case 'VoipCmdGosub':
          // TODO: implement gosub
          $script_name = $script->evalString($cmd->getParam('script_name'));
          $rc = $script->runGosub($script_name);
          if($script->isError()){
              $msg = $script->getErrorMessage();
              $params = array();
              voipcall_processing_error('voiptwilio', $voipcall, $msg, $params);
              $rc = FALSE;
              $processing = FALSE;
              $found = TRUE;
              $response .= "<Say>Processing error in gosub command.</Say>";

          }
          break;

        case 'VoipCmdGoto';
          $label_name = $script->evalString($cmd->getParam('label_name', ''));
          $rc = $script->runGoto($label_name);
          if($script->isError()){
            $msg = $script->getErrorMessage();
            $params = array();
            voipcall_processing_error('voiptwilio', $voipcall, $msg, $params);
            $rc = FALSE;
            $processing = FALSE;
            $found = TRUE;
            $response .= "<Say>$msg.</Say>";
          }
          break;

        case 'VoipCmdGotoIf';
          $condition = $script->evalString($cmd->getParam('condition', ''));
          if($condition) { 
            $label_name = $script->evalString($cmd->getParam('label_name', ''));
            $rc = $script->runGoto($label_name, $condition);
            if($script->isError()){
              $msg = $script->getErrorMessage();
              $params = array();
              voipcall_processing_error('voiptwilio', $voipcall, $msg, $params);
              $rc = FALSE;
              $processing = FALSE;
              $found = TRUE;
              $response .= "<Say>$msg.</Say>";
            }
          }
          break;

        case 'VoipCmdHangup':
          // send the hangup request and stop processing script commands
          // Note: this will trigger a response from Twilio with CallStatus = complete
          $response .= '<Hangup/>';
          $processing = FALSE;
          break;

        case 'VoipCmdLabel':
          // don't do anything. Labels are just placeholders that mark a position within the script.
          break;

        case 'VoipCmdLoad':
          $script_name = $script->evalString($cmd->getParam('script_name', ''));
          $rc = $script->runLoad($script_name);
          if($script->isError()){
            $msg = $script->getErrorMessage();
            $params = array();
            voipcall_processing_error('voiptwilio', $voipcall, $msg, $params);
            $rc = FALSE;
            $processing = FALSE;
            $found = TRUE;
            $response .= "<Say>$msg.</Say>";
          }
          break;

        case 'VoipCmdPlay':
          $url = $script->evalString($cmd->getParam('url'));
          $loop = $script->evalString($cmd->getParam('loop', 1));
          $response .= '<Play loop="' . $loop . '" >' . $url . '</Play>';
          break;

        case 'VoipCmdRecord':
          $menu_path = 'voip/twilio/callhandler';
          $call_id = $script->evalString($script->getVar('call_id'));
          $response_url = $menu_path . '/' . $call_id . '/process_record';
          $action = url($response_url, array('absolute' => TRUE));
          $method = 'POST';
          $timeout = $script->evalString($cmd->getParam('timeout', 5));
          $end_key = $script->evalString($cmd->getParam('end_key', '#'));
          $max_length = $script->evalString($cmd->getParam('max_length', 3600));
          // TODO: implement the "transcribe" feature?"
          $play_beep = $script->evalString($cmd->getParam('play_beep', 'true'));
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

        case 'VoipCmdReturn':
          $rc = $script->runReturn();
          if($script->isError()){
            $msg = $script->getErrorMessage();
            $params = array();
            voipcall_processing_error('voiptwilio', $voipcall, $msg, $params);
            $rc = FALSE;
            $processing = FALSE;
            $found = TRUE;
            $msg = t('Processing error. Please contact your system administrator.');
            $response .= "<Say>$msg.</Say>";
          }
          break;

        case 'VoipCmdSay':
          $text = $script->evalString($cmd->getParam('text'));
          $response .= "<Say>$text</Say>";
          break;

        case 'VoipCmdSet';
          $var_name = $script->evalString($cmd->getParam('var_name'));
          $var_value = $script->evalString($cmd->getParam('var_value'));
          $script->setVar($var_name, $var_value);
          break;

        case 'VoipCmdUnset';
          $var_name = $script->evalString($cmd->getParam('var_name'));
          $script->unsetVar($var_name);
          break;

      }

      // go to next command
    }
  }

  // save script for next iteration
  $voipcall['script'] = $script;
  $voipcall = voipcall_save($voipcall);

  // send response back to Twilio
  if($response) {

//$response .= '<!--' . "_REQUEST: " . print_r($_REQUEST,true) .  "\n" . '-->';
//$response .= '<!--' . "voipcall: " . print_r($voipcall,true) .  "\n" . '-->';

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

  return TRUE;
}


