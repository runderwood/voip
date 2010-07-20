<?php

// $Id$

/**
 * @file
 * Implementation of the TwilioServer class
 */

foreach (array('voipserver.inc') as $file) {
  require_once(dirname(__FILE__) . DIRECTORY_SEPARATOR . $file);
}


/**
 * Class definitions
 */

class TwilioServer extends VoipServer {

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
   * Implementation of script commands
   */

  function runDial($call_id, $number, $timeout=30, $hangup_key='*', $time_limit=3600, $caller_number=''){
    $response_url = $this->menu_path . '/' . $call_id . '/process_dial';
    $action = url($response_url, array('absolute' => TRUE));
    $method = 'POST';
    // TODO: implement the other features such as multi-dial, url and sendDigits?
    // TODO: implement the hangupOnStar feature?
    $response .= '<Dial action="' . $action
                   . '" method="' . $method
                   . '" timeout="' . $timeout
                   . '" timeLimit="' . $time_limit
                   . '" callerId="' . $caller_number
                   . '" >';
    $response .= '<Number>' . $number . '</Number>';
    $response .= '</Dial>';
    $rc = $this->send_response($response);
    return $rc;
  }

  function runGetInput($call_id, $text='', $url='', $timeout=5, $end_key='#', $num_digits=NULL) {
    $response_url = $this->menu_path . '/' . $call_id . '/process_gather';
    $action = url($response_url, array('absolute' => TRUE));
    $method = 'POST';
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
    $rc = $this->send_response($response);
    return $rc;
  }

  function runHangup($call_id) {
    $response .= '<Hangup/>';
    $rc = $this->send_response($response);
    return $rc;
  }

  function runPlay($call_id, $url, $loop=1) {
    $response .= '<Play loop="' . $loop . '" >' . $url . '</Play>';
    $this->send_response($response);
  }

  function runRecord($call_id, $timeout=5, $end_key='#', $max_length=3600, $play_beep=TRUE) {
    $response_url = $this->menu_path . '/' . $call_id . '/process_record';
    $action = url($response_url, array('absolute' => TRUE));
    $method = 'POST';
    // TODO: implement the "transcribe" feature?"
    $response .= '<Record action="' . $action
                   . '" method="' . $method
                   . '" timeout="' . $timeout
                   . '" finishOnKey="' . $end_key
                   . '" maxLength="' . $max_length
                   . '" playBeep="' . $play_beep
                   . '" />';
    $rc = $this->send_response($response);
    return $rc;
  }

  function runSay($call_id, $text, $voice=NULL, $language=NULL) {
    // TODO: implement voice and language support
    $response .= "<Say>$text</Say>";
    $rc = $this->send_response($response);
    return $rc;
  }

  /**
   * Protected methods
   */

  protected function send_response($response, $get_result=FALSE) {
    $output .= '<?xml version="1.0" encoding="UTF-8" ?>';
    $output .= '<Response>';
    $output .= $response;
    $output .= '</Response>';

    drupal_set_header('Content-Type: text/xml; charset=utf-8');

    print $output;

dsm('sending: ' . $output);
    return TRUE;
  }

  protected static function save_result($call_id, $result) {
  }

  protected static function get_result($call_id, $result) {
  }

}
