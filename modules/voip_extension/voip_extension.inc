<?php 
/**
 * @file voip_extension.inc
 *   Functions provided by Voip Extension module, not part of Drupal API functions.
 */

/**
 * Callback for voip_extension/autocomplete.
 * 
 * Populate autocomplete field in node/%/edit form for voip_extension nodes.
 */
function voip_extension_autocomplete() {
  $items = array();  
  $result = db_query("SELECT nid, title FROM {node} WHERE status = 1 AND type = 'voip_script' AND title LIKE '%s%%' ", $string);
  
  while($obj = db_fetch_object($result)) {
    $items[$obj->nid] = check_plain($obj->title);
  }
  print drupal_to_js($items); 
  exit();
}

