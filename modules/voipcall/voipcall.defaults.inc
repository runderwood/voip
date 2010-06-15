<?php

/**
 * Helper to implementation of hook_content_default_fields().
 */
function _voipcall_content_default_fields() {
  $fields = array();

  // Exported field: field_voipcall_call_id
  $fields[] = array(
    'field_name' => 'field_voipcall_call_id',
    'type_name' => 'voipcall',
    'display_settings' => array(
      'label' => array(
        'format' => 'above',
        'exclude' => 0,
      ),
      'teaser' => array(
        'format' => 'default',
        'exclude' => 0,
      ),
      'full' => array(
        'format' => 'default',
        'exclude' => 0,
      ),
      '4' => array(
        'format' => 'default',
        'exclude' => 0,
      ),
    ),
    'widget_active' => '1',
    'type' => 'text',
    'required' => '0',
    'multiple' => '0',
    'module' => 'text',
    'active' => '1',
    'text_processing' => '0',
    'max_length' => '',
    'allowed_values' => '',
    'allowed_values_php' => '',
    'widget' => array(
      'rows' => 5,
      'size' => '60',
      'default_value' => array(
        '0' => array(
          'value' => '',
          '_error_element' => 'default_value_widget][field_voipcall_call_id][0][value',
        ),
      ),
      'default_value_php' => NULL,
      'label' => 'Call Id',
      'weight' => '-4',
      'description' => 'Unique identifier associated with the call.',
      'type' => 'text_textfield',
      'module' => 'text',
    ),
  );

  // Exported field: field_voipcall_caller_name
  $fields[] = array(
    'field_name' => 'field_voipcall_caller_name',
    'type_name' => 'voipcall',
    'display_settings' => array(
      'label' => array(
        'format' => 'above',
        'exclude' => 0,
      ),
      'teaser' => array(
        'format' => 'default',
        'exclude' => 0,
      ),
      'full' => array(
        'format' => 'default',
        'exclude' => 0,
      ),
      '4' => array(
        'format' => 'default',
        'exclude' => 0,
      ),
      'token' => array(
        'format' => 'default',
        'exclude' => 0,
      ),
    ),
    'widget_active' => '1',
    'type' => 'text',
    'required' => '0',
    'multiple' => '0',
    'module' => 'text',
    'active' => '1',
    'text_processing' => '0',
    'max_length' => '',
    'allowed_values' => '',
    'allowed_values_php' => '',
    'widget' => array(
      'rows' => 5,
      'size' => '60',
      'default_value' => array(
        '0' => array(
          'value' => '',
          '_error_element' => 'default_value_widget][field_voipcall_caller_name][0][value',
        ),
      ),
      'default_value_php' => NULL,
      'label' => 'Caller Name',
      'weight' => 0,
      'description' => 'The name of the caller making the call.',
      'type' => 'text_textfield',
      'module' => 'text',
    ),
  );

  // Exported field: field_voipcall_caller_number
  $fields[] = array(
    'field_name' => 'field_voipcall_caller_number',
    'type_name' => 'voipcall',
    'display_settings' => array(
      'label' => array(
        'format' => 'above',
        'exclude' => 0,
      ),
      'teaser' => array(
        'format' => 'default',
        'exclude' => 0,
      ),
      'full' => array(
        'format' => 'default',
        'exclude' => 0,
      ),
      '4' => array(
        'format' => 'default',
        'exclude' => 0,
      ),
    ),
    'widget_active' => '1',
    'type' => 'text',
    'required' => '0',
    'multiple' => '0',
    'module' => 'text',
    'active' => '1',
    'text_processing' => '0',
    'max_length' => '',
    'allowed_values' => '',
    'allowed_values_php' => '',
    'widget' => array(
      'rows' => 5,
      'size' => '60',
      'default_value' => array(
        '0' => array(
          'value' => '',
          '_error_element' => 'default_value_widget][field_voipcall_caller_number][0][value',
        ),
      ),
      'default_value_php' => NULL,
      'label' => 'Caller Number',
      'weight' => '-2',
      'description' => '',
      'type' => 'text_textfield',
      'module' => 'text',
    ),
  );

  // Exported field: field_voipcall_dest_number
  $fields[] = array(
    'field_name' => 'field_voipcall_dest_number',
    'type_name' => 'voipcall',
    'display_settings' => array(
      'label' => array(
        'format' => 'above',
        'exclude' => 0,
      ),
      'teaser' => array(
        'format' => 'default',
        'exclude' => 0,
      ),
      'full' => array(
        'format' => 'default',
        'exclude' => 0,
      ),
      '4' => array(
        'format' => 'default',
        'exclude' => 0,
      ),
    ),
    'widget_active' => '1',
    'type' => 'text',
    'required' => '0',
    'multiple' => '0',
    'module' => 'text',
    'active' => '1',
    'text_processing' => '0',
    'max_length' => '',
    'allowed_values' => '',
    'allowed_values_php' => '',
    'widget' => array(
      'rows' => 5,
      'size' => '60',
      'default_value' => array(
        '0' => array(
          'value' => '',
          '_error_element' => 'default_value_widget][field_voipcall_dest_number][0][value',
        ),
      ),
      'default_value_php' => NULL,
      'label' => 'Destination Number',
      'weight' => '-3',
      'description' => '',
      'type' => 'text_textfield',
      'module' => 'text',
    ),
  );

  // Exported field: field_voipcall_duration
  $fields[] = array(
    'field_name' => 'field_voipcall_duration',
    'type_name' => 'voipcall',
    'display_settings' => array(
      'label' => array(
        'format' => 'above',
        'exclude' => 0,
      ),
      'teaser' => array(
        'format' => 'default',
        'exclude' => 0,
      ),
      'full' => array(
        'format' => 'default',
        'exclude' => 0,
      ),
      '4' => array(
        'format' => 'default',
        'exclude' => 0,
      ),
      'token' => array(
        'format' => 'default',
        'exclude' => 0,
      ),
    ),
    'widget_active' => '1',
    'type' => 'text',
    'required' => '0',
    'multiple' => '0',
    'module' => 'text',
    'active' => '1',
    'text_processing' => '0',
    'max_length' => '',
    'allowed_values' => '',
    'allowed_values_php' => '',
    'widget' => array(
      'rows' => 5,
      'size' => '60',
      'default_value' => array(
        '0' => array(
          'value' => '',
          '_error_element' => 'default_value_widget][field_voipcall_duration][0][value',
        ),
      ),
      'default_value_php' => NULL,
      'label' => 'Duration',
      'weight' => '11',
      'description' => 'The duration of the call in seconds.',
      'type' => 'text_textfield',
      'module' => 'text',
    ),
  );

  // Exported field: field_voipcall_end_time
  $fields[] = array(
    'field_name' => 'field_voipcall_end_time',
    'type_name' => 'voipcall',
    'display_settings' => array(
      'label' => array(
        'format' => 'above',
        'exclude' => 0,
      ),
      'teaser' => array(
        'format' => 'default',
        'exclude' => 0,
      ),
      'full' => array(
        'format' => 'default',
        'exclude' => 0,
      ),
      '4' => array(
        'format' => 'default',
        'exclude' => 0,
      ),
    ),
    'widget_active' => '1',
    'type' => 'text',
    'required' => '0',
    'multiple' => '0',
    'module' => 'text',
    'active' => '1',
    'text_processing' => '0',
    'max_length' => '',
    'allowed_values' => '',
    'allowed_values_php' => '',
    'widget' => array(
      'rows' => 5,
      'size' => '60',
      'default_value' => array(
        '0' => array(
          'value' => '',
          '_error_element' => 'default_value_widget][field_voipcall_end_time][0][value',
        ),
      ),
      'default_value_code' => '',
      'default_value2' => 'same',
      'default_value_code2' => '',
      'input_format' => 'm/d/Y - H:i:s',
      'input_format_custom' => '',
      'increment' => '1',
      'text_parts' => array(),
      'year_range' => '0:+1',
      'label_position' => 'above',
      'default_value_php' => NULL,
      'label' => 'End time',
      'weight' => '10',
      'description' => '',
      'type' => 'text_textfield',
      'module' => 'text',
    ),
  );

  // Exported field: field_voipcall_error_message
  $fields[] = array(
    'field_name' => 'field_voipcall_error_message',
    'type_name' => 'voipcall',
    'display_settings' => array(
      'label' => array(
        'format' => 'above',
        'exclude' => 0,
      ),
      'teaser' => array(
        'format' => 'default',
        'exclude' => 0,
      ),
      'full' => array(
        'format' => 'default',
        'exclude' => 0,
      ),
      '4' => array(
        'format' => 'default',
        'exclude' => 0,
      ),
      'token' => array(
        'format' => 'default',
        'exclude' => 0,
      ),
    ),
    'widget_active' => '1',
    'type' => 'text',
    'required' => '0',
    'multiple' => '0',
    'module' => 'text',
    'active' => '1',
    'text_processing' => '0',
    'max_length' => '',
    'allowed_values' => '',
    'allowed_values_php' => '',
    'widget' => array(
      'rows' => 5,
      'size' => '60',
      'default_value' => array(
        '0' => array(
          'value' => '',
          '_error_element' => 'default_value_widget][field_voipcall_error_message][0][value',
        ),
      ),
      'default_value_php' => NULL,
      'label' => 'Error Message',
      'weight' => '12',
      'description' => 'Eventual error message associated with the call.',
      'type' => 'text_textfield',
      'module' => 'text',
    ),
  );

  // Exported field: field_voipcall_hangup_script
  $fields[] = array(
    'field_name' => 'field_voipcall_hangup_script',
    'type_name' => 'voipcall',
    'display_settings' => array(
      'label' => array(
        'format' => 'above',
        'exclude' => 0,
      ),
      'teaser' => array(
        'format' => 'default',
        'exclude' => 0,
      ),
      'full' => array(
        'format' => 'default',
        'exclude' => 0,
      ),
      '4' => array(
        'format' => 'default',
        'exclude' => 0,
      ),
      'token' => array(
        'format' => 'default',
        'exclude' => 0,
      ),
    ),
    'widget_active' => '1',
    'type' => 'text',
    'required' => '0',
    'multiple' => '0',
    'module' => 'text',
    'active' => '1',
    'text_processing' => '0',
    'max_length' => '',
    'allowed_values' => '',
    'allowed_values_php' => '',
    'widget' => array(
      'rows' => 5,
      'size' => '60',
      'default_value' => array(
        '0' => array(
          'value' => '',
          '_error_element' => 'default_value_widget][field_voipcall_hangup_script][0][value',
        ),
      ),
      'default_value_php' => NULL,
      'label' => 'Hangup Script',
      'weight' => '5',
      'description' => 'Name of the script that should be evoked whenever the system receives a "hangup-related event" leading to call status such as VOIPCALL_ERROR, VOIPCALL_NOT_CALLED, VOIPCALL_DIALING, VOIPCALL_NO_ANSWER, VOIPCALL_BUSY, VOIPCALL_INVALID, VOIPCALL_MACHINE_HANGUP, VOIPCALL_COMPLETED.  

The only events that do not activate the hangup script are the ones that lead to the following call status: VOIPCALL_ANSWERED, VOIPCALL_ANSWERED_MACHINE, VOIPCALL_ANSWERED_HUMAN.

',
      'type' => 'text_textfield',
      'module' => 'text',
    ),
  );

  // Exported field: field_voipcall_script_index
  $fields[] = array(
    'field_name' => 'field_voipcall_script_index',
    'type_name' => 'voipcall',
    'display_settings' => array(
      'label' => array(
        'format' => 'above',
        'exclude' => 0,
      ),
      'teaser' => array(
        'format' => 'default',
        'exclude' => 0,
      ),
      'full' => array(
        'format' => 'default',
        'exclude' => 0,
      ),
      '4' => array(
        'format' => 'default',
        'exclude' => 0,
      ),
      'token' => array(
        'format' => 'default',
        'exclude' => 0,
      ),
    ),
    'widget_active' => '1',
    'type' => 'number_integer',
    'required' => '0',
    'multiple' => '0',
    'module' => 'number',
    'active' => '1',
    'prefix' => '',
    'suffix' => '',
    'min' => '',
    'max' => '',
    'allowed_values' => '',
    'allowed_values_php' => '',
    'widget' => array(
      'default_value' => array(
        '0' => array(
          'value' => '',
          '_error_element' => 'default_value_widget][field_voipcall_script_index][0][value',
        ),
      ),
      'default_value_php' => NULL,
      'label' => 'Script Index',
      'weight' => '4',
      'description' => 'Index of the script\'s current command.',
      'type' => 'number',
      'module' => 'number',
    ),
  );

  // Exported field: field_voipcall_script_name
  $fields[] = array(
    'field_name' => 'field_voipcall_script_name',
    'type_name' => 'voipcall',
    'display_settings' => array(
      'label' => array(
        'format' => 'above',
        'exclude' => 0,
      ),
      'teaser' => array(
        'format' => 'default',
        'exclude' => 0,
      ),
      'full' => array(
        'format' => 'default',
        'exclude' => 0,
      ),
      '4' => array(
        'format' => 'default',
        'exclude' => 0,
      ),
      'token' => array(
        'format' => 'default',
        'exclude' => 0,
      ),
    ),
    'widget_active' => '1',
    'type' => 'text',
    'required' => '0',
    'multiple' => '0',
    'module' => 'text',
    'active' => '1',
    'text_processing' => '0',
    'max_length' => '',
    'allowed_values' => '',
    'allowed_values_php' => '',
    'widget' => array(
      'rows' => 5,
      'size' => '60',
      'default_value' => array(
        '0' => array(
          'value' => '',
          '_error_element' => 'default_value_widget][field_voipcall_script_name][0][value',
        ),
      ),
      'default_value_php' => NULL,
      'label' => 'Script Name',
      'weight' => '2',
      'description' => 'The name of the dialplan script to be evoked once the call is completed.',
      'type' => 'text_textfield',
      'module' => 'text',
    ),
  );

  // Exported field: field_voipcall_script_variables
  $fields[] = array(
    'field_name' => 'field_voipcall_script_variables',
    'type_name' => 'voipcall',
    'display_settings' => array(
      'label' => array(
        'format' => 'above',
        'exclude' => 0,
      ),
      'teaser' => array(
        'format' => 'default',
        'exclude' => 0,
      ),
      'full' => array(
        'format' => 'default',
        'exclude' => 0,
      ),
      '4' => array(
        'format' => 'default',
        'exclude' => 0,
      ),
      'token' => array(
        'format' => 'default',
        'exclude' => 0,
      ),
    ),
    'widget_active' => '1',
    'type' => 'text',
    'required' => '0',
    'multiple' => '0',
    'module' => 'text',
    'active' => '1',
    'text_processing' => '0',
    'max_length' => '',
    'allowed_values' => '',
    'allowed_values_php' => '',
    'widget' => array(
      'rows' => 5,
      'size' => '60',
      'default_value' => array(
        '0' => array(
          'value' => '',
          '_error_element' => 'default_value_widget][field_voipcall_script_variables][0][value',
        ),
      ),
      'default_value_php' => NULL,
      'label' => 'Script Variables',
      'weight' => '7',
      'description' => 'A (serialized) array containing the variables set and read by the scripts.',
      'type' => 'text_textfield',
      'module' => 'text',
    ),
  );

  // Exported field: field_voipcall_source
  $fields[] = array(
    'field_name' => 'field_voipcall_source',
    'type_name' => 'voipcall',
    'display_settings' => array(
      'label' => array(
        'format' => 'above',
        'exclude' => 0,
      ),
      'teaser' => array(
        'format' => 'default',
        'exclude' => 0,
      ),
      'full' => array(
        'format' => 'default',
        'exclude' => 0,
      ),
      '4' => array(
        'format' => 'default',
        'exclude' => 0,
      ),
      'token' => array(
        'format' => 'default',
        'exclude' => 0,
      ),
    ),
    'widget_active' => '1',
    'type' => 'text',
    'required' => '0',
    'multiple' => '0',
    'module' => 'text',
    'active' => '1',
    'text_processing' => '0',
    'max_length' => '',
    'allowed_values' => '',
    'allowed_values_php' => '',
    'widget' => array(
      'default_value' => array(
        '0' => array(
          'value' => '',
        ),
      ),
      'default_value_php' => NULL,
      'label' => 'Call Source',
      'weight' => '8',
      'description' => 'The server that originated the call, i.e., \'drupal\', \'asterisk\', \'twilio\', etc.',
      'type' => 'optionwidgets_buttons',
      'module' => 'optionwidgets',
    ),
  );

  // Exported field: field_voipcall_start_time
  $fields[] = array(
    'field_name' => 'field_voipcall_start_time',
    'type_name' => 'voipcall',
    'display_settings' => array(
      'label' => array(
        'format' => 'above',
        'exclude' => 0,
      ),
      'teaser' => array(
        'format' => 'default',
        'exclude' => 0,
      ),
      'full' => array(
        'format' => 'default',
        'exclude' => 0,
      ),
      '4' => array(
        'format' => 'default',
        'exclude' => 0,
      ),
    ),
    'widget_active' => '1',
    'type' => 'number_integer',
    'required' => '0',
    'multiple' => '0',
    'module' => 'number',
    'active' => '1',
    'prefix' => '',
    'suffix' => '',
    'min' => '',
    'max' => '',
    'allowed_values' => '',
    'allowed_values_php' => '',
    'widget' => array(
      'default_value' => array(
        '0' => array(
          'value' => '',
          '_error_element' => 'default_value_widget][field_voipcall_start_time][0][value',
        ),
      ),
      'default_value_code' => '',
      'default_value2' => 'same',
      'default_value_code2' => '',
      'input_format' => 'm/d/Y - H:i:s',
      'input_format_custom' => '',
      'increment' => '1',
      'text_parts' => array(),
      'year_range' => '0:+1',
      'label_position' => 'above',
      'default_value_php' => NULL,
      'label' => 'Start time',
      'weight' => '9',
      'description' => '',
      'type' => 'number',
      'module' => 'number',
    ),
  );

  // Exported field: field_voipcall_status
  $fields[] = array(
    'field_name' => 'field_voipcall_status',
    'type_name' => 'voipcall',
    'display_settings' => array(
      'label' => array(
        'format' => 'above',
        'exclude' => 0,
      ),
      'teaser' => array(
        'format' => 'default',
        'exclude' => 0,
      ),
      'full' => array(
        'format' => 'default',
        'exclude' => 0,
      ),
      '4' => array(
        'format' => 'default',
        'exclude' => 0,
      ),
    ),
    'widget_active' => '1',
    'type' => 'text',
    'required' => '0',
    'multiple' => '0',
    'module' => 'text',
    'active' => '1',
    'text_processing' => '0',
    'max_length' => '',
    'allowed_values' => '',
    'allowed_values_php' => '',
    'widget' => array(
      'rows' => 5,
      'size' => '60',
      'default_value' => array(
        '0' => array(
          'value' => '',
          '_error_element' => 'default_value_widget][field_voipcall_status][0][value',
        ),
      ),
      'default_value_php' => NULL,
      'label' => 'Call Status',
      'weight' => '1',
      'description' => '',
      'type' => 'text_textfield',
      'module' => 'text',
    ),
  );

  // Translatables
  array(
    t('Call Id'),
    t('Call Source'),
    t('Call Status'),
    t('Caller Name'),
    t('Caller Number'),
    t('Destination Number'),
    t('Duration'),
    t('End time'),
    t('Error Message'),
    t('Hangup Script'),
    t('Script Index'),
    t('Script Name'),
    t('Script Variables'),
    t('Start time'),
  );

  return $fields;
}
