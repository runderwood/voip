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
  protected $is_error;
  protected $error_message;
            
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

  // Evaluate the given string based on the current value of script variables.
  // Notes regarding the way values are handled: 
  // * placeholders for script variables start with '%'
  // * strings that start with '^' are treated as expressions to be
  //   evaluated
  // * when defining expressions that include string variables, make sure the
  //   variable placeholder is enclosed in " or '.  For instance,
  //      Correct form:   "^echo('The content is ' . '%msg');"
  //      Incorrect form: "^echo('The content is ' . %msg);"
  //   The incorrect form will produce a parser error if the variable msg
  //   contains characters such as a space, math sign, etc...
  function evalString($string) {
    $variables = $this->variables;
    // replace placeholders by the contents of the associated variable name
    // since variables might refer to other variables, run the loop until all
    // replacements are done.
    $pattern = "/%(\w+)/e";
    $replacement = '$variables[\'$1\']';
    do {
      $val = preg_replace($pattern, $replacement, $string, -1, $count);
      $string = $val;
    } while ($count);

    //check if $val contains a function or expression to be processed
    if($val[0] == '^') {
      $expression = substr($val,1);
      $val = eval("return ($expression);");
    }
    return $val;
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

  function addDial($number, $timeout=30, $hangup_key='*', $max_length=3600, $caller_id=''){
    $this->commands[] = new VoipCmdDial($number, $timeout, $hangup_key, $max_length, $caller_id);
  }

  function addGather($text='', $url='', $timeout=5, $end_key='#', $num_digits=NULL) {
    $this->commands[] = new VoipCmdGather($text, $url, $timeout, $end_key, $num_digits);
  }

  function addGosub($script_name) {
    $this->commands[] = new VoipCmdGosub($script_name);
  }

  function addGoto($label_name) {
    $this->commands[] = new VoipCmdGoto($label_name);
  }

  function addGotoIf($label_name, $condition) {
    $this->commands[] = new VoipCmdGotoIf($label_name, $condition);
  }

  function addHangup() {
    $this->commands[] = new VoipCmdHangup();
  }

  function addLabel($label_name) {
    $this->commands[] = new VoipCmdLabel($label_name);
  }

  function addLoad($script_name) {
    $this->commands[] = new VoipCmdLoad($script_name);
  }

  function addPlay($url, $loop=1) {
    $this->commands[] = new VoipCmdPlay($url, $loop);
  }

  function addRecord($timeout=5, $end_key='#', $max_length=3600) {
    $this->commands[] = new VoipCmdRecord($timeout, $end_key, $max_length);
  }

  function addReturn() {
    $this->commands[] = new VoipCmdReturn();
  }

  function addSay($text, $loop=1, $voice=NULL, $language=NULL) {
    $this->commands[] = new VoipCmdSay($text, $loop, $voice, $language);
  }

  function addSet($var_name, $value) {
    $this->commands[] = new VoipCmdSet($var_name, $value);
  }

  /**
   * Protected methods
   */

  // the following function is provided in case subclasses decide to
  // extend the script API to include additional commands
  protected function addCommand($command) {
    $this->commands[] = $command;
  }

}



/**
 * Implementation of command-related classes
 */

class VoipCommand {

  private $id; // command id
  private $params; // command parameters
  protected $is_error;
  protected $error_message;

  function __construct($params=array()) {
    $this->id = get_class($this);
    $this->params = $params;
    $this->is_error = FALSE;
    $this->error_message = NULL;
  }

  function getId() {
    return $this->id;
  }

  function getParam($param_name) {
    return $this->params[$param_name];
  }

  function isError(){
    return $this->is_error;
  }

  function getErrorMessage() {
    return $this->error_message;
  }

  // virtual function to be implemented by subclasses
  function process(&$processing, &$script=NULL, &$stack=array()) {
    $msg = "(" . $script->getName() . ')(' . $this->id .') '
           . "proc: $processing "
           . " stack: " . ($stack?print_r($stack, TRUE):NULL)
           . " params: " . ($this->params?print_r($this->params, TRUE):NULL);
    return $msg;
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

  function process(&$processing, &$script=NULL, &$stack=array()) {
    return parent::process($processing, $script, $stack);
  }
}

class VoipCmdGather extends VoipCommand {

  function __construct($text='', $url='', $timeout=5, $end_key='#', $num_digits=NULL){
    $params['text'] = $text;
    $params['url'] = $url;
    $params['timeout'] = $timeout;
    $params['end_key'] = $end_key;
    $params['num_digits'] = $num_digits;
    parent::__construct($params);
  }

  function process(&$processing, &$script=NULL, &$stack=array()) {
    return parent::process($processing, $script, $stack);
  }
}


class VoipCmdGosub extends VoipCommand {

  function __construct($script_name) {
    $params['script_name'] = $script_name;
    parent::__construct($params);
  }

  function process(&$processing, &$script=NULL, &$stack=array()) {
// TODO: implement gosub
    return parent::process($processing, $script, $stack);
  }
}


class VoipCmdGoto extends VoipCommand {

  function __construct($label_name) {
    $params['label_name'] = $label_name;
    parent::__construct($params);
  }

  function process(&$processing, &$script=NULL, &$stack=array()) {
    $string = $this->params['label_name'];
    $label_name = $script->evalString($string);
    $found = FALSE;
    $i = 0;
    $script_tmp = $script;
    $script_tmp->resetIndex();
    while(!$found) {
      $cmd = $script_tmp->getNextCommand();
      if(!$cmd) {
        // label nor found
        $this->is_error = TRUE;
        $script_name = $script_tmp->getName();
        $this->error_message = "Label '$label_name' not found in script '$script_name': " . print_r($script, true);
        $processing = FALSE;
        $found = TRUE;
      }
      else {
        if($cmd->getId() == 'VoipCmdLabel') {
          $string_tmp = $cmd->getParam('label_name');
          $label_name_tmp = $script->evalString($string_tmp);
          if($label_name_tmp == $label_name) {
            $script = $script_tmp;
            $found = TRUE;
          }
        }
      }
    }

echo (parent::process($processing, $script, $stack));
    return NULL;
  }

}


class VoipCmdGotoIf extends VoipCommand {

  function __construct($label_name, $condition) {
    $params['label_name'] = $label_name;
    $params['condition'] = $condition;
    parent::__construct($params);
  }

  function process(&$processing, &$script=NULL, &$stack=array()) {
// TODO: implement gotoif
    return parent::process($processing, $script, $stack);
  }
}


class VoipCmdHangup extends VoipCommand {

  function __construct() {
    $params = array();
    parent::__construct($params);
  }

  function process(&$processing, &$script=NULL, &$stack=array()) {
    $processing = FALSE;
    return parent::process($processing, $script, $stack);
  }
}



class VoipCmdLabel extends VoipCommand {

  function __construct($label_name) {
    $params['label_name'] = $label_name;
    parent::__construct($params);
  }

  function process(&$processing, &$script=NULL, &$stack=array()) {
    // Labels don't do anything
    return parent::process($processing, $script, $stack);
  }

}


class VoipCmdLoad extends VoipCommand {

  function __construct($script_name) {
    $params['script_name'] = $script_name;
    parent::__construct($params);
  }

  function process(&$processing, &$script=NULL, &$stack=array()) {
// TODO: implement load
    return parent::process($processing, $script, $stack);
  }
}


class VoipCmdPlay extends VoipCommand {

  function __construct($url, $loop=1){
    $params['url'] = $url;               
    $params['loop'] = $loop;
    parent::__construct($params);
  }     
        
  function process(&$processing, &$script=NULL, &$stack=array()) {
    return parent::process($processing, $script, $stack);
  }
}
    
class VoipCmdRecord extends VoipCommand {

  function __construct($timeout=5, $end_key='#', $max_length=3600){
    $params['timeout'] = $timeout;  
    $params['end_key'] = $end_key;
    $params['max_length'] = $max_length;
    parent::__construct($params);
  }

  function process(&$processing, &$script=NULL, &$stack=array()) {
    return parent::process($processing, $script, $stack);
  }
}


class VoipCmdReturn extends VoipCommand {

  function __construct() {
    $params = array();
    parent::__construct($params);
  }

  function process(&$processing, &$script=NULL, &$stack=array()) {
// TODO: implement return
    return parent::process($processing, $script, $stack);
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
        
  function process(&$processing, &$script=NULL, &$stack=array()) {
    return parent::process($processing, $script, $stack);
  }
}
    

class VoipCmdSet extends VoipCommand {

  function __construct($var_name, $value) {
    $params['var_name'] = $var_name;
    $params['value'] = $value;
    parent::__construct($params);
  }

  function process(&$processing, &$script=NULL, &$stack=array()) {
    $string = $this->params['var_name'];
    $var_name = $script->evalString($string);
    $string = $this->params['value'];
    $value = $script->evalString($string);
    $script->setVar($var_name, $value);
    return;
  }

}


