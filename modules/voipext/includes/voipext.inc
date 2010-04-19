<?php 
/**
 * @file voipext.inc
 *   Functions provided by Voip Extension module, not part of Drupal API functions.
 */

/**
 * Callback for voipext/autocomplete.
 * 
 * Populate autocomplete field in node/%/edit form for voipext nodes.
 */
function voipext_autocomplete() {
  $items = array();  
  $result = db_query("SELECT nid, title FROM {node} WHERE status = 1 AND type = 'voip_script' AND title LIKE '%s%%' ", $string);
  
  while($obj = db_fetch_object($result)) {
    $items[$obj->nid] = check_plain($obj->title);
  }
  print drupal_to_js($items); 
  exit();
}

/**
 * Retrieve information about the extension related to the specified node
 *
 * @param $nid
 *   integer. the id of the node related to the desired voip extension
TODO Handle user extensions here too. Make param $id, not $nid. $uid should be a valid param.
 *
 * @return
 *   array with
 *     ['error_msg'] :: string with eventual error message
 *     ['extension_info''] :: array with extension info
 */
function voipext_get_ext($r_nid, $type = 'node'){
  $ext = array();

  // get nid of extension node where related node's nid (r_nid) is the value passed here
  $query = "SELECT nid FROM {voip_extension} WHERE r_nid = %d";
  $db_result = db_query($query, $r_nid);
  if (!$db_result) {
    $ext['error_msg'] = t('Processing error: could not query voip extension: %e', array('%e' => db_error()));
    return $ext;
  }

  $db_entry = db_fetch_array($db_result);
  if (!$db_entry) {
    $ext['error_msg'] = t('Extension for node %n does not exist: %e', array('%n' => $r_nid, '%e' => db_error()));
    return $ext;
  }

  $ext_node = node_load($db_entry['nid']); 
  
  return $ext_node;
}

/**
 * @param $node
 *  (required) Node object for a voip-enabled node.
 * 
 * @param $msg
 *  (optional) Array. $msg['display message'] indicates whether user should be notified
 *  about the node that was created (TRUE/FALSE). $msg['message'] can include a custom message
 *  to be displayed instead of the default notification message.
 * 
 * Create a phone extension (related voipext node) for this voip-enabled node.
 */
function voipext_create_ext($node, $msg = array('display message' => TRUE, 'message' => '')) {
  // Create corresponding extension node.
  $ext = new stdClass();
  global $user;
  $ext->type = 'voipext';
  $ext->uid = $user->uid; // author of extension
  $ext->r_nid = $node->nid; // related nid (from newly saved node)
  $ext->r_type = $node->type;
  // User-defined default is_active setting
  $var = 'voip_default_is_active_'. $node->type;
  $is_active = variable_get($var, FALSE);
  $ext->is_active = ($is_active) ? TRUE : FALSE; 
  // default script
  $var = 'voip_default_script_'. $node->type;
  $script = variable_get($var, FALSE);
  // TODO validate script. Make sure (1) it exists, (2) it works.
  if ($script) {
    $ext->r_script = $script; 
  } else {
    $ext->is_active = FALSE; // Set to inactive if there isn't a valid script. 
    if ($msg['display message']) {
      // set warning message
      $txt = t('This new extension does not have a valid default '
              .'script. Without a valid script, the extension will '
              .'not work. To activate this extension, click "Edit" below, '
              .'add a script, and activate the extension. '
              .'To prevent this error in the future, set a '
              .'default script !here.', 
              array('!here' => l('here', url('admin/voip/extensions-defaults', array('absolute' => TRUE)))));
      
      drupal_set_message($txt, 'warning');
    }
  }
  // set node options from voipext node_type_form
  $node_options = variable_get('node_options_voipext', FALSE);
  if ($node_options) {
    $ext->status = (in_array('status', $node_options)) ? 1 : 0; 
    $ext->promote = (in_array('promote', $node_options)) ? 1 : 0; 
    $ext->sticky = (in_array('sticky', $node_options)) ? 1 : 0; 
    $ext->revision = (in_array('revision', $node_options)) ? 1 : 0; 
  }
  // save it!
  node_save($ext);

  // Notify user about new extension node and relevant extension info.
  if ($msg['display message']) {
    $ext_nid = $node->nid + 1;
    // use custom message if one has been provided
    $output = ($msg['message']) ? $msg['message'] : theme('voipext_created', $ext_nid); 
    drupal_set_message($output);
  }
     
  // Log notice of extension being created with watchdog.
  $type = 'voipext';
  $message = t("Voip Extension (voipext) node created. This extension, "
              ."node $ext_nid, is related to node $node->nid.");
  $severity = WATCHDOG_NOTICE;
  watchdog($type, $message);
}
