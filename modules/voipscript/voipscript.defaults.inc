<?php

/**
 * Helper to implementation of hook_content_default_fields().
 */
function _voipscript_content_default_fields() {
  $fields = array();

  // Exported field: field_voipscript_code
  $fields[] = array(
    'field_name' => 'field_voipscript_code',
    'type_name' => 'voipscript',
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
    'required' => '1',
    'multiple' => '0',
    'module' => 'text',
    'active' => '1',
    'text_processing' => '1',
    'max_length' => '',
    'allowed_values' => '',
    'allowed_values_php' => '',
    'widget' => array(
      'rows' => '20',
      'size' => 60,
      'default_value' => array(
        '0' => array(
          'value' => '$script = new VoipScript(\'your script name here\');',
          '_error_element' => 'default_value_widget][field_voipscript_code][0][value',
        ),
      ),
      'default_value_php' => NULL,
      'label' => 'Script code',
      'weight' => '-4',
      'description' => 'Fill in the variable <i>$script</i> above with the code for your script. That variable will be returned whenever a module requests your script name via <i>hook_voipscript_load_script($script_name)</i>.',
      'type' => 'text_textarea',
      'module' => 'text',
    ),
  );

  // Translatables
  array(
    t('Script code'),
  );

  return $fields;
}
