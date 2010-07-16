<?php

/**
 * Implementation of hook_content_default_fields().
 */
function voiptest_content_default_fields() {
  module_load_include('inc', 'voiptest', 'voiptest.defaults');
  $args = func_get_args();
  return call_user_func_array('_voiptest_content_default_fields', $args);
}

/**
 * Implementation of hook_node_info().
 */
function voiptest_node_info() {
  module_load_include('inc', 'voiptest', 'voiptest.features.node');
  $args = func_get_args();
  return call_user_func_array('_voiptest_node_info', $args);
}

/**
 * Implementation of hook_views_default_views().
 */
function voiptest_views_default_views() {
  module_load_include('inc', 'voiptest', 'voiptest.features.views');
  $args = func_get_args();
  return call_user_func_array('_voiptest_views_default_views', $args);
}
