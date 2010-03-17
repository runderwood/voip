<?php
// $Id$

function voip_admin_default_form() {
  $servers = voip_servers();
  
  foreach ($servers as $identifier => $server) {
    $options[$identifier] = '';
    $form[$server['name']]['id'] = array('#value' => $identifier);
    if (function_exists($server['configure form'])) {
      $form[$server['name']]['configure'] = array('#value' => l(t('configure'), 'admin/voip/servers/'. $identifier));
    }
    else {
      $form[$server['name']]['configure'] = array('#value' => t('No configuration options'));
    }
  }
  $form['default'] = array('#type' => 'radios', '#options' => $options, '#default_value' => variable_get('voip_default_server', 'log'));
  
  $form['submit'] = array(
    '#type' => 'submit', 
    '#value' => t('Set default voip server'),
  );
  
  return $form;
}

function voip_admin_default_form_submit($form, &$form_state) {
  // Process form submission to set the default voip server
  if ($form_state['values']['default']) {
    drupal_set_message(t('Default voip server updated.'));
    variable_set('voip_default_server', $form_state['values']['default']);
  }
}

function theme_voip_admin_default_form($form) {
  $rows = array();
  foreach ($form as $name => $element) {
    if (isset($element['id']) && is_array($element['id'])) {
      $rows[] = array(
        drupal_render($form['default'][$element['id']['#value']]),
        check_plain($name),
        drupal_render($element['configure']),
      );
      unset($form[$name]);
    }
  }
  $header = array(t('Default'), t('Name'), array('data' => t('Operations'), 'colspan' => 1));
  $output .= theme('table', $header, $rows);
  $output .= drupal_render($form);

  return $output;
}

function voip_admin_server_form(&$form_state, $server_id) {
  $server = voip_servers('server', $server_id);
  if ($server && function_exists($server['configure form'])) {
    drupal_set_title(t('@server configuration', array('@server' => $server['name'])));
    $form = $server['configure form']($server['configuration']);

    $form['submit'] = array(
      '#type' => 'submit', 
      '#value' => t('Save'),
    );
    $form['server'] = array('#type' => 'value', '#value' => $server);

    return $form;
  }
  drupal_not_found();
  exit();
}

/**
 * Validation for voip server configuration. If the server defines a validation
 * function it uses that.
 */
function voip_admin_server_form_validate($form, &$form_state) {
  // Pass validation to voip server
  $function = $form_state['values']['server']['configure form'] .'_validate';
  if (function_exists($function)) {
    $function($form, $form_state);
  }
}

function voip_admin_server_form_submit($form, &$form_state) {
  $server = $form_state['values']['server'];
  
  // Remove unecessary values
  unset($form_state['values']['op'], $form_state['values']['submit'], $form_state['values']['server'], $form_state['values']['form_token'], $form_state['values']['form_id']);

  // store server configuration
  variable_set('voip_'. $server['identifier'] .'_settings', $form_state['values']);
  drupal_set_message(t('The server settings have been saved.'));

  $form_state['redirect'] = 'admin/voip/servers';
}

