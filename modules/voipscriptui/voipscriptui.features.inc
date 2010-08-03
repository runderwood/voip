<?php

/**
 * Implementation of hook_content_default_fields().
 */
function voipscriptui_content_default_fields() {
  module_load_include('inc', 'voipscriptui', 'voipscriptui.defaults');
  $args = func_get_args();
  return call_user_func_array('_voipscriptui_content_default_fields', $args);
}

/**
 * Implementation of hook_node_info().
 */
function voipscriptui_node_info() {
  module_load_include('inc', 'voipscriptui', 'voipscriptui.features.node');
  $args = func_get_args();
  return call_user_func_array('_voipscriptui_node_info', $args);
}

/**
 * Implementation of hook_views_default_views().
 */
function voipscriptui_views_default_views() {
  module_load_include('inc', 'voipscriptui', 'voipscriptui.features.views');
  $args = func_get_args();
  return call_user_func_array('_voipscriptui_views_default_views', $args);
}
