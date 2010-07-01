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
  private $index; // index of the current command
  private $variables; 
  private $stack; // stack of active subroutines 
  protected $is_error;
  protected $error_message;
            
  /*
   * Constructors
   */

  function __construct($name, $variables=array()) {
    $this->name = $name;
    $this->commands = array();
    $this->index = 0;
    $this->variables = $variables;
    $this->stack = array();
    $this->is_error = FALSE;
    $this->error_message = NULL;
  }

  public static function loadScript($script_name){
    $rc = NULL;
    $script_tmp = new VoipScript($script_name);
    $script_tmp->runLoad($script_name);
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

  function getVar($name) {
    return $this->variables[$name];
  }

  function setVar($name, $value) {
    $this->variables[$name] = $value;
  }


  // Methods used during the script execution

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
    do {
      // if the string is to be evaluated, make sure NULL variables are replaced
      // by the text 'NULL' (rather than ''). That will help prevent eventual
      // syntax errors in the evaluation.
      if($string[0] == '^') {
        $replacement = '($this->variables[\'$1\']?$this->variables[\'$1\']:"NULL")';
      }
      else {
        $replacement = '$this->variables[\'$1\']';
      }
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
    $params['number'] = $number;
    $params['timeout'] = $timeout;
    $params['hangup_key'] = $hangup_key;
    $params['max_length'] = $max_length;
    $params['caller_id'] = $caller_id;
    $this->commands[] = new VoipCommand('VoipCmdDial', $params);
  }

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

  function addSet($var_name, $value) {
    $params['var_name'] = $var_name;
    $params['value'] = $value;
    $this->commands[] = new VoipCommand('VoipCmdSet', $params);
  }


  /*
   * Execution of basic script commands that are not dependent on the voip server
   */

  function runGosub($script_name) {
    $script_state = array('name' => $this->name,
                          'commands' => $this->commands,
                          'index' => $this->index);
    $this->stack[] = $script_state;
    $rc =  $this->runLoad($script_name);
    return $rc;
  }

  function runGoto($label_name) {
    $rc = TRUE;
    $label_name = $this->evalString($label_name);
    $found = FALSE;
    $i = 0;
    while(!$found) {
      $cmd = $this->commands[$i];
      if(!$cmd) {
        // label nor found
        $this->is_error = TRUE;
        $script_name = $this->name;
        $this->error_message = "Label '$label_name' not found in script '$script_name': " . print_r($this, true);
        $rc = FALSE;
        $found = TRUE;
      }
      else {
        if($cmd->getId() == 'VoipCmdLabel') {
          $string_tmp = $cmd->getParam('label_name');
          $label_name_tmp = $this->evalString($string_tmp);
          if(strcasecmp($label_name_tmp, $label_name) == 0) {
            $this->index = $i;
            $found = TRUE;
          }
        }
        // move to the next command in the script
        $i++;
      }
    }
    return $rc;
  }

  function runGotoIf($label_name, $condition) {
    $rc = TRUE;
    $condition = $this->evalString($condition);
    if($condition) {
      $rc = $this->runGoto($label_name);
    }
    return $rc;
  }

  function runLoad($script_name) {
    $rc = TRUE;
    $found = FALSE;

    // evoke hook_voipscript_load_script() giving modules the opportunity to return a
    // custom script
    foreach (module_implements('voipscript_load_script') as $module) {
      $function = $module .'_'. 'voipscript_load_script';
      $new_script = call_user_func($function, $script_name);
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

    if(!$found) {
      // no script found with the given name
      $rc = FALSE;
      $this->is_error = TRUE;
      $this->error_message = "Script '$script_name' not found for script '" . $this->name . "': " . print_r($this->commands, TRUE);
    }

    return $rc;
  } 

  function runReturn() {
    $rc = TRUE;
    $script_state = array_pop($this->stack);
    if ($script_state) {
      $this->name = $script_state['name'];
      $this->commands = $script_state['commands'];
      $this->index = $script_state['index'];
      // get ready to execute the next command in the script
      $this->index++;
    }
    else {
      // invalid return command
      $this->is_error = TRUE;
      $this->error_message = "Invalid return command for script '" . $this->name . "': " . print_r($this, true);
    }
    return $rc;
  }

  function runSet($var_name, $value) {
    $var_name = $this->evalString($var_name);
    $value = $this->evalString($value);
    $this->variables[$var_name] = $value;
    return;
  }


  /**
   * Protected methods
   */

  protected function getCommands() {
    return $this->commands;
  }

  protected function getVariables() {
    return $this->variables;
  }

  // the following function is provided in case there is a need to
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

  function __construct($id, $params=array()) {
    $this->id = $id;
    $this->params = $params;
  }

  function getId() {
    return $this->id;
  }

  function getParam($param_name) {
    return $this->params[$param_name];
  }
}
    

