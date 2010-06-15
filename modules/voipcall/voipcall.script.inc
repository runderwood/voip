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
 * Return the name of the default script to be used with new incoming calls
 */
function voipcall_get_default_script_name() {
  $script_name = variable_get('voipcall_pickup_script_name', 'voipcall_call_pickup');
  return $script_name;
}

/**
 * Implementation of hook_voipcall_load_script()
 */
function voipcall_voipcall_load_script($voipcall, $script_name) {
  $script = NULL;
  if($script_name == 'voipcall_call_pickup') { 
    $script = array();
    $script[] = array(
      VOIPCALL_CMD_ID => VOIPCALL_SAY,
      VOIPCALL_CMD_BODY => array(
        'text' => array(VOIPCALL_VAL => t("Welcome to the Voip Drupal platform!")),
      ),
    );
    $script[] = array(
      VOIPCALL_CMD_ID => VOIPCALL_SAY,
      VOIPCALL_CMD_BODY => array(
       'text' => array(VOIPCALL_VAL => "Bye bye."),
       ),
    );
    $script[] = array(
      VOIPCALL_CMD_ID => VOIPCALL_HANGUP
    );
  }

  return $script;
}

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
 * Load script context associated with the given call
 *
 * @param array $voipcall instance with the script info
 *
 * @param string $script_name. If NULL, the function will use the call's current script name.
 *
 * @param boolean $reset_index. If TRUE, set the index of the new script to 0.
 *
 * @return array with the new context. Check voip_error() for processing errors.
 */
function voipcall_load_script($voipcall, $script_name=NULL, $reset_index=FALSE) {

  if(!$script_name) {
    $script_name = $voipcall['field_voipcall_script_name'][0]['value'];
  }

  // evoke hook_load_script() giving modules the opportunity to return a
  // custom script and update $voipcall as needed
  $script_commands = array();
  foreach (module_implements('voipcall_load_script') as $module) {
    $function = $module .'_'. 'voipcall_load_script';
    $result = call_user_func($function, &$voipcall, $script_name);
    if(isset($result)) {
      $script_commands = array_merge_recursive($script_commands, $result);
    }
  }

  // now set the context for the script to run with
  $context = array();

  $context['call_status'] = $voipcall['field_voipcall_status'][0]['value'];

  $context['script_name'] = $voipcall['field_voipcall_script_name'][0]['value'];
  $context['script_commands'] = $script_commands;
  if($reset_index) {
    $context['script_index'] = 0;
  }
  else {
    $context['script_index'] = $voipcall['field_voipcall_script_index'][0]['value'];
  }
  $context['script_variables'] = unserialize($voipcall['field_voipcall_script_variables'][0]['value']);
  $context['hangup_script'] = $voipcall['field_voipcall_hangup_script'][0]['value'];

  // add call variables to script variables

  $context['script_variables']['uid'] = $voipcall['uid'];

  $context['script_variables']['call_status'] = $voipcall['field_voipcall_status'][0]['value'];

  $context['script_variables']['call_id'] = $voipcall['field_voipcall_call_id'][0]['value'];
  $context['script_variables']['dest_number'] = $voipcall['field_voipcall_dest_number'][0]['value'];
  $context['script_variables']['caller_number'] = $voipcall['field_voipcall_caller_number'][0]['value'];
  $context['script_variables']['caller_name'] = $voipcall['field_voipcall_caller_name'][0]['value'];

  $context['script_variables']['source'] = $voipcall['field_voipcall_source'][0]['value'];

  $context['script_variables']['start_time'] = $voipcall['field_voipcall_start_time'][0]['value'];
  $context['script_variables']['end_time'] = $voipcall['field_voipcall_end_time'][0]['value'];
  $context['script_variables']['duration'] = $voipcall['field_voipcall_duration'][0]['value'];

  $context['script_variables']['error_message'] = $voipcall['field_voipcall_error_message'][0]['value'];

  return $context;
}


/**
 * Save the script context within the given call
 *
 * @param array $voipcall instance 
 *
 * @param string $context
 *
 * @return array with the updated voipcall info
 */
function voipcall_save_script_context($voipcall, $context) {

  $voipcall['script_name'] = $context['script_name'];
  $voipcall['script_index'] = $context['script_index'];
  $voipcall['script_variables'] = $context['script_variables'];
  $voipcall['hangup_script'] = $context['hangup_script'];

  $call = voipcall_save($voipcall);
  return $call;
}


/**
 * Internal functions
 */

