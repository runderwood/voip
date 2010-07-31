<?php

// $Id$

/**
 * @file
 * Implementation of the VoipTwilioServer class
 */


/**
 * File dependencies
 */

foreach (array('twilio.php') as $file) {
  require_once(dirname(__FILE__) . DIRECTORY_SEPARATOR . $file);
}

module_load_include('inc', 'voip', 'includes/voipserver');


/**
 * Class definitions
 */
        
class VoipTwilioServer extends VoipServer {

  /*
   * Constructors
   */

  function __construct() {
    $id = 'twilio';
    $name = 'Twilio';
    $config_form = 'voiptwilio_admin_form';
    parent::__construct($id, $name, $config_form);
  }


  /*
   * API methods
   */

  function dial($voipcall, $server_config=NULL) {

    $rc = FALSE;
    voip_error_reset();

    if(is_object($voipcall)) {
      $voipcall = (array)$voipcall;
    }

    if(!$server_config) {
      $server_config = $this->getConfig();
    }

    // Twilio configuration
    $api_version = '2008-08-01';
    $account_sid = $server_config['voiptwilio_account_sid'];
    $auth_token = $server_config['voiptwilio_auth_token'];

    // Update call variables
    $voipcall['call_status'] = VOIPCALL_DIALING;
    $voipcall['source'] = 'drupal';
    $voipcall = (array)voipcall_save($voipcall);

    // Instantiate a new Twilio Rest Client
    $client = new TwilioRestClient($account_sid, $auth_token);

    // Initiate a new outbound call by POST'ing to the Calls resource
    $twilio_path = "/$api_version/Accounts/$account_sid/Calls";
    $caller_number = ($voipcall['field_voipcall_caller_number'][0]['value'])
                       ? $voipcall['field_voipcall_caller_number'][0]['value']
                       : variable_get('voipcall_cid_number', NULL);
    $dest_number = $voipcall['field_voipcall_dest_number'][0]['value'];

    $menu_path = 'voip/twilio/callhandler';
    $call_id = $voipcall['field_voipcall_call_id'][0]['value'];
    $response_url = $menu_path . '/' . $call_id;
    $call_handler_url = url($response_url, array('absolute' => TRUE));

    $vars =    array(
        "Caller" => $caller_number,
        "Called" => $dest_number,
        "Url" => $call_handler_url,
        'Method' => 'POST',
        'SendDigits' => '',  // TODO: shall we encode extra digits with dest_number?
// TODO: ideally, IfMachine should be set via field 'check_machine' with options such as 'do not check', 'continue' and 'hangup'
        'IfMachine' => 'Continue', // Note: adding any value in here -- even 'Continue' -- adds a 5 sec delay to the call. To prevent that, use NULL
        'Timeout' => 60 // TODO: Set to 15 in order to avoid answering machines?
      );

    $response = $client->request($twilio_path, "POST", $vars);

    // update call based on response
    if($response->IsError){
      $msg = "Twilio dial command failed: @error";
      $params = array('@error' => $response->ErrorMessage);
      voipcall_processing_error('voiptwilio', $voipcall, $msg, $params);
      $rc = FALSE;
    }
    else {
      // Note: call_status will be updated by call_handler
      $voipcall['start_time'] = time();
      $voipcall = voipcall_save($voipcall);
      $rc = $call_id;
    }

    return $rc;
  }

  function ping($server_config=NULL) {

    if(!$server_config) {
      $server_config = $this->getConfig();
    }

    // Twilio configuration
    $api_version = '2008-08-01';
    $account_sid = $server_config['voiptwilio_account_sid'];
    $auth_token = $server_config['voiptwilio_auth_token'];

    // Instantiate a new Twilio Rest Client
    $client = new TwilioRestClient($account_sid, $auth_token);

    // Request a call log as a way to test the server connectivity
    voip_error_reset();
    $path = "/$api_version/Accounts/$account_sid/Calls";
    $vars = array();
    $response = $client->request($path, "GET", $vars);

    if($response->IsError){
      voip_error("Twilio command Ping failed: " . $response->ErrorMessage);
      return FALSE;
    }
    return TRUE;
  }

}

