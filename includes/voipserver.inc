<?php

// $Id$

/**
 * @file
 * Implementation of the VoipServer-related classes
 */


/**
 * Class definitions
 */
        
abstract class VoipServer {

  private $id;  // internal server id
  private $name; // human-readable name of the server
  private $config_form; // (optional) name of form function for server configuration options
  private $config = NULL; // current configuration

  /*
   * Constructors
   */

  function __construct($id, $name, $config_form=NULL) {
    $this->id = $id;
    $this->name = $name;
    $this->config_form = $config_form;
  }

  public static function getServer($server_id) {
    $rc = NULL;
    $server_array = self::getServers();
    foreach($server_array as $server) {
      if($server->id == $server_id) {
        $rc = $server;
        break;
      }
    }
    return $rc;
  }


  /*
   * Public methods
   */

  function getId() {
    return $this->id;
  }

  function getName() {
    return $this->name;
  }

  function getConfigForm() {
    return $this->config_form;
  }

  function getConfig() {
    return $this->config;
  }

  public static function getServers() {
    $server_array = module_invoke_all('voipserver_info');
    foreach ($server_array as $server) {
      // retrieve server-specific configuration
      $server->config = variable_get('voip_'. $server->id .'_settings', '');
    }
    return $server_array;
  }



  /*
   * API methods
   */

  abstract function dial($voipcall, $server_config=NULL);

  abstract function ping($server_config=NULL);

}
