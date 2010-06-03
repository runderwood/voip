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
define('VOIPCMD_DIAL', 'dial'); // dial the specified number
define('VOIPCMD_GATHER', 'menu'); // present an audio menu with options
define('VOIPCMD_HANGUP', 'hangup'); // hangup the call
define('VOIPCMD_PLAY', 'play'); // play the specified audio file
define('VOIPCMD_RECORD', 'record');
define('VOIPCMD_SAY', 'say'); // say a given text

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
  private $commands;
  private $variables;
  private $index;
            
  function __construct($name) {
    $this->name = $name;
    $this->commands = array();
    $this->variables = array();
    $this->index = 0;
  }
        
  function addCommand($command) {
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

  /*
   * Convenience functions
   */

  function addHangup() {
    $self->commands[] = new VoipCmdHangup();
  }

  function addLoad($script_name) {
    $self->commands[] = new VoipCmdLoad($script_name);
  }

  function addSet($var_name, $value) {
    $self->commands[] = new VoipCmdSet($var_name, $value);
  }

  function addLabel($label_name) {
    $self->commands[] = new VoipCmdLabel($label_name);
  }

  function addGoto($label_name) {
    $self->commands[] = new VoipCmdGoto($label_name);
  }

  function addGotoIf($label_name, $condition) {
    $self->commands[] = new VoipCmdGotoIf($label_name, $condition);
  }

  function addGosub($script_name) {
    $self->commands[] = new VoipCmdGosub($script_name);
  }

  function addReturn() {
    $self->commands[] = new VoipCmdReturn();
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
class VoipCmdGather extends VoipCommand {

  function __construct($timeout=5, $end_key='#', $num_digits=NULL){
    $params['timeout'] = $timeout;
    $params['end_key'] = $end_key;
    $params['num_digits'] = $num_digits;
    parent::__construct(VOIPCMD_GATHER, $params);
  }

}


class VoipCmdHangup extends VoipCommand {

  function __construct() {
    $params = array();
    parent::__construct(VOIPCMD_HANGUP, $params);
  }

}



class VoipCmdLoad extends VoipCommand {

  function __construct($script_name) {
    $params['script_name'] = $script_name;
    parent::__construct(VOIPCMD_LOAD, $params);
  }

}


class VoipCmdSet extends VoipCommand {

  function __construct($var_name, $value) {
    $params['var_name'] = $var_name;
    $params['value'] = $value;
    parent::__construct(VOIPCMD_SET, $params);
  }

}


class VoipCmdLabel extends VoipCommand {

  function __construct($label_name) {
    $params['label_name'] = $label_name;
    parent::__construct(VOIPCMD_LABEL, $params);
  }

}


class VoipCmdGoto extends VoipCommand {

  function __construct($label_name) {
    $params['label_name'] = $label_name;
    parent::__construct(VOIPCMD_GOTO, $params);
  }

}


class VoipCmdGotoIf extends VoipCommand {

  function __construct($label_name, $condition) {
    $params['label_name'] = $label_name;
    $params['condition'] = $condition;
    parent::__construct(VOIPCMD_GOTOIF, $params);
  }

}


class VoipCmdGosub extends VoipCommand {

  function __construct($script_name) {
    $params['script_name'] = $script_name;
    parent::__construct(VOIPCMD_GOSUB, $params);
  }

}


class VoipCmdReturn extends VoipCommand {

  function __construct($) {
    $params = array();
    parent::__construct(VOIPCMD_RETURN, $params);
  }

}

