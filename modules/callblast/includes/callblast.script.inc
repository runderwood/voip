<?php
/**
 * @file callblast.script.inc
 *  Scripts provided to voipcall nodes by call blast module. 
 */ 

/**
 * Implementation of hook_voipcall_load_script()
 * 
 */
function callblast_voipscript_load_script($script_name) {
  $script = new VoipScript($script_name);

  switch ($script_name) {
    case 'callblast_message':
   
      $script->addLabel('start');
      
     // play the main message/announcement
      $script[] = array(
        VOIPCALL_CMD_ID => VOIPCALL_GATHER,
        VOIPCALL_CMD_BODY => array(
          'url' => array(VOIPCALL_VAR => 'message_url'),
          'num_digits' => array(VOIPCALL_VAL => 1),
          'timeout' => array(VOIPCALL_VAL => 1),
        ),
      );

      // If user presses a button during the main message, handle the options
      $script[] = array(
        VOIPCALL_CMD_ID => VOIPCALL_GOTOIF,
        VOIPCALL_CMD_BODY => array(
          'label_name' => array(VOIPCALL_VAL => 'process_options'),
          // Note: variable names have to be enclosed in between '@<'and '>@'
          'expression' => array(VOIPCALL_EVAL => "@<gather_digits>@ != '" . VOIPCALL_NO_INPUT . "'"),
        ),
      );

//-----------

      // If no options message provided, hangup
      $script[] = array(
        VOIPCALL_CMD_ID => VOIPCALL_GOTOIF,
        VOIPCALL_CMD_BODY => array(
          'label_name' => array(VOIPCALL_VAL => 'end call'),
          // Note: variable names have to be enclosed in between '@<'and '>@'
          'expression' => array(VOIPCALL_EVAL => "!@<options_url>@"),
        ),
      );

      // If the call has been answered by a machine, skip the options and hangup
      $script[] = array(
        VOIPCALL_CMD_ID => VOIPCALL_GOTOIF,
        VOIPCALL_CMD_BODY => array(
          'label_name' => array(VOIPCALL_VAL => 'end call'),
          // Note: variable names have to be enclosed in between '@<'and '>@'
          'expression' => array(VOIPCALL_EVAL => "@<dial_status>@ == '" . VOIPCALL_ANSWERED_MACHINE . "'"),
        ),
      );

      // play options
      $script[] = array(
        VOIPCALL_CMD_ID => VOIPCALL_LABEL,
        VOIPCALL_CMD_BODY => array(
          'label_name' => array(VOIPCALL_VAL => 'options'),
        ),
      );
      $script[] = array(
        VOIPCALL_CMD_ID => VOIPCALL_GATHER,
        VOIPCALL_CMD_BODY => array(
          'url' => array(VOIPCALL_VAR => 'options_url'),
          'num_digits' => array(VOIPCALL_VAL => 1),
          'end_key' => array(VOIPCALL_VAL => ""),
        ),
      );

      // process options
      $script[] = array(
        VOIPCALL_CMD_ID => VOIPCALL_LABEL,
        VOIPCALL_CMD_BODY => array(
          'label_name' => array(VOIPCALL_VAL => 'process_options'),
        ),
      );

      // Go to user's selected option.
      // * replay previous message 
      $script[] = array(
        VOIPCALL_CMD_ID => VOIPCALL_GOTOIF,
        VOIPCALL_CMD_BODY => array(
          'label_name' => array(VOIPCALL_VAL => 'start'),
          // Note: variable names have to be enclosed in between '@<'and '>@'
          'expression' => array(VOIPCALL_EVAL => "@<gather_digits>@ == '*'"),
        ),
      );

      // #, end call
      $script[] = array(
        VOIPCALL_CMD_ID => VOIPCALL_GOTOIF,
        VOIPCALL_CMD_BODY => array(
          'label_name' => array(VOIPCALL_VAL => 'end call'),
          // Note: variable names have to be enclosed in between '@<'and '>@'
          'expression' => array(VOIPCALL_EVAL => "@<gather_digits>@ == '#'"),
        ),
      );

      // 0, go to call forwarding
      $script[] = array(
        VOIPCALL_CMD_ID => VOIPCALL_GOTOIF,
        VOIPCALL_CMD_BODY => array(
          'label_name' => array(VOIPCALL_VAL => 'forward'),
          // Note: variable names have to be enclosed in between '@<'and '>@'
          'expression' => array(VOIPCALL_EVAL => "(@<gather_digits>@ == '0') && @<forward_to_number>@"),
        ),
      );

      // go to one 
      $script[] = array(
        VOIPCALL_CMD_ID => VOIPCALL_GOTOIF,
        VOIPCALL_CMD_BODY => array(
          'label_name' => array(VOIPCALL_VAL => 'one'),
          // Note: variable names have to be enclosed in between '@<'and '>@'
          'expression' => array(VOIPCALL_EVAL => "(@<gather_digits>@ == '1') && @<one_url>@"),
        ),
      );

      // go to two 
      $script[] = array(
        VOIPCALL_CMD_ID => VOIPCALL_GOTOIF,
        VOIPCALL_CMD_BODY => array(
          'label_name' => array(VOIPCALL_VAL => 'two'),
          // Note: variable names have to be enclosed in between '@<'and '>@'
          'expression' => array(VOIPCALL_EVAL => "(@<gather_digits>@ == '2') && @<two_url>@"),
        ),
      );

      // go to three 
      $script[] = array(
        VOIPCALL_CMD_ID => VOIPCALL_GOTOIF,
        VOIPCALL_CMD_BODY => array(
          'label_name' => array(VOIPCALL_VAL => 'three'),
          // Note: variable names have to be enclosed in between '@<'and '>@'
          'expression' => array(VOIPCALL_EVAL => "(@<gather_digits>@ == '3') && @<three_url>@"),
        ),
      );

      // go to four
      $script[] = array(
        VOIPCALL_CMD_ID => VOIPCALL_GOTOIF,
        VOIPCALL_CMD_BODY => array(
          'label_name' => array(VOIPCALL_VAL => 'four'),
          // Note: variable names have to be enclosed in between '@<'and '>@'
          'expression' => array(VOIPCALL_EVAL => "(@<gather_digits>@ == '4') && @<four_url>@"),
        ),
      );

      // go to five
      $script[] = array(
        VOIPCALL_CMD_ID => VOIPCALL_GOTOIF,
        VOIPCALL_CMD_BODY => array(
          'label_name' => array(VOIPCALL_VAL => 'five'),
          // Note: variable names have to be enclosed in between '@<'and '>@'
          'expression' => array(VOIPCALL_EVAL => "@<gather_digits>@ == '5'"),
          'expression' => array(VOIPCALL_EVAL => "(@<gather_digits>@ == '5') && @<five_url>@"),
        ),
      );

      // go to six
      $script[] = array(
        VOIPCALL_CMD_ID => VOIPCALL_GOTOIF,
        VOIPCALL_CMD_BODY => array(
          'label_name' => array(VOIPCALL_VAL => 'six'),
          // Note: variable names have to be enclosed in between '@<'and '>@'
          'expression' => array(VOIPCALL_EVAL => "@<gather_digits>@ == '6'"),
          'expression' => array(VOIPCALL_EVAL => "(@<gather_digits>@ == '6') && @<six_url>@"),
        ),
      );
      
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
