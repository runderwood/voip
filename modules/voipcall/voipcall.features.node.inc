<?php

/**
 * Helper to implementation of hook_node_info().
 */
function _voipcall_node_info() {
  $items = array(
    'voipcall' => array(
      'name' => t('Voip Call'),
      'module' => 'features',
      'description' => t('Voip phone call record. '),
      'has_title' => '1',
      'title_label' => t('Title'),
      'has_body' => '0',
      'body_label' => '',
      'min_word_count' => '0',
      'help' => '',
    ),
  );
  return $items;
}
