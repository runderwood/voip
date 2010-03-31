<?php

/**
 * Helper to implementation of hook_node_info().
 */
function _vf_voip_script_node_info() {
  $items = array(
    'voip_script' => array(
      'name' => t('Voip Script'),
      'module' => 'features',
      'description' => t('Voip Scripts give the voip server (e.g. Asterisk or FreeSwitch) instructions like \'Play recording X. Then prompt caller to press 1 for A, 2 for B, 3 for C or # to return to the main menu.\''),
      'has_title' => '1',
      'title_label' => t('Voip Script Name'),
      'has_body' => '1',
      'body_label' => t('Script'),
      'min_word_count' => '0',
      'help' => '',
    ),
  );
  return $items;
}
