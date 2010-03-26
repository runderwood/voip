<?php

/**
 * Implementation of hook_content_default_fields().
 */
function vf_call_blast_content_default_fields() {
  module_load_include('inc', 'vf_call_blast', 'vf_call_blast.defaults');
  $args = func_get_args();
  return call_user_func_array('_vf_call_blast_content_default_fields', $args);
}

/**
 * Implementation of hook_node_info().
 */
function vf_call_blast_node_info() {
  module_load_include('inc', 'vf_call_blast', 'vf_call_blast.features.node');
  $args = func_get_args();
  return call_user_func_array('_vf_call_blast_node_info', $args);
}
