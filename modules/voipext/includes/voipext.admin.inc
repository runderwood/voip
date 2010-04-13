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
 * TODO enable users to enable/disable voip for user roles for voip here.
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
    '#title' => 'Voip-enable selected user roles',
    '#collapsible' => TRUE,
    '#collapsed' => FALSE, 
    '#description' => t("Note: Extensions are assigned to users "
                       ."with 'user extension' permissions. " 
                       ."Add 'user extension' permissions to user roles on the " 
                       ."!perm-page.", 
                        array('!perm-page' => l(t('permissions page'), 'admin/user/permissions'))),
  );
  // Show all roles, indicating which have users with extensions
  // and which do not. 
  $all_roles = user_roles();
  $ue_roles = user_roles(FALSE, 'user extension');
  foreach ($all_roles as $key => $role) {
    $status = (in_array($role, $ue_roles)) ? t('Users have extensions') : t('No extensions');
    $form['voip_users'][$key] = array(
      '#type' => 'item',
      '#title' => t($role),
      '#value' => $status,
    );
  }

  // Generate extensions for voip-enabled nodes that aleady 
  // exist and do not have extensions yet.
  // TODO Generate extensions for users that exist and do 
  // not have them yet.
  $form['buttons']['voipext'] = array(
    '#type' => 'submit',
    '#value' => t('Save and create extensions'),
    '#weight' => 41,
    '#submit' => array('system_settings_form_submit', 'voipext_voipenable_form_submit'),
  );

  return system_settings_form($form);
}

function voipext_voipenable_form_submit($form, &$form_state) {
dsm('generate extensions here');
}
