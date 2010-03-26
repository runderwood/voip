<?php

/**
 * Helper to implementation of hook_node_info().
 */
function _vf_call_blast_node_info() {
  $items = array(
    'call_blast' => array(
      'name' => t('Call Blast'),
      'module' => 'features',
      'description' => '',
      'has_title' => '1',
      'title_label' => t('Title'),
      'has_body' => '1',
      'body_label' => t('Body'),
      'min_word_count' => '0',
      'help' => '',
    ),
  );
  return $items;
}
