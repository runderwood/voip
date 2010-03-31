<?php

/**
 * Helper to implementation of hook_content_default_fields().
 */
function _vf_voip_script_content_default_fields() {
  $fields = array();

  // Exported field: field_script_desc
  $fields[] = array(
    'field_name' => 'field_script_desc',
    'type_name' => 'voip_script',
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
      'rows' => '5',
      'size' => 60,
      'default_value' => array(
        '0' => array(
          'value' => '',
          '_error_element' => 'default_value_widget][field_script_desc][0][value',
        ),
      ),
      'default_value_php' => NULL,
      'label' => 'Description',
      'weight' => '-4',
      'description' => 'Please describe what this script does.',
      'type' => 'text_textarea',
      'module' => 'text',
    ),
  );

  // Translatables
  array(
    t('Description'),
  );

  return $fields;
}
