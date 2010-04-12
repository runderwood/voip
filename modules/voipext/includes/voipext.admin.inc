<?php 
/**
 * @file voipext.admin.inc
 *   admin/voip/extension form
 */

/**
 * Form builder. 
 * 
 * Give users ability to voip-enable specific node-types
 * and selected users. 
 * 
 * todo Let users include/exclude individual.
 * todo Let users include/exclude roles.  
 *
 * @ingroup forms
 * @see system_settings_form().
 */
function voipext_admin_enable() {
  // Voip-enable selected node types.
  // Get an array of node types with internal names as keys
  // and "friendly" names as values. 
  $options = node_get_types('names');
  // Note: Do not use #type => 'checkboxes'. 
  // We want users to be able to voip enable/disable node types
  // here and on the node_type_form page. This makes it so that the 
  // two forms are both updating the same variables.
  $form['voip_nodes'] = array(
    '#type' => 'fieldset',
    '#title' => 'Voip-enable selected node types',
    '#collapsible' => TRUE,
    '#collapsed' => FALSE, 
  );
  foreach ($options as $node_type => $node_name) {
    // All node types except voipext should be voip enable-able.
    if ($node_type != 'voipext') {
      $var = 'voip_enable_'. $node_type;
      $form['voip_nodes'][$var] = array(
        '#type' => 'checkbox',
        '#title' => t($node_name),
        '#default_value' => variable_get($var, FALSE),
      );
    }
  }
  
  // Voip-enable selected users 
  $form['voip_users'] = array(
    '#type' => 'fieldset',
    '#title' => 'Voip-enable selected node types',
    '#collapsible' => TRUE,
    '#collapsed' => FALSE, 
  );
  $result = db_query("SELECT uid, name FROM {users} "); 
  while ($row = db_fetch_object($result)) {
    $var = ('voip_enable_uid_'. $uid);
    $form['voip_users'][$var] = array(
      '#type' => 'checkbox',
      '#title' => $row->name,
      '#default_value' => variable_get($var, FALSE),
    );
  }

  return system_settings_form($form);
}
