<?php

/**
 * Implementation of hook_content_default_fields().
 */
function vf_voip_script_content_default_fields() {
  module_load_include('inc', 'vf_voip_script', 'vf_voip_script.defaults');
  $args = func_get_args();
  return call_user_func_array('_vf_voip_script_content_default_fields', $args);
}

/**
 * Implementation of hook_node_info().
 */
function vf_voip_script_node_info() {
  module_load_include('inc', 'vf_voip_script', 'vf_voip_script.features.node');
  $args = func_get_args();
  return call_user_func_array('_vf_voip_script_node_info', $args);
}
