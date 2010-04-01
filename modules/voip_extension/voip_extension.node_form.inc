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

  // get related node info
  if ($node->r_nid) {
    $r_node = node_load($node->r_nid);
    $form['r_nid'] = array(
      '#type' => 'item',
      '#title' => t('Related Node'),
      '#value' => ($r_node) ? l($r_node->title, "node/$r_node->nid") : '',
    );
  }

  // get related user info
  if ($node->r_uid) {
    $r_user = user_load($node->r_uid);
    $form['r_uid'] = array(
      '#type' => 'item',
      '#title' => t('Related User'),
      '#value' => ($r_user) ? l($r_user->name, "user/$r_user->uid") : '',
    );
  }

  $form['r_type'] = array(
    '#type' => 'item',
    '#title' => t('Related Node Type (or "user" if not a node)'),
    '#value' => isset($node->r_type) ? $node->r_type : '',
  );
  $form['r_script'] = array(
    '#type' => 'textfield',
    '#title' => t('Script Node ID'),
    '#size' => 30,
    '#default_value' => isset($node->r_script) ? $node->r_script : '',
    '#autocomplete_path' => 'voip_extension/autocomplete'
  );
  $form['is_active'] = array(
    '#type' => 'checkbox',
    '#title' => t('Is Active'),
    '#default_value' => $node->is_active,
  );

