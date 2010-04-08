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
