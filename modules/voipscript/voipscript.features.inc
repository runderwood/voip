<?php

/**
 * Implementation of hook_content_default_fields().
 */
function voipscript_content_default_fields() {
  module_load_include('inc', 'voipscript', 'voipscript.defaults');
  $args = func_get_args();
  return call_user_func_array('_voipscript_content_default_fields', $args);
}

/**
 * Implementation of hook_node_info().
 */
function voipscript_node_info() {
  module_load_include('inc', 'voipscript', 'voipscript.features.node');
  $args = func_get_args();
  return call_user_func_array('_voipscript_node_info', $args);
}

/**
 * Implementation of hook_views_default_views().
 */
function voipscript_views_default_views() {
  module_load_include('inc', 'voipscript', 'voipscript.features.views');
  $args = func_get_args();
  return call_user_func_array('_voipscript_views_default_views', $args);
}
