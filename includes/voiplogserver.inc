<?php

// $Id$

/**
 * @file
 * Implementation of the VoipLogServer class
 */

require_once('voipserver.inc');


/**
 * Class definitions
 */
        
class VoipLogServer extends VoipServer {


  /*
   * Constructors
   */

  function __construct() {
    $id = 'log';
    $name = t('Log only');
    parent::__construct($id, $name);
  }


  /*
   * API methods
   */

  function dial($voipcall, $server_config=NULL) {
    $options = array('server_config' => $server_config, 'voipcall' => $voipcall);
    watchdog('voip', 'VoipLogServer dial() called with: @options', array('@options' => print_r($options, TRUE)), WATCHDOG_INFO);
    return TRUE;
  }

  function ping($server_config=NULL) {
    $options = array('server_config' => $server_config);
    watchdog('voip', 'VoipLogServer ping() called with: @options', array('@options' => print_r($options, TRUE)), WATCHDOG_INFO);
    return TRUE;
  }

}

/**
 * Implementation of hook_voipserver_info()
 */
function voip_voipserver_info() {
  $server = new VoipLogServer();
  return $server;
}
