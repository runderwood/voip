<?php 
/**
 * @file
 *   Return $form array for extension node add/edit form.
 */

  $form['ext'] = array(
    '#type' => 'item',
    '#title' => t('Extension Number'),
    '#value' => isset($node->ext) ? $node->ext : t('(auto number)'),
  );
  $form['r_nid'] = array(
    '#type' => 'textfield',
    '#title' => t('Related node ID (nid)'),
    '#size' => 30,
    '#default_value' => isset($node->r_nid) ? $node->r_nid : '',
  );
  $form['r_uid'] = array(
    '#type' => 'textfield',
    '#title' => t('Related user ID (uid)'),
    '#size' => 30,
    '#default_value' => isset($node->r_uid) ? $node->r_uid : '',
  );
  $form['r_type'] = array(
    '#type' => 'textfield',
    '#title' => t('Related Node Type (or "user" if not a node)'),
    '#size' => 30,
    '#default_value' => isset($node->r_type) ? $node->r_type : '',
  );
  $form['is_active'] = array(
    '#type' => 'checkbox',
    '#title' => t('Is Active'),
    '#default_value' => $node->is_active,
  );
  $form['script_nid'] = array(
    '#type' => 'textfield',
    '#title' => t('Script Node ID'),
    '#size' => 30,
    '#default_value' => isset($node->script_nid) ? $node->script_nid : '',
  );
