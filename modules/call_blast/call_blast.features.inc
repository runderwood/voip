<?php

/**
 * Implementation of hook_views_default_views().
 */
function call_blast_views_default_views() {
  module_load_include('inc', 'call_blast', 'call_blast.features.views');
  $args = func_get_args();
  return call_user_func_array('_call_blast_views_default_views', $args);
}
