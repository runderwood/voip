<?php

// $Id$

/**
 * @file
 * Implementation of core functionality associated with voip scripts
 */


/**
 * Class definitions
 */

        
class VoipScript {
  private $name;
  private $variables; 
  private $call_id;
  protected $is_error;
  protected $error_message;
  protected $server;
            
  /*
   * Constants
   */

  const NO_INPUT='no_input';


  /*
   * Constructors
   */

  function __construct($name, $call_id, $variables=array()) {
    $this->name = $name;
    $this->call_id = $call_id;
    $this->variables = $variables;
    $this->is_error = FALSE;
    $this->error_message = NULL;
    // TODO: determine voipserver to be used
$account_sid = 'AC4b3755d057cb03b16bbf471916f9e460';
$auth_token = '8a2d20e93cf5bfa242cce7bdc242bb43';
    $this->server = new VoipTwilioServer($account_sid, $auth_token);
  }

  public static function loadScript($script_name, $call_id, $params = NULL){
// todo: rewrite this function
    $rc = NULL;
    $script_tmp = new VoipScript($script_name, $call_id);
    $script_tmp->runLoad($script_name, $params);
    if(!$script_tmp->isError()) {
      $rc = $script_tmp;
    }
    return $rc;
  }
        

  /*
   * Public methods
   */

  function getName() {
    return $this->name;
  }

  function getCallId() {
    return $this->call_id;
  }

  function getVar($name) {
    return $this->variables[$name];
  }

  function setVar($name, $value) {
    $this->variables[$name] = $value;
  }

  function isError(){
    return $this->is_error;
  }

  function getErrorMessage() {
    return $this->error_message;
  }

  /*
   * Supported script commands
   */

  function dial($number, $timeout=30, $hangup_key='*', $max_length=3600, $caller_id=''){
    $rc = $this->server->runDial($this->call_id, $number, $timeout, $hangup_key, $time_limit, $caller_number);
    if($server->isError()) {
      $this->is_error = TRUE;
      $this->error_message = $server->getErrorMessage();
    }
    return $rc;
  }

  /*
   * Read input from phone keypad
   */
  function getInput($text='', $url='', $timeout=5, $end_key='#', $num_digits=NULL) {
  }

  function hangup() {
    $rc = $this->server->runHangup($this->call_id);
    return $rc;
  }

  function play($url, $loop=1) {
  }

  function record($timeout=5, $end_key='#', $max_length=3600) {
  }

  function say($text, $loop=1, $voice=NULL, $language=NULL) {
  }

  /**
   * @param $script_name
   *  string, name of script to be loaded
   * 
   * @param $params
   *  array, optional parameters 
   *  to be passed to implementations of hook_voipscript_load_script
   */
  function runLoad($script_name, $params = NULL) {
    $rc = TRUE;
    $found = FALSE;

    // Invoke hook_voipscript_load_script() giving modules the 
    // opportunity to return a custom script. 
/*****
    $new_script = module_invoke_all('voipscript_load_script', $script_name, $params);
    if(isset($new_script)) {
      $found = TRUE;
      // loads new name and commands
      $this->name = $new_script->getName();
      $this->commands = $new_script->getCommands();
      $this->index = 0;
      // override variables with the same name and preserve the others
      $this->variables = array_merge($this->variables, $new_script->getVariables());
      // preserve the stack as is
        // $this->stack = array();
      $this->is_error = FALSE;
      // scripts must have unique names. end here.
    }
// *******/

// todo Consider replacing hook invocation below with conventional hook invocation above. As it is now, module_invoke_all() isn't working right. I'm not sure why yet.

    // evoke hook_voipscript_load_script() giving modules the opportunity to return a
    // custom script
    foreach (module_implements('voipscript_load_script') as $module) {
      $function = $module .'_'. 'voipscript_load_script';
      $new_script = call_user_func($function, $script_name, $params);
      if(isset($new_script)) {
        $found = TRUE;
        // loads new name and commands
        $this->name = $new_script->getName();
        $this->commands = $new_script->getCommands();
        $this->index = 0;
        // override variables with the same name and preserve the others
        $this->variables = array_merge($this->variables, $new_script->getVariables());
        // preserve the stack as is
          // $this->stack = array();
        $this->is_error = FALSE;
        // scripts must have unique names. exit the foreach
        break;
      }
    }
// ************/

    if(!$found) {
      // no script found with the given name
      $rc = FALSE;
      $this->is_error = TRUE;
      $this->error_message = "Script '$script_name' not found for script '" . $this->name . "': " . print_r($this->commands, TRUE);
    }

    return $rc;
  } 

}


