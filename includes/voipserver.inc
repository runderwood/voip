<?php

// $Id$

/**
 * @file
 * Implementation of the VoipServer-related classes
 */

foreach (array('voipscript.inc') as $file) {
  require_once(dirname(__FILE__) . DIRECTORY_SEPARATOR . $file);
}




/**
 * Class definitions
 */
        
abstract class VoipServer {

  protected $is_error = FALSE; // error flag

  protected $error_message = NULL; // error message


  /*
   * Error-related methods
   */

  function isError(){
    return $this->is_error;
  }

  function getErrorMessage() {
    return $this->error_message;
  }


  /*
   * API methods
   */

  abstract function ping();


  /*
   * Implementation of script commands
   */

// TODO: move $call_id to a global variable?

  abstract function runDial($call_id, $number, $timeout=30, $hangup_key='*', $time_limit=3600, $caller_number='');
   

  abstract function runGetInput($call_id, $text='', $url='', $timeout=5, $end_key='#', $num_digits=NULL);

  abstract function runHangup($call_id);

  abstract function runPlay($call_id, $url, $loop=1);

  abstract function runRecord($call_id, $timeout=5, $end_key='#', $max_length=3600, $play_beep=TRUE);

  abstract function runSay($call_id, $text, $voice=NULL, $language=NULL);

}


/* VoipServer classes throws VoipHangupException whenever a call hangup is 
 * detected during the execution of script commands.   
 * The 'message' field of the exception contains the id of the call being hangup
 */
class VoipHangupException extends Exception {}


