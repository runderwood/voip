<?php

// $Id$

/**
 * @file
 * Implementation of the VoipServer-related classes
 */


/**
 * Class definitions
 */
        
abstract class VoipServer {

  /*
   * Constants
   */

  const NO_INPUT='no_input';


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


// Handles errors and signals associated with the Voip Server processing
// TODO: why can't we have something like: class VoipServerException extends VoipException (like in voipscript.inc)?
class VoipServerException extends Exception {}


/* VoipServer classes throws VoipHangupException whenever a call hangup is 
 * detected during the execution of script commands.   
 * The 'message' field of the exception contains the id of the call being hangup
 */
class VoipHangupException extends VoipServerException {}


