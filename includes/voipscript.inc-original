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
   * Constants
   */

  const NO_INPUT='no_input';


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

  public static function loadScript($script_name, $params = NULL){
    $rc = NULL;
    $script_tmp = new VoipScript($script_name);
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

  function getVar($name) {
    return $this->variables[$name];
  }

  function setVar($name, $value) {
    $this->variables[$name] = $value;
  }


  // Methods used during script execution

  function getNextCommand() {
    $cmd = $this->commands[$this->index];
    $this->index++;
    return $cmd;
  }

  function resetIndex() {
    $this->index = 0;
  }


  // Evaluate the given string based on the current value of script variables.
  function evalString($string) {
    $rc = _voipscript_expand_variables($string, $this->variables);
    return $rc;
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

  function runReturn() {
    $rc = TRUE;
    $script_state = array_pop($this->stack);
    if ($script_state) {
      $this->name = $script_state['name'];
      $this->commands = $script_state['commands'];
      // Note: index is already pointing to the next command to be executed
      $this->index = $script_state['index'];
    }
    else {
      // invalid return command
      $this->is_error = TRUE;
      $this->error_message = "Invalid return command for script '" . $this->name . "': " . print_r($this, true);
    }
    return $rc;
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

  function getParam($param_name, $default=NULL) {
    return $this->params[$param_name]?$this->params[$param_name]:$default;
  }
}
    


/**
 * Internal functions
 */

/**
 * The following function recursively replaces predefined variables from
 * inside the given string.  It was based on code made available at 
 * http://stackoverflow.com/questions/3241070/php-recursive-variable-replacement
 *
 * Notes:
 * - Strings that start with '^' are treated as expressions to be evaluated
 * - Placeholders for script variables start with '%'
 * - This function is smart enough to process variables referenced by
 *   other variables
 * - There is no check against circular inclusion, which might simply lead
 *   to an infinite loop. (Example: $vars['s'] = '%s'; ..) 
 * - When defining expressions that include string variables, make sure the
 *   variable placeholder is enclosed in " or '.  For instance,
 *     Correct form:   "^print_r('The content is ' . '%msg');"
 *     Incorrect form: "^print_r('The content is ' . %msg);"
 *   The incorrect form might produce a parser error if the variable msg
 *   contains characters such as a space, math sign, etc... It might also
 *   produce undesirable results if the variable starts with 0.
 */
function _voipscript_expand_variables($str, $vars) {
  $eval = substr($str, 0, 1) == '^';
  $regex = '/\%(\w+)/e';
  $replacement = "_voipscript_replace_variable(\$1, \$vars, \$eval);" ;
  $res = preg_replace($regex, $replacement ,$str);
  if($eval) {
    ob_start();
    $expr = substr($res, 1);
    if(eval('$res = ' . $expr . ';')===false) {
      ob_end_clean();
//      die('Invalid PHP-Expression: '.$expr);
        watchdog('voipscript', 'Invalid PHP expression: @expr', array('@expr' => $expr), WATCHDOG_ERROR);
    }
    ob_end_clean();
  }
  return $res;
}

function _voipscript_replace_variable($var_name, $vars, $eval) {
  if(isset($vars[$var_name])) {
    $expanded = _voipscript_expand_variables($vars[$var_name], $vars);
    if($eval) {
      // Special handling since $str is going to be evaluated ..
      if(!is_numeric($expanded) || (substr($expanded . '', 0, 1)==='0'
          && strpos($expanded . '', '.')===false)) {
        $expanded = "'$expanded'";
      }
    }
    return $expanded;
  } else {
    // Variable does not exist in $vars array
    if($eval) {
      return 'null';
    }
//  return "$var_name";
    return '';
  }
}

