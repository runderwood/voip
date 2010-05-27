<?php
/**
 * @file callblast.script.inc
 *  Scripts provided to voipcall nodes by call blast module. 
 */ 

/**
 * Implementation of hook_voipcall_load_script()
 * 
 * TESTING: play recorded message.
 * 
 */
function callblast_voipcall_load_script($voipcall, $script_name) {
  switch ($script_name) {
    case 'callblast_message_test':
      // play message attached to callblast node
      $script = array();
      $script[] = array(
        VOIPCALL_CMD_ID => VOIPCALL_LABEL,
        VOIPCALL_CMD_BODY => array(
          'label_name' => array(VOIPCALL_VAL => 'start'),
        ),
      );
      $script[] = array(
        VOIPCALL_CMDID => VOIPCALL_PLAY,
        VOIPCALL_CMD_BODY => array(
                       'url' => array(VOIPCALL_VAR => 'message_url'),
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
