<?php 
/**
 * @file voip_extension.admin.inc
 *   admin/voip_extension/settings form
 */

/**
 * Form builder. 
 * 
 * @ingroup forms
 * @see system_settings_form().
 */
function voip_extension_admin_settings() {
  // Get an array of node types with internal names as keys
  // and "friendly" names as values. 
  $options = node_get_types('names');

  /*
  $form['voip_extension_node_types'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Voip enable these node types'),
    '#options' => $options,
    '#default_value' => variable_get('voip_extension_node_types', array('')),
    '#description' => t('Voip enable the selected node types.'),
  );
  */
  
  // Note: Do not use #type => 'checkboxes'. 
  // We want users to be able to voip enable/disable node types
  // here and on the node_type_form page. This makes it so that the 
  // two forms are both updating the same variables.
  $form['voip_enable'] = array(
    '#type' => 'fieldset',
    '#title' => 'Voip enable selected node types',
    '#collapsible' => TRUE,
    '#collapsed' => FALSE, 
  );
  foreach ($options as $node_type => $node_name) {
    $var = 'voip_enable_'. $node_type;
    $form['voip_enable'][$var] = array(
      '#type' => 'checkbox',
      '#title' => t($node_name),
      '#default_value' => variable_get($var, FALSE),
    );
  }

  return system_settings_form($form);
}
