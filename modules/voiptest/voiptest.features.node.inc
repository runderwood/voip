<?php

/**
 * Helper to implementation of hook_node_info().
 */
function _voiptest_node_info() {
  $items = array(
    'voiptest' => array(
      'name' => t('Voip Test'),
      'module' => 'features',
      'description' => t('Dialplan script used in Voip Drupal calls.'),
      'has_title' => '1',
      'title_label' => t('Script name'),
      'has_body' => '0',
      'body_label' => '',
      'min_word_count' => '0',
      'help' => '',
    ),
  );
  return $items;
}
