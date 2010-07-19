<?php
/**
 * @file callblast.script.inc
 *  Scripts provided to voipcall nodes by call blast module. 
 */ 

/**
 * Implementation of hook_voipcall_load_script()
 * 
 * @param $script_name 
 *  string
 * 
 * @param $vars
 *   $vars = array(
 *     'message_url' => $message_url, 
 *     'forward_to_number' => $forward_to_number,
 *     'options_url' => $options_url,
 *     'one_url' => $one_url,
 *     'two_url' => $two_url,
 *     'three_url' => $three_url,
 *     'four_url' => $four_url,
 *     'five_url' => $five_url,
 *     'six_url' => $six_url,
 *     'seven_url' => $seven_url,
 *     'eight_url' => $eight_url,
 *     'nine_url' => $nine_url,
 *   );  
 */
function callblast_voipscript_load_script($script_name, $vars) {
  $script = new VoipScript($script_name);

  switch ($script_name) {
    case 'callblast_message':
      // start here
      $script->addLabel('start');
      // play main message      
      $text = '';
      $url = $vars['message_url'];
      $timeout = 1;
      $end_key = '';
      $num_digits = 1;
      $script->addGetInput($text, $url, $timeout, $end_key, $num_digits);
        // If user presses a button during the main message, handle the options
        $script->addGotoIf('process_options', "^%input_digits == '" . VoipScript::NO_INPUT . "'");
        // If no options message provided, hangup
        $script->addGotoIf('end_call', "^%input_digits == '!@<options_url>@'");
        // If the call has been answered by a machine, skip the options and hangup
        $script->addGotoIf('end_call', "@<dial_status>@ == '" . VOIPCALL_ANSWERED_MACHINE . "'");

      // play options
      $script->addLabel('options');
      $text = '';
      $url = $vars['options_url'];
      $timeout = 1;
      $end_key = '';
      $num_digits = 1;
      $script->addGetInput($text, $url, $timeout, $end_key, $num_digits);
      // process options
      $script->addLabel('process_options');
      // Go to user's selected option.
      // * replay previous message 
      $script->addGotoIf('start', "^%input_digits == '*'");
      // #, end call
      $script->addGotoIf('end_call', "^%input_digits == '#'");
      // 0, go to call forwarding
      $script->addGotoIf('forward', "(^%input_digits == '0') && @<forward_to_number>@");
      // go to one 
      $script->addGotoIf('one', "(^%input_digits == '1') && @<one_url>@");
      // go to two 
      $script->addGotoIf('two', "(^%input_digits == '2') && @<two_url>@");
      // go to three 
      $script->addGotoIf('three', "(^%input_digits == '3') && @<three_url>@");
      // go to four
      $script->addGotoIf('four', "(^%input_digits == '4') && @<four_url>@");
      // go to five
      $script->addGotoIf('five', "(^%input_digits == '5') && @<five_url>@");
      // go to six
      $script->addGotoIf('six', "(^%input_digits == '6') && @<six_url>@");
      
      // go to seven
      $script[] = array(
        VOIPCALL_CMD_ID => VOIPCALL_GOTOIF,
        VOIPCALL_CMD_BODY => array(
          'label_name' => array(VOIPCALL_VAL => 'seven'),
          // Note: variable names have to be enclosed in between '@<'and '>@'
          'expression' => array(VOIPCALL_EVAL => "@<gather_digits>@ == '7'"),
          'expression' => array(VOIPCALL_EVAL => "(@<gather_digits>@ == '7') && @<seven_url>@"),
        ),
      );

      // go to eight
      $script[] = array(
        VOIPCALL_CMD_ID => VOIPCALL_GOTOIF,
        VOIPCALL_CMD_BODY => array(
          'label_name' => array(VOIPCALL_VAL => 'eight'),
          // Note: variable names have to be enclosed in between '@<'and '>@'
          'expression' => array(VOIPCALL_EVAL => "@<gather_digits>@ == '8'"),
          'expression' => array(VOIPCALL_EVAL => "(@<gather_digits>@ == '8') && @<eight_url>@"),
        ),
      );

      // go to nine
      $script[] = array(
        VOIPCALL_CMD_ID => VOIPCALL_GOTOIF,
        VOIPCALL_CMD_BODY => array(
          'label_name' => array(VOIPCALL_VAL => 'nine'),
          // Note: variable names have to be enclosed in between '@<'and '>@'
          'expression' => array(VOIPCALL_EVAL => "@<gather_digits>@ == '9'"),
          'expression' => array(VOIPCALL_EVAL => "(@<gather_digits>@ == '9') && @<nine_url>@"),
        ),
      );

      // If invalid option selected, go back to the options menu
      $script[] = array(
        VOIPCALL_CMD_ID => VOIPCALL_SAY,
        VOIPCALL_CMD_BODY => array(
          'text' => array(VOIPCALL_VAL => "Invalid option selected. Please try again."),
        ),
      );
      $script[] = array(
        VOIPCALL_CMD_ID => VOIPCALL_GOTO,
        VOIPCALL_CMD_BODY => array(
          'label_name' => array(VOIPCALL_VAL => 'options'),
        ),
      );

      // 0, forward call
      $script[] = array(
        VOIPCALL_CMD_ID => VOIPCALL_LABEL,
        VOIPCALL_CMD_BODY => array(
          'label_name' => array(VOIPCALL_VAL => 'forward'),
        ),
      );
      $script[] = array(
        VOIPCALL_CMD_ID => VOIPCALL_SAY,
        VOIPCALL_CMD_BODY => array(
          'text' => array(VOIPCALL_VAL => "Dialing. Please wait."),
        ),  
      );
      $script[] = array(
        VOIPCALL_CMD_ID => VOIPCALL_DIAL,
        VOIPCALL_CMD_BODY => array(
          'number' => array(VOIPCALL_VAR => 'forward_to_number'),
        ),
      );

      // option/message one
      $script[] = array(
        VOIPCALL_CMD_ID => VOIPCALL_LABEL,
        VOIPCALL_CMD_BODY => array(
          'label_name' => array(VOIPCALL_VAL => 'one'),
        ),
      );
      $script[] = array(
        VOIPCALL_CMD_ID => VOIPCALL_PLAY,
        VOIPCALL_CMD_BODY => array(
                       'url' => array(VOIPCALL_VAR => 'one_url'),
        ),
      );
      // go back to "options" menu
      $script[] = array(
        VOIPCALL_CMD_ID => VOIPCALL_GOTO,
        VOIPCALL_CMD_BODY => array(
          'label_name' => array(VOIPCALL_VAL => 'options'),
        ),
      );

      // option/message two 
      $script[] = array(
        VOIPCALL_CMD_ID => VOIPCALL_LABEL,
        VOIPCALL_CMD_BODY => array(
          'label_name' => array(VOIPCALL_VAL => 'two'),
        ),
      );
      $script[] = array(
        VOIPCALL_CMD_ID => VOIPCALL_PLAY,
        VOIPCALL_CMD_BODY => array(
                       'url' => array(VOIPCALL_VAR => 'two_url'),
        ),
      );
      // go back to "options" menu
      $script[] = array(
        VOIPCALL_CMD_ID => VOIPCALL_GOTO,
        VOIPCALL_CMD_BODY => array(
          'label_name' => array(VOIPCALL_VAL => 'options'),
        ),
      );

      // option/message three 
      $script[] = array(
        VOIPCALL_CMD_ID => VOIPCALL_LABEL,
        VOIPCALL_CMD_BODY => array(
          'label_name' => array(VOIPCALL_VAL => 'three'),
        ),
      );
      $script[] = array(
        VOIPCALL_CMD_ID => VOIPCALL_PLAY,
        VOIPCALL_CMD_BODY => array(
                       'url' => array(VOIPCALL_VAR => 'three_url'),
        ),
      );
      // go back to "options" menu
      $script[] = array(
        VOIPCALL_CMD_ID => VOIPCALL_GOTO,
        VOIPCALL_CMD_BODY => array(
          'label_name' => array(VOIPCALL_VAL => 'options'),
        ),
      );

      // option/message four 
      $script[] = array(
        VOIPCALL_CMD_ID => VOIPCALL_LABEL,
        VOIPCALL_CMD_BODY => array(
          'label_name' => array(VOIPCALL_VAL => 'four'),
        ),
      );
      $script[] = array(
        VOIPCALL_CMD_ID => VOIPCALL_PLAY,
        VOIPCALL_CMD_BODY => array(
                       'url' => array(VOIPCALL_VAR => 'four_url'),
        ),
      );
      // go back to "options" menu
      $script[] = array(
        VOIPCALL_CMD_ID => VOIPCALL_GOTO,
        VOIPCALL_CMD_BODY => array(
          'label_name' => array(VOIPCALL_VAL => 'options'),
        ),
      );

      // option/message five 
      $script[] = array(
        VOIPCALL_CMD_ID => VOIPCALL_LABEL,
        VOIPCALL_CMD_BODY => array(
          'label_name' => array(VOIPCALL_VAL => 'five'),
        ),
      );
      $script[] = array(
        VOIPCALL_CMD_ID => VOIPCALL_PLAY,
        VOIPCALL_CMD_BODY => array(
                       'url' => array(VOIPCALL_VAR => 'five_url'),
        ),
      );
      // go back to "options" menu
      $script[] = array(
        VOIPCALL_CMD_ID => VOIPCALL_GOTO,
        VOIPCALL_CMD_BODY => array(
          'label_name' => array(VOIPCALL_VAL => 'options'),
        ),
      );

      // option/message six
      $script[] = array(
        VOIPCALL_CMD_ID => VOIPCALL_LABEL,
        VOIPCALL_CMD_BODY => array(
          'label_name' => array(VOIPCALL_VAL => 'six'),
        ),
      );
      $script[] = array(
        VOIPCALL_CMD_ID => VOIPCALL_PLAY,
        VOIPCALL_CMD_BODY => array(
                       'url' => array(VOIPCALL_VAR => 'six_url'),
        ),
      );
      // go back to "options" menu
      $script[] = array(
        VOIPCALL_CMD_ID => VOIPCALL_GOTO,
        VOIPCALL_CMD_BODY => array(
          'label_name' => array(VOIPCALL_VAL => 'options'),
        ),
      );

      // option/message seven
      $script[] = array(
        VOIPCALL_CMD_ID => VOIPCALL_LABEL,
        VOIPCALL_CMD_BODY => array(
          'label_name' => array(VOIPCALL_VAL => 'seven'),
        ),
      );
      $script[] = array(
        VOIPCALL_CMD_ID => VOIPCALL_PLAY,
        VOIPCALL_CMD_BODY => array(
                       'url' => array(VOIPCALL_VAR => 'seven_url'),
        ),
      );
      // go back to "options" menu
      $script[] = array(
        VOIPCALL_CMD_ID => VOIPCALL_GOTO,
        VOIPCALL_CMD_BODY => array(
          'label_name' => array(VOIPCALL_VAL => 'options'),
        ),
      );

      // option/message eight
      $script[] = array(
        VOIPCALL_CMD_ID => VOIPCALL_LABEL,
        VOIPCALL_CMD_BODY => array(
          'label_name' => array(VOIPCALL_VAL => 'eight'),
        ),
      );
      $script[] = array(
        VOIPCALL_CMD_ID => VOIPCALL_PLAY,
        VOIPCALL_CMD_BODY => array(
                       'url' => array(VOIPCALL_VAR => 'eight_url'),
        ),
      );
      // go back to "options" menu
      $script[] = array(
        VOIPCALL_CMD_ID => VOIPCALL_GOTO,
        VOIPCALL_CMD_BODY => array(
          'label_name' => array(VOIPCALL_VAL => 'options'),
        ),
      );

      // option/message nine
      $script[] = array(
        VOIPCALL_CMD_ID => VOIPCALL_LABEL,
        VOIPCALL_CMD_BODY => array(
          'label_name' => array(VOIPCALL_VAL => 'nine'),
        ),
      );
      $script[] = array(
        VOIPCALL_CMD_ID => VOIPCALL_PLAY,
        VOIPCALL_CMD_BODY => array(
                       'url' => array(VOIPCALL_VAR => 'nine_url'),
        ),
      );
      // go back to "options" menu
      $script[] = array(
        VOIPCALL_CMD_ID => VOIPCALL_GOTO,
        VOIPCALL_CMD_BODY => array(
          'label_name' => array(VOIPCALL_VAL => 'options'),
        ),
      );

      // end call (hang up)
      $script[] = array(
        VOIPCALL_CMD_ID => VOIPCALL_LABEL,
        VOIPCALL_CMD_BODY => array(
          'label_name' => array(VOIPCALL_VAL => 'end call'),
        ),
      );
      $script[] = array(
        VOIPCALL_CMD_ID => VOIPCALL_HANGUP
      );

    break;
  }

  return $script;
}
