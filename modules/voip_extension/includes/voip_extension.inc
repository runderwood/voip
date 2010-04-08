<?php 
/**
 * @file voipextension.inc
 *   Functions provided by Voip Extension module, not part of Drupal API functions.
 */

 //     ['error_code'] :: VOIP_SUCCESS, VOIP_EXTENSION_DOES_NOT_EXIST, VOIP_PROCESSING_FAILURE

/**
 * Callback for voipextension/autocomplete.
 * 
 * Populate autocomplete field in node/%/edit form for voipextension nodes.
 */
function voipextension_autocomplete() {
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
 *   integer. the id of the node that has the desired voip extension
 *
 * @return
 *   array with
 *     ['error_code'] :: VOIP_SUCCESS, VOIP_EXTENSION_DOES_NOT_EXIST, VOIP_PROCESSING_FAILURE
 *     ['error_msg'] :: string with eventual error message
 *     ['extension_info''] :: array with extension info
 */
function voipextension_get_info_from_nid($nid){
  $rc = array();

  $query = "SELECT r_nid FROM {voip_extension} WHERE r_nid = %d";
  $db_result = db_query($query, $nid);
  if (!$db_result) {
    $rc['error_code'] = VOIP_PROCESSING_FAILURE;
    $rc['error_msg'] = t('Processing error: could not query voip extension: %e', array('%e' => db_error()));
    return $rc;
  }

  $db_entry = db_fetch_array($db_result);
  if (!$db_entry) {
    $rc['error_code'] = VOIP_EXTENSION_DOES_NOT_EXIST;
    $rc['error_msg'] = t('Extension for node %n does not exist: %e', array('%n' => $nid_contents, '%e' => db_error()));
    return $rc;
  }

  $extension_node = node_load($db_entry['nid_extension']); 
  $rc['error_code'] = VOIP_SUCCESS;
  $rc['extension_info'] = $extension_node->voipextension_info;	
  
  return $rc;
}
