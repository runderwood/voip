<?php

// $Id$

/**
 * @file
 * Implementation of core functionality associated with voip scripts
 */


foreach (array('voipserver.inc') as $file) {
  require_once(dirname(__FILE__) . DIRECTORY_SEPARATOR . $file);
}


/**
 * Class definitions
 */

        
class VoipScript {
  protected $name;
  protected $call_id;
  protected $server;
            

  /*
   * Constructors
   */

  function __construct($name, $call_id) {
    $this->name = $name;
    $this->call_id = $call_id;
// TODO: determine voipserver to be used
$account_sid = 'AC4b3755d057cb03b16bbf471916f9e460';
$auth_token = '8a2d20e93cf5bfa242cce7bdc242bb43';
$this->server = new VoipTwilioServer($account_sid, $auth_token);
  }

  /*
   * Public static methods
   */

  /**
   * Invoke the desired voip script
   *
   * @param $script_name
   *  string, name of script. should match a <script_name>_voipscript() function
   *
   * @param $call_id
   *  string, the id of the current call
   * 
   * @param $params
   *  array, optional parameters 
   *  to be passed to implementations of hook_voipscript_load_script
   */
  public static function runScript($script_name, $call_id, $params = NULL){
    $rc = NULL;
    $function = $script_name . '_voipscript';
    $exists = function_exists($function);
    if(!$exists) {
      $msg = t('Invalid voip script name: @name', array('@name' => $script_name));
      throw new VoipScriptException($msg);
    }
    else {
      $rc = call_user_func($function, $call_id, $params);
    }
    return $rc;
  }
        

  /*
   * Public non-static methods
   */

  function getName() {
    return $this->name;
  }

  function getCallId() {
    return $this->call_id;
  }

  /*
   * Supported script commands
   */

  function dial($number, $timeout=30, $hangup_key='*', $max_length=3600, $caller_id=''){
    try {
      $rc = $this->server->runDial($this->call_id, $number, $timeout, $hangup_key, $time_limit, $caller_number);
    }
    catch(VoipServerException $e) {
      throw new VoipScriptException($e->errorMessage());
    }
    return $rc;
  }

  /*
   * Read input from phone keypad
   */
  function getInput($text='', $url='', $timeout=5, $end_key='#', $num_digits=NULL) {
    try {
      $rc = $this->server->runGetInput($text, $url, $timeout, $end_key, $num_digits);
    }
    catch(VoipServerException $e) {
      throw new VoipScriptException($e->errorMessage());
    }
    return $rc;
  }

  function hangup() {
    try {
      $rc = $this->server->runHangup($this->call_id);
    }
    catch(VoipServerException $e) {
      throw new VoipScriptException($e->errorMessage());
    }
    return $rc;
  }

  function play($url, $loop=1) {
    try {
      $rc = $this->server->runPlay($this->call_id, $url, $loop);
    }
    catch(VoipServerException $e) {
      throw new VoipScriptException($e->errorMessage());
    }
    return $rc;
  }

  function record($timeout=5, $end_key='#', $max_length=3600) {
    try {
      $rc = $this->server->runRecord($this->call_id, $timeout, $end_key, $max_length);
    }
    catch(VoipServerException $e) {
      throw new VoipScriptException($e->errorMessage());
    }
    return $rc;
  }

  function say($text, $loop=1, $voice=NULL, $language=NULL) {
    try {
      $rc = $this->server->runSay($this->call_id, $text, $loop, $voice, $language);
    }
    catch(VoipServerException $e) {
      throw new VoipScriptException($e->errorMessage());
    }
    return $rc;
  }


}

// Handles processing errors associated with the execution of voip scripts
class VoipScriptException extends VoipException {}

