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
 * Possible status of a call
 */
define('VOIPCALL_ERROR', 'processing error');
define('VOIPCALL_NOT_CALLED', 'not called'); // the call has yet to be called
define('VOIPCALL_DIALING', 'dialing'); // the call is being connected/requested
define('VOIPCALL_NO_ANSWER', 'no answer');
define('VOIPCALL_BUSY', 'busy');
define('VOIPCALL_INVALID', 'invalid number');
define('VOIPCALL_IN_PROGRESS', 'in progress'); // the call is in progress
define('VOIPCALL_ANSWERED_MACHINE', 'answered by machine');
define('VOIPCALL_ANSWERED_HUMAN', 'answered by human');
define('VOIPCALL_MACHINE_HANGUP', 'machine hangup'); // call hangup due to machine pickup
define('VOIPCALL_COMPLETED', 'completed');

/**
 * Script command types
 */
define('VOIPCALL_CMDID', 'command id'); // 'command id' indicator

define('VOIPCALL_SAY', 'say'); // say a given text
define('VOIPCALL_PLAY', 'play'); // play the specified audio file
define('VOIPCALL_RECORD', 'record');
define('VOIPCALL_HANGUP', 'hangup'); // hangup the call
define('VOIPCALL_GATHER', 'menu'); // present an audio menu with options
define('VOIPCALL_DIAL', 'dial'); // dial the specified number

define('VOIPCALL_LOAD', 'get script'); // load the specified script

define('VOIPCALL_SET', 'set'); // set the specified script variable
define('VOIPCALL_LABEL', 'label'); // Define a label within the script
define('VOIPCALL_GOTO', 'goto'); // goto a predefined label in the script
define('VOIPCALL_GOTOIF', 'goto if'); // goto label if condition is TRUE

/**
 * Script parameter types. Depending on the operation, script commands might
 * accept parameters of the following types:
 */
define('VOIPCALL_VAL', 'value');  // A value
define('VOIPCALL_VAR', 'variable'); // The name of a script variable containing the value
define('VOIPVALL_EXEC', 'exec'); // An expression that will result in the value


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
   $script[] = array(VOIPCALL_CMDID => VOIPCALL_SAY,
                     'text' => array(VOIPCALL_VAL => "Hello world!"),
               );
   $script[] = array(VOIPCALL_CMDID => VOIPCALL_SAY,
                     'text' => array(VOIPCALL_VAL => "My caller id is "),
               );
   $script[] = array(VOIPCALL_CMDID => VOIPCALL_SAY,
                     'text' => array(VOIPCALL_VAR => 'call_id'),
               );
   $script[] = array(VOIPCALL_CMDID => VOIPCALL_SAY,
                     'text' => array(VOIPCALL_VAL => "Please leave a message after the beep.  When done, press the start key. "),
               );
   $script[] = array(VOIPCALL_CMDID => VOIPCALL_RECORD,
                     'max_length' => array(VOIPCALL_VAL => '20'),
                     'end_key' => array(VOIPCALL_VAL => '*'),
               );
   $script[] = array(VOIPCALL_CMDID => VOIPCALL_PLAY,
                     'url' => array(VOIPCALL_VAL => "http://api.twilio.com/Cowbell.mp3"),
               );
   $script[] = array(VOIPCALL_CMDID => VOIPCALL_HANGUP);
  }
 return $script;
}

/**
 * Return the value of the given command parameter
 */
function _voipcall_script_get_parameter_value($command, $param_name, $script_variables) {
  $rc = NULL;
  $param = $command[$param_name];
  if($param){
    if($param[VOIPCALL_VAL]) {
      $rc = $param[VOIPCALL_VAL];
    }
    else if($param[VOIPCALL_VAR]) {
      $var_name = $param[VOIPCALL_VAR];
      $rc = $script_variables[$var_name];
    }
    else if($param[VOIPCALL_EXEC]) {
        $function = $param[VOIPCALL_EXEC][0];
        $args =($param[VOIPCALL_EXEC][1])?$param[VOIPCALL_EXEC][1]:array();
        $rc = call_user_func_array($function, $args);
        break;
    }
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
 * @return array with the new context. Check voip_error() for processing errors.
 */
function voipcall_load_script($voipcall, $script_name=NULL) {

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

  $context['status'] = $voipcall['field_voipcall_status'][0]['value'];

  $context['script_name'] = $voipcall['field_voipcall_script_name'][0]['value'];
  $context['script_commands'] = $script_commands;
  $context['script_index'] = $voipcall['field_voipcall_script_index'][0]['value'];
  $context['script_variables'] = unserialize($voipcall['field_voipcall_script_variables'][0]['value']);
  $context['hangup_script'] = $voipcall['field_voipcall_hangup_script'][0]['value'];

  // add call variables to script variables

  $context['script_variables']['uid'] = $voipcall['uid'];

  $context['script_variables']['status'] = $status;

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

  $details = array();

  $details['script_name'] = $context['script_name'];
  $details['script_index'] = $context['script_index'];
  $details['script_variables'] = $context['script_variables'];
  $details['hangup_script'] = $context['hangup_script'];

  // retrieve script variables

  $details['uid'] = $context['script_variables']['uid'];

  $details['status'] = $context['script_variables']['status'];

  $details['call_id'] = $context['script_variables']['call_id'];
  $details['dest_number'] = $context['script_variables']['dest_number'];
  $details['caller_number'] = $context['script_variables']['caller_number'];
  $details['caller_name'] = $context['script_variables']['caller_name'];

  $details['source'] = $context['script_variables']['source'];

  $details['start_time'] = $context['script_variables']['start_time'];
  $details['end_time'] = $context['script_variables']['end_time'];
  $details['duration'] = $context['script_variables']['duration'];

  $details['error_message'] = $context['script_variables']['error_message'];

  $call = voipcall_save($details);
  return $call;
}


/**
 * Internal functions
 */

