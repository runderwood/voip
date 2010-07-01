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
define('VOIPCALL_CMD_ID', 'command_id'); // 'command id' indicator
define('VOIPCALL_CMD_BODY', 'command_body'); // 'command id' indicator

define('VOIPCALL_SAY', 'say'); // say a given text
define('VOIPCALL_PLAY', 'play'); // play the specified audio file
define('VOIPCALL_RECORD', 'record');
define('VOIPCALL_HANGUP', 'hangup'); // hangup the call
define('VOIPCALL_GATHER', 'menu'); // present an audio menu with options
define('VOIPCALL_DIAL', 'dial'); // dial the specified number

define('VOIPCALL_LOAD', 'get_script'); // load the specified script

define('VOIPCALL_SET', 'set'); // set the specified script variable
define('VOIPCALL_LABEL', 'label'); // Define a label within the script
define('VOIPCALL_GOTO', 'goto'); // goto a predefined label in the script
define('VOIPCALL_GOTOIF', 'goto_if'); // goto label if condition is TRUE

/**
 * Script parameter types. Depending on the operation, script commands might
 * accept parameters of the following types:
 */
define('VOIPCALL_VAL', 'value');  // A value
define('VOIPCALL_VAR', 'variable'); // The name of a script variable containing the value
define('VOIPVALL_EXEC', 'exec'); // An expression that will result in the value

/**
 * Other script-related constants
 */
define('VOIPCALL_NO_INPUT', 'no_input'); // Used to report timeouts and empty inputs


/**
 * Public functions
 */

/**
 * Return the value of the given command parameter
 */
function voipcall_script_get_parameter_value($command, $param_name, $variables, $default=NULL) {
  $rc = NULL;
  $param = $command[VOIPCALL_CMD_BODY][$param_name];
  if($param[VOIPCALL_VAL]) {
    $rc = $param[VOIPCALL_VAL];
  }
  else if($param[VOIPCALL_VAR]) {
    $var_name = $param[VOIPCALL_VAR];
    $rc = $variables[$var_name];
  }
  else if($param[VOIPCALL_EVAL]) {
    // evaluate expression
    $expression = $param[VOIPCALL_EVAL];
    // replace placeholders by associated variable name
    $params = array('@<' => '$variables[', '>@' => ']');
    $expression_tmp = strtr($expression, $params);
    $rc = eval("return ($expression_tmp);");
watchdog('voipcal_script_get_param', "expression: $expression, expression_tmp: $expression_tmp, rc: $rc, variables: " . print_r($variables,true));
  }
  else {
    $rc = $default;
  }
  return $rc;
}


/**
 * Internal functions
 */

