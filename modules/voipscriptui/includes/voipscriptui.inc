<?php

// $Id$

/**
 * @file
 * Implementation of the UI functionality associated with voip scripts
 */


/**
 * Class definitions
 */

        
class VoipScriptUI {

  private static $_script;

  /*
   * Static methods
   */

  public static function setWorkingScript($script){
    self::$_script = $script;
  }
        
  public static function getWorkingScript(){
    return self::$_script;
  }
}
        


/*
 * Supported script commands
 */

function voipscript_dial($number, $timeout=30, $hangup_key='*', $max_length=3600, $caller_id=''){
  $script = VoipScriptUI::getWorkingScript();
  $script->addDial($number, $timeout, $hangup_key, $max_length, $caller_id);
  VoipScriptUI::getWorkingScript($script);
}

  /*
   * Read input from phone keypad
   *
   * out: variable input_digits set to input or to VoipScrip::NO_INPUT
   */
  function addGetInput($text='', $url='', $timeout=5, $end_key='#', $num_digits=NULL) {
    $params['text'] = $text;
    $params['url'] = $url;
    $params['timeout'] = $timeout;
    $params['end_key'] = $end_key;
    $params['num_digits'] = $num_digits;
    $this->commands[] = new VoipCommand('VoipCmdGetInput', $params);
  }

  function addGosub($script_name) {
    $params['script_name'] = $script_name;
    $this->commands[] = new VoipCommand('VoipCmdGosub', $params);
  }

  function addGoto($label_name) {
    $params['label_name'] = $label_name;
    $this->commands[] = new VoipCommand('VoipCmdGoto', $params);
  }

  function addGotoIf($label_name, $condition) {
    $params['label_name'] = $label_name;
    $params['condition'] = $condition;
    $this->commands[] = new VoipCommand('VoipCmdGotoIf', $params);
  }

  function addHangup() {
    $this->commands[] = new VoipCommand('VoipCmdHangup');
  }

  function addLabel($label_name) {
    $params['label_name'] = $label_name;
    $this->commands[] = new VoipCommand('VoipCmdLabel', $params);
  }

  function addLoad($script_name) {
    $params['script_name'] = $script_name;
    $this->commands[] = new VoipCommand('VoipCmdLoad', $params);
  }

  function addPlay($url, $loop=1) {
    $params['url'] = $url;
    $params['loop'] = $loop;
    $this->commands[] = new VoipCommand('VoipCmdPlay', $params);
  }

  function addRecord($timeout=5, $end_key='#', $max_length=3600) {
    $params['timeout'] = $timeout;
    $params['end_key'] = $end_key;
    $params['max_length'] = $max_length;
    $this->commands[] = new VoipCommand('VoipCmdRecord', $params);
  }

  function addReturn() {
    $this->commands[] = new VoipCommand('VoipCmdReturn');
  }

  function addSay($text, $loop=1, $voice=NULL, $language=NULL) {
    $params['text'] = $text;
    $params['loop'] = $loop;
    $params['voice'] = $voice;
    $params['language'] = $language;
    $this->commands[] = new VoipCommand('VoipCmdSay', $params);
  }

  function addSet($var_name, $var_value) {
    $params['var_name'] = $var_name;
    $params['var_value'] = $var_value;
    $this->commands[] = new VoipCommand('VoipCmdSet', $params);
  }

  function addUnset($var_name) {
    $params['var_name'] = $var_name;
    $this->commands[] = new VoipCommand('VoipCmdUnset');
  }

