<?php
// $Id$

/**
 * @file
 * Implementation of a simple log server to facilitate testing of the Voip Drupal framework
 */


/**
 * Implementation of hook_voip_server_info().
 */
function voip_voip_server_info() {
  return array(
    'log' => array(
      'name' => t('Log only'),
      'send' => 'voip_send_log',
    ),
  );
}

function voip_send_log($request, $options) {
  watchdog('voip', 'Voip request %request sent with the following options: @options', array('%request' => $request, '@options' => print_r($options, TRUE)), WATCHDOG_INFO);
  return array('status' => TRUE);
}


