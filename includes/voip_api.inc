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

define('VOIP_SUCCESS', 1);
define('VOIP_ERROR', 0);
// TODO: How to define other error codes?


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
 * @param $voip_server
 *   string with the URL of the xmlrpc.php file of the Drupal site where voip.module is located.
 * 
 * @param $request
 *   array with the request to be processed.
 *
 * @return
 *   array with
 */
function voip_process_inbound_request($voip_server, $request) {
  $result['error_code'] = VOIP_SUCCESS;
  $result = @xmlrpc($voip_server, 'voip.processInboundRequest', $request);
  if ($error = xmlrpc_error()) {
    if ($error->code <= 0) {
//TODO: Error messages should also be translated with something similar to t()
      $error->message = 'Outgoing HTTP request failed because the socket could not be opened.';
    }
    $result['error_code'] = VOIP_ERROR;
    $result['error_msg'] =  "XMLRPC error: " . $error>message . ' ('. $error->code . ').';
  }
  if ($result === FALSE) {
    // if xmlrpc call fails...
  }
  return $result;
}


?>
