<?php

/**
 * Implementation of hook_content_default_fields().
 */
function voipcall_content_default_fields() {
  module_load_include('inc', 'voipcall', 'voipcall.defaults');
  $args = func_get_args();
  return call_user_func_array('_voipcall_content_default_fields', $args);
}

/**
 * Implementation of hook_node_info().
 */
function voipcall_node_info() {
  module_load_include('inc', 'voipcall', 'voipcall.features.node');
  $args = func_get_args();
  return call_user_func_array('_voipcall_node_info', $args);
}
