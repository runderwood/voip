<?php

// $Id$

/**
 * @file
 * Implementation of core functionality associated with voip scripts
 */

/**
 * Constants
 */

/**
 * Script command types
 */
define('VOIPCMD_SAY', 'say'); // say a given text
define('VOIPCMD_PLAY', 'play'); // play the specified audio file
define('VOIPCMD_RECORD', 'record');
define('VOIPCMD_HANGUP', 'hangup'); // hangup the call
define('VOIPCMD_GATHER', 'menu'); // present an audio menu with options
define('VOIPCMD_DIAL', 'dial'); // dial the specified number

define('VOIPCMD_LOAD', 'get_script'); // load the specified script

define('VOIPCMD_SET', 'set'); // set the specified script variable

define('VOIPCMD_LABEL', 'label'); // Define a label within the script
define('VOIPCMD_GOTO', 'goto'); // goto a predefined label in the script
define('VOIPCMD_GOTOIF', 'goto_if'); // goto label if condition is TRUE

define('VOIPCMD_GOSUB', 'gosub'); // execute subroutine
define('VOIPCMD_RETURN', 'return'); // return from subroutine


/**
 * Class definitions
 */

        
class VoipScript {
  private $name;
  private $cmd_ids;
  private $cmd_params;
  private $variables;
  private $index;
            
  function __construct($name) {
    $this->name = $name;
    $this->commands = array();
    $this->variables = array();
    $this->index = 0;
  }
        
  private function addCommand($command) {
    $self->commands[] = $command;
  }

  function setVar($name, $value) {
    $this-variables[$name] = $value;
  }

  function getVar($name) {
    return $this->variables['name'];
  }

  function resetIndex() {
    $this->index = 0;
  }

  function getNextCommand() {
    $cmd = $this->commands[$this->index];
    $this->index++;
    return $cmd;
  }
}

class VoipCommand {
  private $id;
  private $params;

  function __construct($id, $params=array()) {
    $this->id = $id;
    $this->params = $params;
  }

  function getId() {
    return $this->id;
  }

  function getParam($name, $default=NULL) {
    $val = is_null($this->params[$name])? $default : $this->params[$name];
    return $val;
  }

  function setParam($name, $value) {
    $this->params[$name] = $value;
  }

}
    

class VoipCmdSay extends VoipCommand {
        
  function __construct($text, $loop=1, $voice=NULL, $language=NULL){
    $params['text'] = $text;      
    $params['loop'] = $loop;      
    $params['voice'] = $voice;      
    $params['language'] = $language;      
    parent::__construct(VOIPCMD_SAY, $params);
  }
        
}
    

class VoipCmdPlay extends VoipCommand {

  function __construct($url, $loop=1){
    $params['url'] = $url;               
    $params['loop'] = $loop;
    parent::__construct(VOIPCMD_PLAY, $params);
  }     
        
}
    
class VoipCmdRecord extends VoipCommand {

  function __construct($timeout=5, $end_key='#', $max_length=3600){
    $params['timeout'] = $timeout;  
    $params['end_key'] = $end_key;
    $params['max_length'] = $max_length;
    parent::__construct(VOIPCMD_RECORD, $params);
  }

}

class VoipCmdDial extends VoipCommand {

  function __construct($number, $timeout=30, $hangup_key='*', $max_length=3600, $caller_id=''){
    $params['number'] = $number;  
    $params['timeout'] = $timeout;  
    $params['hangup_key'] = $hangup_key;  
    $params['max_length'] = $max_length;
    $params['caller_id'] = $caller_id;
    parent::__construct(VOIPCMD_DIAL, $params);
  }

}

class VoipCmdHangup extends VoipCommand {

  function __construct() {
    $params = array();
    parent::__construct(VOIPCMD_HANGUP, $params);
  }

}

class VoipCmdGather extends VoipCommand {

  function __construct($timeout=5, $end_key='#', $num_digits=NULL){
    $params['timeout'] = $timeout;
    $params['end_key'] = $end_key;
    $params['num_digits'] = $num_digits;
    parent::__construct(VOIPCMD_GATHER, $params);
  }

}

