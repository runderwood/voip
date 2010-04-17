<?php 
/**
 * @file voipext.admin_defaults.inc
 *   admin/voip/extension/defaults form
 */

/**
 * Form builder. 
 * 
 * @ingroup forms
 * @see system_settings_form().
 */
function voipext_admin_defaults() {
  // Get an array of node types with internal names as keys
  // and "friendly" names as values. 
  $options = node_get_types('names');

  foreach ($options as $node_type => $node_name) {
    // Check to see if node type is enabled
    $var = 'voip_enable_'. $node_type;
    $enabled = variable_get($var, FALSE); 

    // If it is voip-enabled, add form elements
    if ($enabled && $node_type != 'voipext') {
      $script = 'voip_default_script_'. $node_type;
      $active = 'voip_default_is_active_'. $node_type;

      $form[$script] = array(
        '#type' => 'textfield',
        '#title' => t("Default script for $node_name"),
        '#default_value' => variable_get($script, FALSE),
        '#size' => 30,
        '#autocomplete_path' => 'voipext/autocomplete'
      );
      $form[$active] = array(
        '#type' => 'checkbox',
        '#title' => t('Activate extensions by default (extensions '
                     .'default to inactive if box is left unchecked)'),
        '#default_value' => variable_get($active, FALSE),
      );
    }
  }

  return system_settings_form($form);
}
