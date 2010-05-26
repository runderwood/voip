<?php
// $Id$

/**
 * @file
 * Implementation of a simple log server for Voip Drupal
 */

foreach (array('phpagi-asmanager.php') as $file) {
  require_once(dirname(__FILE__) . DIRECTORY_SEPARATOR . $file); 
}


/**
 * Internal functions
 */

/**
 * Return current instance of the Asterisk Manager. 
 * Note: This function was written to prevent the creation of a
 *       $voip_asterisk_manager global variable
 * TODO: Is this the best way to handle variables such as this one?
 */
function _voip_asterisk_manager()
{
  static $manager = NULL;
  if(is_null($manager)) {
    $manager = new AGI_AsteriskManager(NULL, array('log_method' => '_voip_asterisk_ami_log'));
    // set a default "catch all" event handler
    $event = '*';
    $callback = '_voip_asterisk_generic_event_handler';
    $manager->add_event_handler($event, $callback);
  }

  return $manager;
}

function _voip_asterisk_generic_event_handler($ecode, $data, $server, $port) {
  // phpagi-asmanager.php always set the error log no matter what
  voip_error_reset();

  // by default, we do not do anything with unsolicited events coming from the server
  return TRUE;
}

/**
 * Define how AMI errors should be reported
 */
function _voip_asterisk_ami_log($message, $level=1)
{
  voip_error($message);
}

/**
 * Test the connection with the Asterisk server
 */
function _voip_asterisk_ping($server_config) {

  $ami_host = $server_config['voip_asterisk_host'];
  $ami_port = $server_config['voip_asterisk_port'];
  $ami_host = ($ami_port)? $ami_host.':'.$ami_port : $ami_host;
  $ami_user = $server_config['voip_asterisk_user'];
  $ami_pass = $server_config['voip_asterisk_password'];

  voip_error_reset();

  $result = _voip_asterisk_manager()->connect($ami_host, $ami_user, $ami_pass);
  if(voip_error()) {
    voip_error("Connection to Asterisk manager failed: " . voip_error_message());
    return FALSE;
  }

  $result = _voip_asterisk_manager()->Ping();
  if($result['Response'] != 'Success'){
    voip_error("AMI command Ping failed: " . print_r($result, TRUE));
    return FALSE;
  }

  _voip_asterisk_manager()->disconnect();
  return TRUE;
}


/**
 * Establish a new call by dialing the given number and connecting it to the given script
 *
 *   @param array $options is an array containing, among other things, 'variables' to be set in the new channels
 * 
 *   @return XXXXXX the call. Check voip_error() for processing errors.
 */
function _voip_asterisk_dial_out($server_config, $call_node) {
dsm("dial_out config is: " . print_r($server_config,true));


//TODO: set start and end times

  $ami_host = $server_config['voip_asterisk_host'];
  $ami_port = $server_config['voip_asterisk_port'];
  $ami_host = ($ami_port)? $ami_host.':'.$ami_port : $ami_host;
  $ami_user = $server_config['voip_asterisk_user'];
  $ami_pass = $server_config['voip_asterisk_password'];

  // determine the number to be dialed
  $dest_number = $call_node->field_voipcall_dest_number[0]['value'];

  // determine the caller id to be used
  if(!is_null($call_node->field_voipcall_caller_name[0]['value'])){
    $caller_name = $call_node->field_voipcall_caller_name[0]['value'];
  }
  else {
    $caller_name = $server_config['voip_asterisk_cid_name']?$server_config['voip_asterisk_cid_name']:'';
  }
  if(!is_null($call_node->field_voipcall_caller_number[0]['value'])){
    $caller_number = $call_node->field_voipcall_caller_number[0]['value'];
  }
  else {
    $caller_number = $server_config['voip_asterisk_cid_number']?$server_config['voip_asterisk_cid_number']:'';
  }
  $cid = $caller_name . '<' . $caller_number . '>';

  // determine the unique id of the call
  $call_id = $call_node->field_voipcall_call_id[0]['value'];

  // determine the script name to be used
  $script_name = $call_node->field_voipcall_script_name[0]['value'];

  voip_error_reset();

  $ami = _voip_asterisk_manager();
  $result = $ami->connect($ami_host, $ami_user, $ami_pass);
  if(voip_error()) {
    voip_error("Connection to Asterisk manager failed: " . voip_error_message());
    return FALSE;
  }

  // update the status of the call
  $call_node->call_status = 'dialing';
  $n = voipcall_save($call_node);
dsm('node w/ dialing: ');
dsm($n);

  // The following channel string should match the one in voip_drupal.conf
//  $channel_string = 'Local/%number@vd-dial_out/n'; // This is the goal.  But it's not working...
  $channel_string = 'Local/%number@vd-dial_out'; // This seems to be working... (note the lack of /n at the end)
  $channel = str_replace("%number", $dest_number, $channel_string); // Channel from which to originate the call

  $exten = 's'; // Extension to use on connect (must use Context & Priority with it) 
  $context = 'vd-bridge'; // Context to use on connect (must use Exten & Priority with it) 
  $priority = 1; // Priority to use on connect (must use Context & Exten with it) 
  $application = NULL; // Application to use on connect (use Data for parameters) 
  $application_data = NULL; // Data if Application parameter is used 

  $timeout = 30000; // Timeout (in milliseconds) for the originating connection to happen
                    // (defaults to 30000 milliseconds) 

  $caller_id = $cid;  // CallerID to be used for both ends of the call.  Note that some
                      // voicemail systems might be automatically activated if the
                      // caller id is the same as the number being dialed.

  // Channel variables to set (max 32). Variables will be set for both channels
  // (local and connected). Note that the syntax for Asterisk 1.4+ and 1.6+ is
  // different from the previous versions 
  $variables = $options['variables']?$options['variables']:array(); // NOTE: This might not be supported by non-Asterisk voip servers...
  $variables['VD_SCRIPT_NAME'] = $script_name;
  $variables['VD_CALL_ID'] = $call_id;
  $numOfItems = count($variables);
  $counter = 0;
  $var_string = NULL;
  foreach($variables as $name => $value){
    $counter += 1;
    if ($counter <> $numOfItems){
      $var_string .= $name . '=' . $value . "\r\nVariable: ";
    } else {
      $var_string .= $name . '=' . $value;
    }
  } 

  // Account code for the call. Used for call data record (CDR) management
  $account = $call_id; 

  // Origination mode. Use 0 for the origination to be synchronous and 1 for
  // the origination to be asynchronous (allows multiple calls to be generated
  // without waiting for a response). Using Async leads to an OriginateResponse
  // event which contains the failure reason if any. Reasons may be one of the
  // following: 0 = no such extension or number; 1 = no answer; 4 = answered;
  // and 8 = congested or not available 
  $async = 1; // Note: the async parameter is likely to be removed in future 
              //   versions of Asterisk.  By default Originate calls will be
              //   all asynchronous.

  // The request identifier. It allows you to identify the response to this
  // request. You may use a number or a string. Useful when you make several
  // simultaneous requests. 
  $action_id = $call_id;

dsm("About to call: Originate( $channel, $exten, $context, $priority, $application, $application_data, $timeout, $caller_id, $var_string, $account, $async, $action_id);");
  $result = $ami->Originate(
     $channel, $exten, $context, $priority, $application, $application_data,
     $timeout, $caller_id, $var_string, $account, $async, $action_id);
dsm('result: ' . print_r($result,true));


  if($result['Response'] != 'Success'){
    $call_node->field_voipcall_status[0]['value'] = 'processing error';
    voipcall_save($call_node);
    voip_error("AMI command Originate failed: " . print_r($result, TRUE));
    return FALSE;
  }

  // wait for event confirming result of the originate execution
  $event_handler = '_voip_asterisk_event_handler';
  $event_code = 'OriginateResponse';
dsm("About to call_voip_asterisk_process_events(ami, $event_handler, $event_code, $action_id)");
  $r =  _voip_asterisk_process_events($ami, $event_handler, $event_code, $action_id);

  $ami->disconnect();
  return $call_id;
}


/**
 * Process AMI events
 *
 * @param AMI @ami Instance of the AMI, already connected to the server
 *
 * @param string $event_handler with the name of the function that will handle events
 *   It should be in the format foo($event_code, $data, $server, $port)
 * 
 * @param string $event_code listing the code of the event that will trigger
 * the event handler.  Use '*' for any event.
 * 
 * @param int $action_id with the Action Id of the event that should trigger
 * the handler.  Use Null for any id.
 *
 * @return TRUE in case of success. Check voip_error() for processing errors.
 *
 */
function _voip_asterisk_process_events($ami, $event_handler, $event_code='*', $action_id=NULL)
{
// TODO: Ideally, we should have event handler running in its own process and with its own socket to the server...

  $rc = TRUE;
  voip_error_reset();

  // start processing events from the server
  // Note: the following functionality was based on phpagi_asmanager.php->wait_response()
  $timeout = false;
  $done = FALSE;
  do
  {
    $type = NULL;
    $parameters = array();

    // read input from the socket
    $buffer = trim(fgets($ami->socket, 4096));
    while($buffer != '')
    {
      $a = strpos($buffer, ':');
      if($a) {
        if(!count($parameters)) // first line in a response?
        {
          $type = strtolower(substr($buffer, 0, $a));
          if(substr($buffer, $a + 2) == 'Follows')
          {
            // A follows response means there is a multiline field that follows.
            $parameters['data'] = '';
            $buff = fgets($ami->socket, 4096);
            while(substr($buff, 0, 6) != '--END ')
            {
              $parameters['data'] .= $buff;
              $buff = fgets($ami->socket, 4096);
            }
          }
        }

        // store parameter in $parameters
        $parameters[substr($buffer, 0, $a)] = substr($buffer, $a + 2);
      }
        $buffer = trim(fgets($ami->socket, 4096));
    }

    // process response
    switch($type)
    {
      case '': // timeout occured
        $timeout = TRUE;
        $ami->log('Time out processing events from the Asterisk Manager');
        $rc = FALSE;
        $done = TRUE;
        break;
      case 'event':
        $e = $parameters['Event'];
        $event_match = FALSE;
        if ($event_code == '*') {
          $event_match = TRUE;
        }
        else {
          $event_match = (strcasecmp($e, $event_code) == 0); 
        }
        $id_match = FALSE;
        if(is_null($action_id)) {
          $id_match = TRUE;
        }
        else if($action_id == $parameters['ActionID']){
            $id_match = TRUE;
        }
        $done = FALSE;
        if($event_match && $id_match){
          $done = $event_handler($e, $parameters, $ami->server, $ami->port);
        }
        break;
      case 'response':
        break;
      default:
        $ami->log('Unhandled response packet from Manager: ' . print_r($parameters, true));
        $rc = FALSE;
        $done = TRUE;
        break;
    }
  } while(!$timeout && !$done);

  return $rc;
}

/**
 * Callback function to handle AMI events
 */
function _voip_asterisk_event_handler($ecode, $data, $server, $port) {
  $rc = TRUE;

dsm("entering:  _voip_asterisk_event_handler($ecode, $data, $server, $port)");
  $call_id = $data['ActionID'];
dsm("call_id: $call_id");

  $ecode = strtolower($ecode);
  switch($ecode) {
    default:
      break;
    case 'originateresponse':
      $reason = $data['Reason'];
      switch($reason){
        default:
          $call_status = 'invalid';
          voip_error("Invalid AMI event: $ecode");
          $rc = FALSE;
          break;
        case 0: // no such extension or number
          $call_status = 'invalid number';
          break;
        case 1: // no answer
          $call_status = 'no answer';
          break;
        case 4: // answered
          $call_status = 'answered';
          break;
        case 8: // busy
          $call_status = 'busy';
          break;
      }
      // update the status of the call
dsm("future call_status: $call_status");
      $nid = voipcall_get_nid_from_call_id($call_id);
dsm("nid: $nid");
      $details = array('nid' => $nid, 'call_status' => $call_status);
      $n = voipcall_save($details);
dsm("node saved with $call_status:");
dsm($n);
      break;
  }
  return $rc;
}


