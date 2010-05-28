<?php
/**
 * @file callblast.script.inc
 *  Scripts provided to voipcall nodes by call blast module. 
 */ 

/**
 * Implementation of hook_voipcall_load_script()
 * 
 */
function callblast_voipcall_load_script($voipcall, $script_name) {
  switch ($script_name) {
    case 'callblast_message':
      // play message attached to callblast node
      // message 0
      $script = array();
      $script[] = array(
        VOIPCALL_CMD_ID => VOIPCALL_LABEL,
        VOIPCALL_CMD_BODY => array(
          'label_name' => array(VOIPCALL_VAL => 'start'),
        ),
      );
      $script[] = array(
        VOIPCALL_CMD_ID => VOIPCALL_SET,
        VOIPCALL_CMD_BODY => array(
          'var_name' => array(VOIPCALL_VAL => "previous"),
          'var_value' => array(VOIPCALL_VAR => "start"),
        ),
      );
      $script[] = array(
        VOIPCALL_CMD_ID => VOIPCALL_PLAY,
        VOIPCALL_CMD_BODY => array(
                       'url' => array(VOIPCALL_VAR => 'message_url'),
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

      // Go to user's selected option.
      // * replay previous message 
      $script[] = array(
        VOIPCALL_CMD_ID => VOIPCALL_GOTOIF,
        VOIPCALL_CMD_BODY => array(
          'label_name' => array(VOIPCALL_VAR => 'previous'), // TODO Why doesn't this work?
                                                             // TODO Once fixed, add this to two, three, etc.
          // Note: variable names have to be enclosed in between '@<'and '>@'
          'expression' => array(VOIPCALL_EVAL => "@<gather_digits>@ == '*'"),
        ),
      );
      // # end call
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
          'expression' => array(VOIPCALL_EVAL => "@<gather_digits>@ == '0'"),
        ),
      );

      // go to one 
      $script[] = array(
        VOIPCALL_CMD_ID => VOIPCALL_GOTOIF,
        VOIPCALL_CMD_BODY => array(
          'label_name' => array(VOIPCALL_VAL => 'one'),
          // Note: variable names have to be enclosed in between '@<'and '>@'
          'expression' => array(VOIPCALL_EVAL => "@<gather_digits>@ == '1'"),
        ),
      );
      $script[] = array(
        VOIPCALL_CMD_ID => VOIPCALL_SET,
        VOIPCALL_CMD_BODY => array(
          'var_name' => array(VOIPCALL_VAL => "previous"),
          'var_value' => array(VOIPCALL_VAR => "one"),
        ),
      );
      // go to two 
      $script[] = array(
        VOIPCALL_CMD_ID => VOIPCALL_GOTOIF,
        VOIPCALL_CMD_BODY => array(
          'label_name' => array(VOIPCALL_VAL => 'two'),
          // Note: variable names have to be enclosed in between '@<'and '>@'
          'expression' => array(VOIPCALL_EVAL => "@<gather_digits>@ == '2'"),
        ),
      );
      // go to three 
      $script[] = array(
        VOIPCALL_CMD_ID => VOIPCALL_GOTOIF,
        VOIPCALL_CMD_BODY => array(
          'label_name' => array(VOIPCALL_VAL => 'three'),
          // Note: variable names have to be enclosed in between '@<'and '>@'
          'expression' => array(VOIPCALL_EVAL => "@<gather_digits>@ == '3'"),
        ),
      );
      // go to four
      $script[] = array(
        VOIPCALL_CMD_ID => VOIPCALL_GOTOIF,
        VOIPCALL_CMD_BODY => array(
          'label_name' => array(VOIPCALL_VAL => 'four'),
          // Note: variable names have to be enclosed in between '@<'and '>@'
          'expression' => array(VOIPCALL_EVAL => "@<gather_digits>@ == '4'"),
        ),
      );
      // go to five
      $script[] = array(
        VOIPCALL_CMD_ID => VOIPCALL_GOTOIF,
        VOIPCALL_CMD_BODY => array(
          'label_name' => array(VOIPCALL_VAL => 'five'),
          // Note: variable names have to be enclosed in between '@<'and '>@'
          'expression' => array(VOIPCALL_EVAL => "@<gather_digits>@ == '5'"),
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

    default: 
      // copied from voiptest module. sample script.
      $script = array();
      $script[] = array(
        VOIPCALL_CMD_ID => VOIPCALL_LABEL,
        VOIPCALL_CMD_BODY => array(
          'label_name' => array(VOIPCALL_VAL => 'start'),
        ),
      );
      $script[] = array(
        VOIPCALL_CMD_ID => VOIPCALL_SAY,
        VOIPCALL_CMD_BODY => array(
          'text' => array(VOIPCALL_VAL => t("Welcome to the Voip Drupal platform!")),
        ),
      );
      $script[] = array(
        VOIPCALL_CMD_ID => VOIPCALL_GATHER,
        VOIPCALL_CMD_BODY => array(
          'text' => array(VOIPCALL_VAL => t("To connect with the system developers, press 1.  To hangup, press the pound key.")),
          'num_digits' => array(VOIPCALL_VAL => 1),
          'end_key' => array(VOIPCALL_VAL => ""),
        ),
      );
      $script[] = array(
        VOIPCALL_CMD_ID => VOIPCALL_SAY,
        VOIPCALL_CMD_BODY => array(
          'text' => array(VOIPCALL_VAL => "You pressed: "),
        ),
      );
      $script[] = array(
        VOIPCALL_CMD_ID => VOIPCALL_SAY,
        VOIPCALL_CMD_BODY => array(
          'text' => array(VOIPCALL_VAR => "gather_digits"),
        ),
      );
      $script[] = array(
        VOIPCALL_CMD_ID => VOIPCALL_SET,
        VOIPCALL_CMD_BODY => array(
          'var_name' => array(VOIPCALL_VAL => "user input"),
          'var_value' => array(VOIPCALL_VAR => "gather_digits"),
        ),
      );
      $script[] = array(
        VOIPCALL_CMD_ID => VOIPCALL_SAY,
        VOIPCALL_CMD_BODY => array(
          'text' => array(VOIPCALL_VAL => "The variable 'user input' is now set to "),
        ),
      );
      $script[] = array(
        VOIPCALL_CMD_ID => VOIPCALL_SAY,
        VOIPCALL_CMD_BODY => array(
          'text' => array(VOIPCALL_VAR => "user input"),
        ),
      );
      $script[] = array(
        VOIPCALL_CMD_ID => VOIPCALL_GOTOIF,
        VOIPCALL_CMD_BODY => array(
          'label_name' => array(VOIPCALL_VAL => 'end call'),
          // Note: variable names have to be enclosed in between '@<'and '>@'
          'expression' => array(VOIPCALL_EVAL => "@<gather_digits>@ == '#'"),
        ),
      );
      $script[] = array(
        VOIPCALL_CMD_ID => VOIPCALL_SAY,
        VOIPCALL_CMD_BODY => array(
          'text' => array(VOIPCALL_VAL => "Dialing Leo's cellphone number "),
        ),  
      );
  /****
      $script[] = array(
        VOIPCALL_CMD_ID => VOIPCALL_DIAL,
        VOIPCALL_CMD_BODY => array(
          'number' => array(VOIPCALL_VAL => "6177920995"),
        ),
      );
  ****/
      $script[] = array(
        VOIPCALL_CMD_ID => VOIPCALL_GOTO,
        VOIPCALL_CMD_BODY => array(
          'label_name' => array(VOIPCALL_VAL => 'start'),
        ),
      ); 
      $script[] = array(
        VOIPCALL_CMD_ID => VOIPCALL_LABEL,
        VOIPCALL_CMD_BODY => array(
          'label_name' => array(VOIPCALL_VAL => 'end call'),
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
  /********
      $script[] = array(
        VOIPCALL_CMDID => VOIPCALL_RECORD,
        VOIPCALL_CMD_BODY => array(
                       'max_length' => array(VOIPCALL_VAL => '20'),
                       'end_key' => array(VOIPCALL_VAL => '*'),
        ),
      );
      $script[] = array(
        VOIPCALL_CMDID => VOIPCALL_SAY,
        VOIPCALL_CMD_BODY => array(
                       'text' => array(VOIPCALL_VAL => "You said "),
        ),
      );
      $script[] = array(
        VOIPCALL_CMDID => VOIPCALL_PLAY,
        VOIPCALL_CMD_BODY => array(
                       'url' => array(VOIPCALL_VAR => 'recording_url'),
        ),
      );
  ******/
    break;
  }

  return $script;
}
