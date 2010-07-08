<?php 
/**
 * @file callblast.inc
 *  Business logic of Call Blast module.
 * 
 *  This is custom code. The functions here
 *  are not hooks provided by Drupal or contributed
 *  modules.  This code was not written by the 
 *  Features module.
 */

/**
 * Callback for callblast/list/autocomplete
 * to autocomplete call list field on 
 * node/add/callblast and node/edit/% forms.
 */
function callblast_list_autocomplete() {
  $items = array();
  $items = module_invoke_all('callblast_list');
  asort($items);
  foreach ($items as $key => $value) {
    $readable = $value .'  ((('. $key .')))';
    $list[$readable] = $value;
  }
  print drupal_to_js($list);
  exit();
}

/**
 * Implementation of hook_voip_get_script().
 */
function callblast_voip_get_script() {
  // TODO: this function should return callblast-specific scripts.  The "hello_world" script has already been defined by voip_tester.module
  // NOTE: How to name scripts? <module_prefix>_name.xml? Do we need the .xml?
  // NOTE: the following require once should assign the script to a variable and return that variable
  require_once('hello_world.xml');
}

/**
 * Convert a list name to a machine-readable
 * text id, consisting only of numbers, lowercase  
 * letters and underscores.
 */
function callblast_id($module, $name) {
  $id = $module .'_'. $name;  

  // make lowercase
  $id = strtolower($id);

  // Replace space with _
  $pattern = '/ /';
  $replacement = '_';
  $id = preg_replace($pattern, $replacement, $id);

  // Replace - with _
  $pattern = '/-/';
  $replacement = '_';
  $id = preg_replace($pattern, $replacement, $id);

  // Replace !#$%&'*+/=?^`{|}~@.[] 
  $pattern = '([!#$%&\'*+/=?^`{|}~@.\[\]\:<>;])';
  $replacement = '';
  $id = preg_replace($pattern, $replacement, $id);

  // incase we missed anything weird
  $id = filter_var($id, FILTER_SANITIZE_STRING);

  return $id;
}

/**
 * Callblack for #default_value.
 * Make it human readable.
 * 
 * todo Add error handling to check for two lists with the same id and then throw an error message. 
 */
function callblast_list_default($value) {
  $title = module_invoke_all('callblast_get_title', $value);  
  if ($title[0]) {
    return $title[0] .'  ((('. $value .')))';
  } 
}

/**
 * Callblack for formatting Call List title as
 * human readable, without showing calllist ID. 
 * Make it human readable.
 * 
 * todo Add error handling to check for two lists with the same id and then throw an error message. 
 */
function callblast_list_title($value) {
  $title = module_invoke_all('callblast_get_title', $value);  
  if ($title[0]) {
    return $title[0];
  } 
}

/**
 * Invoke hook_callblast_get_list to get 
 * the numbers on the list from the module 
 * (or, modules if there are more than one)
 * that provided it.
 * 
 * @param
 *  $node object
 * 
 * @return
 *  string, callblast list selected on callblast node.
 */
function callblast_list($node) {
  $list_name = check_plain($node->field_callblast_calllist[0]['value']);
  $list = module_invoke_all('callblast_get_list', $list_name);

  return $list;
}

/**
 * @param
 *  $list, an array of numbers to be called. 
 * 
 * @param
 *  $node, object, a callblast node.
 */
function callblast_save_voipcalls($list, $node) {
  global $base_url;

  // generic call details from callblast node
  $details['caller number'] = $node->field_callblast_caller_number[0]['value'];
  $details['caller name'] = $node->field_callblast_caller_name[0]['value'];
  $details['parent node'] = $node->nid; // TODO delete?
  $details['scheduled start'] = $node->field_callblast_scheduled_start[0]['value'];
  $details['script_name'] = 'callblast_message'; 

  // store URL to audio message in script_variable
  $url_tmp = $node->field_callblast_audio_file[0]['filepath'];
  $message_url = ($url_tmp)? $base_url.'/'.drupal_urlencode(check_url($url_tmp)) : NULL;

  // call forwarding number
  $forward_to_number = $node->field_callblast_call_forward[0]['value']; 

  // options
  $url_tmp = $node->field_callblast_audio_options[0]['filepath'];
  $options_url = ($url_tmp)? $base_url.'/'.drupal_urlencode(check_url($url_tmp)) : NULL;

  // message one
  $url_tmp = $node->field_callblast_audio_one[0]['filepath'];
  $one_url = ($url_tmp)? $base_url.'/'.drupal_urlencode(check_url($url_tmp)) : NULL;

  // message two
  $url_tmp = $node->field_callblast_audio_two[0]['filepath'];
  $two_url = ($url_tmp)? $base_url.'/'.drupal_urlencode(check_url($url_tmp)) : NULL;

  // message three
  $url_tmp = $node->field_callblast_audio_three[0]['filepath'];
  $three_url = ($url_tmp)? $base_url.'/'.drupal_urlencode(check_url($url_tmp)) : NULL;

  // message four
  $url_tmp = $node->field_callblast_audio_four[0]['filepath'];
  $four_url = ($url_tmp)? $base_url.'/'.drupal_urlencode(check_url($url_tmp)) : NULL;

  // message five
  $url_tmp = $node->field_callblast_audio_five[0]['filepath'];
  $five_url = ($url_tmp)? $base_url.'/'.drupal_urlencode(check_url($url_tmp)) : NULL;

  // message six
  $url_tmp = $node->field_callblast_audio_six[0]['filepath'];
  $six_url = ($url_tmp)? $base_url.'/'.drupal_urlencode(check_url($url_tmp)) : NULL;

  // message seven
  $url_tmp = $node->field_callblast_audio_seve[0]['filepath'];
  $seven_url = ($url_tmp)? $base_url.'/'.drupal_urlencode(check_url($url_tmp)) : NULL;

  // message eight
  $url_tmp = $node->field_callblast_audio_eigh[0]['filepath'];
  $eight_url = ($url_tmp)? $base_url.'/'.drupal_urlencode(check_url($url_tmp)) : NULL;

  // message nine
  $url_tmp = $node->field_callblast_audio_nine[0]['filepath'];
  $nine_url = ($url_tmp)? $base_url.'/'.drupal_urlencode(check_url($url_tmp)) : NULL;

  // store voipcall variables
  $details['script_variables'] = array(
    'message_url' => $message_url, 
    'forward_to_number' => $forward_to_number,
    'options_url' => $options_url,
    'one_url' => $one_url,
    'two_url' => $two_url,
    'three_url' => $three_url,
    'four_url' => $four_url,
    'five_url' => $five_url,
    'six_url' => $six_url,
    'seven_url' => $seven_url,
    'eight_url' => $eight_url,
    'nine_url' => $nine_url,
  );

  // save up voipcall nodes from $list
  $i = 0;
  $count = count($list);
  while ($i < $count) {
    if ($list[$i]['destination']) {
      $details['dest_number'] = $list[$i]['destination'];
      $details['dest_name'] = $list[$i]['name'];
      // save voipcall node
      voipcall_save($details);
      // store callblast_nid / voipcall_nid pairs in  callblast_call table
      $callblast_call->voipcall_nid = db_last_insert_id('node', 'nid');
      $callblast_call->callblast_nid = $node->nid;
      drupal_write_record('callblast_call', $callblast_call);
    } else {
      // no destination number
      // TODO Add validation/error handling here. 
      //   Right now we're just checking for something or nothing. 
      //   But, for example, a 6 digit phone number should trigger an error too.
      drupal_set_message(t('Invalid destination number on call list.'), 'warning');
    }
    $i++;
  }
}

/**
 * Send call blasst now.
 * 
 * @param
 *  $nid, node ID of a callblast node to be sent now.
 * 
 * @return
 *  boolean (true/false)
 */
// TODO I think we can get rid of this now (BH 7/8/10)
function callblast_send_now($nid) {
// TODO For some reason callblast_send() doesn't see the updated start time, but when the node loads to be displayed to the user, the scheduled start time has changed. This is mysterious. I've tried changing the time with node_save() and updating the tables directly.

dsm("nid: $nid");
  // Load requested node.
  $node = node_load($nid);
  
  // Validation: 
  // Make sure node type is a callblast.
  // Make sure callblast has not been sent yet.
  //   drupal_set_message($txt, $type);
  //   return FALSE;

  // Re-schedule call blast to be sent now.
//dsm($node);
  //$node->field_callblast_scheduled_start[0]['value'] = time() - 1;
  //node_save($node);

  $now = time();
  db_query("UPDATE {content_type_callblast} "
          ."SET field_callblast_scheduled_start_value = %d " 
          ."WHERE nid = %d ",
           $now, $nid);

      // Clear cache to reflect update.
 //     cache_clear_all('content:'. $callblast_nid .':'. $callblast_vid, content_cache_tablename()); 

  cache_clear_all('content:'. $nid .':'. $node->vid, content_cache_tablename()); 
  // Send scheduled call blast(s).
  callblast_send(); 
//dsm($node);
  //   drupal_set_message($txt, $type);
  //   return TRUE; 
}

/**
 * Send scheduled call blasts.
 */
function callblast_run() {
  // Check for call blasts that have been scheduled but not sent.
  $sql = "SELECT n.nid, n.vid " 
        ."FROM {node} n " 
        ."JOIN {content_type_callblast} cb ON n.nid = cb.nid "
        ."WHERE field_blast_sent_value = 0 ";
  $result = db_query($sql);

  // @todo these should be added to a queue. Lots of big lists could get messy.
  // loop through unblasted blast lists and blast them
  while ($row = db_fetch_object($result)) {
    $callblast_nid = $row->nid;
    $callblast_vid = $row->vid;
  
    // Check when call is scheduled to get sent. 
    // Skip if it's scheduled for later.
    // @todo Performance could probably be improved by using litenode for the query above and skipping the node_load here. 
    $node = node_load($callblast_nid);
    $start_time =  $node->field_callblast_scheduled_start[0]['value'];
    // TODO Add timezone handling.
    // $timezone = $node->field_callblast_scheduled_start[0]['timezone'];
    $now = time();
    if ($start_time <= $now) {
      callblast_send($node);
    } // end if ($start_time <= $now)
  } // end while
}

/**
 * Send call blast.
 * 
 * @param
 *  $node, callblast node object.
 */
function callblast_send($node) {
  // Get selected call list.
  $list = callblast_list($node);
  // Save call list. (Each number on list must
  // be stored as a voipcall node.)
  callblast_save_voipcalls($list, $node);

  // get list of voipcall nids 
  // voip.module gets the phone numbers from the voipcall node
  $callblast_nid = $node->nid;
  $callblast_vid = $node->vid;
  $sql = 'SELECT voipcall_nid FROM {callblast_call} WHERE callblast_nid = %d'; 
  $result2 = db_query($sql, $callblast_nid);

  // place phone calls to each number on the list, one-by-one
  while ($voipcall_nid = db_result($result2)) { 
    $voipcall_node = node_load($voipcall_nid);    

    // $request_id, Type of call being requested.
    $request_id = 'voip_dial_out';

    // place outgoing call
    voip_send_request($request_id, $voipcall_node);
  }
  // Update node to indicate that blast has been sent.
  db_query("UPDATE {content_type_callblast} "
          ."SET field_blast_sent_value = 1 " 
          ."WHERE nid = %d " 
          ."AND vid = %d ",
           $callblast_nid, $callblast_vid);

  // Clear cache to reflect update.
  cache_clear_all('content:'. $callblast_nid .':'. $callblast_vid, content_cache_tablename()); 

  // Log call blast with watchdog
  $type = 'call blast';
  $message = t("Call blast nid-$callblast_nid sent.");
  $severity = WATCHDOG_NOTICE;
  watchdog($type, $message);
}
