<?php

// $Id$

/**
 * @file
 * Implementation of the VoipTwilioServer class
 */

// TODO: remove the line below once testing is done
require_once(dirname(__FILE__) . DIRECTORY_SEPARATOR . '../../../includes/voipserver.inc');


/**
 * Class definitions
 */

class VoipTwilioServer extends VoipServer {

  protected $account_sid;

  protected $auth_token;

  protected $menu_path = 'voip/twilio/callhandler';


  /*
   * Constructors
   */

  function __construct($account_sid, $auth_token) {
    $this->account_sid = $account_sid;
    $this->auth_token = $auth_token;
  }


  /*
   * API methods
   */

  // Test the connection with the Twilio server
  function ping() {
    $api_version = '2008-08-01';

    // Instantiate a new Twilio Rest Client
    $client = new TwilioRestClient($this->account_sid, $this->auth_token);

    // Request a call log as a way to test the server connectivity
    $this->is_error = FALSE;
    $path = "/$api_version/Accounts/$this->account_sid/Calls";
    $vars = array();
    $response = $client->request($path, "GET", $vars);

    if($response->IsError){
      $this->is_error = TRUE;
      $this->error_message = "Twilio command Ping failed: " . $response->ErrorMessage;
      return FALSE;
    }

    return TRUE;
  }


  /*
   * Implementation of script commands
   */

  function runDial($call_id, $number, $timeout=30, $hangup_key='*', $time_limit=3600, $caller_number=''){
    $is_hangup = self::is_hangup($call_id);
    if($is_hangup) {
      throw new VoipHangupException($call_id);
    }

    $response_url = $this->menu_path . '/' . $call_id . '/process_dial';
    $action = url($response_url, array('absolute' => TRUE));
    $method = 'POST';
    // TODO: implement the other features such as multi-dial, url and sendDigits?
    // TODO: implement the hangupOnStar feature?
    $command = '<Dial action="' . $action
                  . '" method="' . $method
                  . '" timeout="' . $timeout
                  . '" timeLimit="' . $time_limit
                  . '" callerId="' . $caller_number
                  . '" >';
    $command .= '<Number>' . $number . '</Number>';
    $command .= '</Dial>';
    $rc = $this->send_command($call_id, $command, TRUE);
// TODO: process response from dial command here (check for hangups, etc.)
    return $rc;
  }

  function runGetInput($call_id, $text='', $url='', $timeout=5, $end_key='#', $num_digits=NULL) {
    $is_hangup = self::is_hangup($call_id);
    if($is_hangup) {
      throw new VoipHangupException($call_id);
    }

    $response_url = $this->menu_path . '/' . $call_id . '/process_gather';
    $action = url($response_url, array('absolute' => TRUE));
    $method = 'POST';
    $command = '<Gather action="' . $action
                  . '" method="' . $method
                  . '" timeout="' . $timeout
                  . '" finishOnKey="' . $end_key
                  . '" numDigits="' . $num_digits
                  . '" >';
    if($text) {
      $command .= '<Say>' . $text . '</Say>';
    }
    else if ($url) {
      $command .= '<Play>' . $url . '</Play>';
    }
    $command .= '</Gather>';
    // in case of timeout or empty input, set Digits to VoipScript::NO_INPUT
    $action_timeout = $action . '&amp;Digits=' . VoipScript::NO_INPUT;
    $command = '<Redirect method="GET">'
                  . $action_timeout
                  . '</Redirect>';
    $rc = $this->send_command($call_id, $command, TRUE);
// TODO: process response from get input here
    return $rc;
  }

  function runHangup($call_id) {
    $is_hangup = self::is_hangup($call_id);
    if($is_hangup) {
      return TRUE;
    }

    $command = '<Hangup/>';
    $rc = $this->send_command($call_id, $command);
    return $rc;
  }

  function runPlay($call_id, $url, $loop=1) {
    $is_hangup = self::is_hangup($call_id);
    if($is_hangup) {
      throw new VoipHangupException($call_id);
    }

    $command = '<Play loop="' . $loop . '" >' . $url . '</Play>';
    $this->send_command($call_id, $command);
  }

  function runRecord($call_id, $timeout=5, $end_key='#', $max_length=3600, $play_beep=TRUE) {
    $is_hangup = self::is_hangup($call_id);
    if($is_hangup) {
      throw new VoipHangupException($call_id);
    }

    $response_url = $this->menu_path . '/' . $call_id . '/process_record';
    $action = url($response_url, array('absolute' => TRUE));
    $method = 'POST';
    // TODO: implement the "transcribe" feature?"
    $command .= '<Record action="' . $action
                   . '" method="' . $method
                   . '" timeout="' . $timeout
                   . '" finishOnKey="' . $end_key
                   . '" maxLength="' . $max_length
                   . '" playBeep="' . $play_beep
                   . '" />';
    $rc = $this->send_command($call_id, $command, TRUE);

// TODO: process record response here

    return $rc;
  }

  function runSay($call_id, $text, $voice=NULL, $language=NULL) {
    $is_hangup = self::is_hangup($call_id);
    if($is_hangup) {
      throw new VoipHangupException($call_id);
    }

    // TODO: implement voice and language support
    $command .= "<Say>$text</Say>";
    $rc = $this->send_command($call_id, $command);
    return $rc;
  }

  /**
   * Protected methods
   */

  protected function send_command($call_id, $command, $get_results=FALSE) {
    $rc = TRUE;

    $output .= '<?xml version="1.0" encoding="UTF-8" ?>';
    $output .= '<Response>';
    $output .= $command;
    $output .= '</Response>';

    drupal_set_header('Content-Type: text/xml; charset=utf-8');

    print $output;

dsm($output, 'sending $output: ');
    if($get_results) {
      $rc = self::get_results($call_id);
    }
dsm($rc, '$rc');
    return $rc;
  }


  /**
   * Methods that handle communication with voiptwilio_call_handler()
   */

  public static function save_results($call_id, $result) {
    $var_name = 'voiptwilio_' . $call_id;
    $db_id = queryable_variables_set($var_name, $result);
    return $db_id;
  }

// TODO: the following function should be protected
  static function get_results($call_id) {
    $result = NULL;
    $var_name = 'voiptwilio_' . $call_id;
    $processing = TRUE;
//    $max_count = 100;
    $max_count = 100 * 10;
    $interval = 10000;  // 0.001 of a second // TODO: this should be defined in terms of max_time / max_count
    for($count=0; ($count < $max_count) && $processing; $count++) {
      usleep($interval);
      $result = queryable_variables_get($var_name);
      if($result) {
        queryable_variables_del($var_name);
        $processing = FALSE;
      }
    }
    
    return $result;
  }

// TODO: the following function should be protected
// TODO: add dependency to voipcall
  static function is_hangup($call_id) {
    $is_hangup = FALSE;
    $var_name = 'voiptwilio_' . $call_id;
    $voipcall = queryable_variables_get($var_name);
    if($voipcall){
      $is_hangup = voipcall_is_hangup($voipcall);
      if($is_hangup) {
        queryable_variables_del($var_name);
      }
    }
    return $is_hangup;
  }
}
