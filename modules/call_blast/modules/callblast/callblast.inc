<?php 
/**
 * @file call_blast.inc
 *  Business logic of Call Blast module.
 * 
 *  This is custom code. The functions here
 *  are not hooks provided by Drupal or contributed
 *  modules.  This code was not written by the 
 *  Features module.
 */

/**
 * Callback for call_blast/list/autocomplete
 * to autocomplete call list field on 
 * node/add/callblast and node/edit/% forms.
 */
function call_blast_list_autocomplete() {
  $items = array();
  $items = module_invoke_all('call_blast_list');
  print drupal_to_js($items);
  exit();
}

/**
 * Implementation of hook_voip_get_script().
 */
function call_blast_voip_get_script() {
  // TODO: this function should return call_blast-specific scripts.  The "hello_world" script has already been defined by voip_tester.module
  // NOTE: How to name scripts? <module_prefix>_name.xml? Do we need the .xml?
  // NOTE: the following require once should assign the script to a variable and return that variable
  require_once('hello_world.xml');
}

/**
 * Convert a list name to a machine-readable
 * text id, consisting only of numbers, lowercase  
 * letters and underscores.
 */
function call_blast_id($module, $name) {
  $id = $module .'_'. $name;  

  // make lowercase
  $id = strtolower($id);

  // Replace space with _
  $pattern = '/ /';
  $replacement = '_';
  $id = preg_replace($pattern, $replacement, $id);

  // Replace - with _
  $pattern = '/-/';
  $replacement = '_';
  $id = preg_replace($pattern, $replacement, $id);

  // Replace !#$%&'*+/=?^`{|}~@.[] 
  $pattern = '([!#$%&\'*+/=?^`{|}~@.\[\]\:<>;])';
  $replacement = '';
  $id = preg_replace($pattern, $replacement, $id);

  // incase we missed anything weird
  $id = filter_var($id, FILTER_SANITIZE_STRING);

  return $id;
}
