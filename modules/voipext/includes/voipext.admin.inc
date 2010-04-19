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
  // Remove Voip Extensions from the list, 
  // otherwise we'll end up with extensions with extensions.
  unset($options['voipext']);
  $form['voipext_enable'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Enable phone extensions for selected node types'),
    '#options' => $options,
    '#default_value' => variable_get('voipext_enable', FALSE),
  );

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
// TODO also change voip_enable_role_X to something like voipext_enable_role_X
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
  $form['buttons'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
    '#weight' => 41,
    '#submit' => array('voipext_admin_enable_form_submit'),
  );

  return $form;
}

/**
 * Call back for submit button.
 * 
 * This function is called when user submits the form 
 * on admin/voip/extensions-enable. It generates extensions
 * for voip-enabled nodes without extensions. 
 * 
 * When a node-type is voip-enabled, phone extensions are created 
 * automatically for each new node created. 
 * But if there are already 200 existing "page" 
 * nodes before "page" is voip-enabled, those 200 nodes can be 
 * assigned extensions here.
 */ 
function voipext_admin_enable_form_submit($form, &$form_state) {
dsm($form);
dsm($form_state);
  // get values submitted with form
  $values = $form_state['values'];

  // store voip-enable settings
  $voipext_enable = $values['voipext_enable'];
  variable_set('voipext_enable', $voipext_enable);

// TODO Edit handling of variable_get('voip_enable_*') elsewhere. 

  $values = $form_state['values'];
  foreach ($voip_enable as $type => enabled) {} // CONTINUE HERE replace function below
  foreach($values as $type => $enabled) {
    $prefix = substr($type, 0, 11);
// TODO update this
    if ($prefix == 'voip_enable' && $enabled) {
      $type = substr($type, 12); // type of node that is voip-enabled
      // get voip-enabled nodes without extensions, if there are any
      $result = db_query('SELECT n.nid '
                        .'FROM {node} n '
                        .'LEFT JOIN {voip_extension} ve ON n.nid = ve.r_nid '
                        ."WHERE ISNULL (ve.r_nid) AND n.type = '%s' ", $type);
      // create extensions for each of these nodes
      // todo Consider use batch API here for processing hundreds or thousands of nodes. 
      $count = 0;
      while ($row = db_fetch_object($result)) {
        $count++;
        $nid = $row->nid;
        $node = node_load($nid);
        $msg['display message'] = FALSE;
        voipext_create_ext($node, $msg);
      }

      // If any extensions were created...
      if ($count > 0) {
        // Notify user that extensions were created.
        $txt = t("!count phone extensions were created for !type nodes without extensions.",
                array(
                  '!count' => $count, 
                  '!type' => t($node->type),
                ));
        drupal_set_message($txt);
       
        // Log notice of extensions being created with watchdog.
        $type = 'voipext';
        $message = t("!count Voip Extension (voipext) nodes created.", array('!count' => $count));
        $severity = WATCHDOG_NOTICE;
        watchdog($type, $message);
      }

      // TODO change this to a custom form (rather than a system settings form) 
      // so that all submitted forms call this function 

      // TODO change form alter in voipext.module. node_type_form should have an item noting voip status
      // with a link to here. Don't let user directly voip-enable a module on the node_type form
      // otherwise they can skip this function and end up with voip-enabled nodes without extensions.  

      // TODO when you disable a voip-enabled node type, related extensions should be deleted
      // (if you don't want to delete the extensions, de-activeate them)
    }
  }
}
