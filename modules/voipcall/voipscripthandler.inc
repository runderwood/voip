<?php

// $Id$

/**
 * @file
 * Implementation of the VoipScriptHandler class
 */

foreach (array('voipscript.inc') as $file) {
  require_once(dirname(__FILE__) . DIRECTORY_SEPARATOR . $file);
}


/**
 * Class definitions
 */

        
class VoipScriptHandler {

  private $script; // the current script being processed

  private $variables; // variables processed by the script

  private $stack; // stack of active subroutines 

  private $is_error; // error flag

  private $error_message; // error message


  function __construct($script, $variables=array(), $stack=array()) {
    $this->script = $script;
    $this->variables = $variables;
    $this->stack = $stack;
    $this->is_error = FALSE;
    $this->error_message = NULL;
  }

  function isError(){
    return $this->is_error;
  }

  function getErrorMessage() {
    return $this->error_message;
  }

  function run() {
    $processing = TRUE;
    $processed = array();

    while($processing) {
      $cmd = $script->getNextCommand();
      if(!$cmd) {
        // no more commands to be processed
        $processing = FALSE;
      }
      else {
        // process current command and update script context
        $p = $cmd->process($processing, $this->variables, $this->stack);
        if($cmd->isError()) {
          $this->is_error = TRUE;
          $this->error_message = $cmd->getErrorMessage();
          $processing = FALSE;
        }
        else {
          $processed[] = $p;
        } 
      }
    }

    // now send the processed commands to the voip server
    $this->send($processed);

  }

  function send($processed) {
watchdog('voipscripthandler', 'Sending the following processed commands to the server: '. print_r($processed, true));
  }


}
