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
  private $commands;
  private $index;
  private $is_error;
  private $error_message;
            
  function __construct($name) {
    $this->name = $name;
    $this->commands = array();
    $this->index = 0;
    $this->is_error = FALSE;
    $this->error_message = NULL;
  }
        
  function getName() {
    return $this->name;
  }

  function getNextCommand() {
    $cmd = $this->commands[$this->index];
    $this->index++;
    return $cmd;
  }

  function resetIndex() {
    $this->index = 0;
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

  // the following function is provided in case subclasses decide to
  // extend the script API to include additional commands
  protected function addCommand($command) {
    $this->commands[] = $command;
  }

}


class VoipCommand {

  private $id; // command id
  private $params; // command parameters
  private $is_error;
  private $error_message;

  function __construct($params=array()) {
    $this->id = get_class($this);
    $this->params = $params;
    $this->is_error = FALSE;
    $this->error_message = NULL;
  }

  function getId() {
    return $this->id;
  }

  // return the value associated with the given parameter
  function getParam($param_name, $script_variables) {
// TODO:    $val = ;
    return $val;
  }

  function isError(){
    return $this->is_error;
  }

  function getErrorMessage() {
    return $this->error_message;
  }

  // virtual function to be implemented by subclasses
  function process(&$processing, &$script_variables=array(), &$script_stack=array()) {
    $msg = "Command $this->id being processed with the following arguments: processing: $processing, variables: " . print_r($variables, TRUE) . ", and script_stack: " . print_r($script_stack, TRUE);
    return $msg;
  }

}
    

class VoipCmdSay extends VoipCommand {
        
  function __construct($text, $loop=1, $voice=NULL, $language=NULL){
    $params['text'] = $text;      
    $params['loop'] = $loop;      
    $params['voice'] = $voice;      
    $params['language'] = $language;      
    parent::__construct($params);
  }
        
}
    

class VoipCmdPlay extends VoipCommand {

  function __construct($url, $loop=1){
    $params['url'] = $url;               
    $params['loop'] = $loop;
    parent::__construct($params);
  }     
        
}
    
class VoipCmdRecord extends VoipCommand {

  function __construct($timeout=5, $end_key='#', $max_length=3600){
    $params['timeout'] = $timeout;  
    $params['end_key'] = $end_key;
    $params['max_length'] = $max_length;
    parent::__construct($params);
  }

}

class VoipCmdDial extends VoipCommand {

  function __construct($number, $timeout=30, $hangup_key='*', $max_length=3600, $caller_id=''){
    $params['number'] = $number;  
    $params['timeout'] = $timeout;  
    $params['hangup_key'] = $hangup_key;  
    $params['max_length'] = $max_length;
    $params['caller_id'] = $caller_id;
    parent::__construct($params);
  }

}
class VoipCmdGather extends VoipCommand {

  function __construct($timeout=5, $end_key='#', $num_digits=NULL){
    $params['timeout'] = $timeout;
    $params['end_key'] = $end_key;
    $params['num_digits'] = $num_digits;
    parent::__construct($params);
  }

}


class VoipCmdHangup extends VoipCommand {

  function __construct() {
    $params = array();
    parent::__construct($params);
  }

  function process(&$processing, &$script_variables=array(), &$script_stack=array()) {
    $processing = TRUE;
    return parent::process($processing, $script_variables, $script_stack);
  }
}



class VoipCmdLoad extends VoipCommand {

  function __construct($script_name) {
    $params['script_name'] = $script_name;
    parent::__construct($params);
  }

  function process(&$processing, &$script_variables=array(), &$script_stack=array()) {
// TODO: implement load
    return parent::process($processing, $script_variables, $script_stack);
  }
}


class VoipCmdSet extends VoipCommand {

  function __construct($var_name, $value) {
    $params['var_name'] = $var_name;
    $params['value'] = $value;
    parent::__construct($params);
  }

  function process(&$processing, &$script_variables=array(), &$script_stack=array()) {
// TODO: implement set
    return parent::process($processing, $script_variables, $script_stack);
  }

}


class VoipCmdLabel extends VoipCommand {

  function __construct($label_name) {
    $params['label_name'] = $label_name;
    parent::__construct($params);
  }

  function process(&$processing, &$script_variables=array(), &$script_stack=array()) {
    // Labels don't do nothing
    return;
  }

}


class VoipCmdGoto extends VoipCommand {

  function __construct($label_name) {
    $params['label_name'] = $label_name;
    parent::__construct($params);
  }

  function process(&$processing, &$script_variables=array(), &$script_stack=array()) {
// TODO: implement goto
    return parent::process($processing, $script_variables, $script_stack);
  }

}


class VoipCmdGotoIf extends VoipCommand {

  function __construct($label_name, $condition) {
    $params['label_name'] = $label_name;
    $params['condition'] = $condition;
    parent::__construct($params);
  }

  function process(&$processing, &$script_variables=array(), &$script_stack=array()) {
// TODO: implement gotoif
    return parent::process($processing, $script_variables, $script_stack);
  }
}


class VoipCmdGosub extends VoipCommand {

  function __construct($script_name) {
    $params['script_name'] = $script_name;
    parent::__construct($params);
  }

  function process(&$processing, &$script_variables=array(), &$script_stack=array()) {
// TODO: implement gosub
    return parent::process($processing, $script_variables, $script_stack);
  }
}


class VoipCmdReturn extends VoipCommand {

  function __construct() {
    $params = array();
    parent::__construct($params);
  }

  function process(&$processing, &$script_variables=array(), &$script_stack=array()) {
// TODO: implement return
    return parent::process($processing, $script_variables, $script_stack);
  }
}



class VoipScriptHandler {

  private $script; // the current script being processed

  private $variables; // variables processed by the script

  private $stack; // stack of active subroutines 

  private $is_error; // error flag

  private $error_message; // error message


  function __construct($script, $variables) {
    $this->script = $script;
    $this->variables = $variables;
    $this->stack = array();
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
      if($this->index >= count($this->commands)) {
        // no more commands to be processed
        // TODO: either stop processing or go back to subroutine
        $processing = FALSE;
      }
      else {
        // process current command
        $cmd = $this->commands[$this->index];
        $p = $cmd->process($processing, $this->variables, $this->call_queue);
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

   // now execute the processed commands

  }

  private function setVar($name, $value) {
    $this-variables[$name] = $value;
  }

  private function getVar($name) {
    return $this->variables['name'];
  }
}
