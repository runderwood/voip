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


// -----------------------------------------------------------------------------
// initialization
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
// public function definitions
// -----------------------------------------------------------------------------

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
  $result = @xmlrpc($drupal_server, 'voip.processRequest', $request_id, $options);
  if ($error = xmlrpc_error()) {
//TODO: Error messages should also be translated with something similar to t()
    $result = array('status' => FALSE,
      'error_msg' =>  "XMLRPC error: " . $error->message . ' ('. $error->code . ').');
  }
  return $result;
}


?>
