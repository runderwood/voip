<?php
// $id$

/**
 * @file
 * Implementation of audio recorder functionality
 */

/**
 * Internal functions
 */

/*
 * Handles files being uploaded by the recorder
 *
 * @param string $session_id with the id of the session associated with the upload.
 *
 * @return void
 */
function _audiorecorder_upload_handler($session_id=NULL) {

$msg = "in _audiorecorder_upload_handler() with session_id: $session_id, _REQUEST: " . print_r($_REQUEST, TRUE) . ', and _FILES: ' . print_r($_FILES, TRUE);
$vars = array();
watchdog('audiorecorder', $msg, $vars, WATCHDOG_INFO);
//dsm($msg);
$msg = "The following file has been uploaded: " . $_FILES['uploadedfile']['name'];
  return $msg; // NOTE: this function MUST not return any value...
}

