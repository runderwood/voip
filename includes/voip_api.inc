<?php
/* $Id$ */

/**
 * @file
 *   Basic API for the Voip Drupal framework
 */


// -----------------------------------------------------------------------------
// include required files
// -----------------------------------------------------------------------------

foreach (array('common.inc', 'xmlrpc.inc') as $file) {
  require_once($file);
}


// -----------------------------------------------------------------------------
// constants
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
// global variables
// -----------------------------------------------------------------------------

// Stores the current error object
$_voip_api_error;


// -----------------------------------------------------------------------------
// initialization
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
// public function definitions
// -----------------------------------------------------------------------------

/*
 * Report processing errors associated with the Voip Drupal infrastructure.
 */
function voip_api_error($message = NULL) {
  global $_voip_api_error;
  if ($message) {
    $_voip_api_error = new stdClass();
    $_voip_api_error->is_error = TRUE;
    $_voip_api_error->message = $message;
  }
  return $_voip_api_error;
}

function voip_api_error_message() {
  global $_voip_api_error;
  return $_voip_api_error->message;
}

function voip_api_error_reset() {
  global $_voip_api_error;
  $_voip_api_error = NULL;
}


/*
 * Sends request to be processed by Voip Drupal site.
 *
 * @param $drupal_server
 *   string with the URL of the xmlrpc.php file of the Drupal site where voip.module is located.
 * 
 * @param $request_id
 *   Id of the request
 *
 * @param $options
 *   array with the options associated with the request
 *
 * @return
 *   array with
 */
function voip_process_request($drupal_server, $request_id, $options=array()) {
  voip_api_error_reset();
  $result = @xmlrpc($drupal_server, 'voip.processRequest', $request_id, $options);
  if ($error = xmlrpc_error()) {
    $result = FALSE;
    voip_api_error("XMLRPC error: " . $error->message . ' ('. $error->code . ').');
  }
  return $result;
}


?>
