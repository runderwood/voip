<?php
// $Id$

/**
 * @file
 * Implementation of a simple log server for Voip Drupal
 */

// TODO: connect via SSL
// TODO: define max number of simultaneous connections



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
  // by default, initialize the manager with the voip_asterisk.module logger
  static $manager = NULL;
  if(is_null($manager)) {
    $manager = new AGI_AsteriskManager(NULL, array('log_method' => '_voip_asterisk_ami_log'));
  }

  return $manager;
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

  $ami_port = $server_config['ami_port'];
  $ami_host = $server_config['ami_host'];
  $ami_host = ($ami_port)? $ami_host.':'.$ami_port : $ami_host;
  $ami_user = $server_config['ami_user'];
  $ami_pass = $server_config['ami_password'];

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
 *   @return int A unique id associated with the call. Check voip_error() for processing errors.
 */
function _voip_asterisk_dial_out($server_config, $number, $script_name, $options) {

  $ami_port = $server_config['ami_port'];
  $ami_host = $server_config['ami_host'];
  $ami_host = ($ami_port)? ($ami_host.':'.$ami_port) : $ami_host;
  $ami_user = $server_config['ami_user'];
  $ami_pass = $server_config['ami_password'];

  // define the caller id to be used
  $cid_name = $server_config['system_cid_name']?$server_config['system_cid_name']:'';
  $cid_number = $server_config['system_cid_number']?$server_config['system_cid_number']:'';
  $cid = $cid_name . '<' . $cid_number . '>';

  // define a unique id to be associated with this call
  $call_id = uniqid();

  voip_error_reset();

  $result = _voip_asterisk_manager()->connect($ami_host, $ami_user, $ami_pass);
  if(voip_error()) {
    voip_error("Connection to Asterisk manager failed: " . voip_error_message());
    return FALSE;
  }

  // keep track of the dial attempt
  $call_status = 'dialing';
  $module = $options['module']?$options['module']:''; // Name of the module originating the call
  $uid = $options['uid']?$options['uid']:0; // Id of the user placing the call
  voip_tracker_set_call_info('voip_asterisk', $call_id, $call_status, $cid_number, $number, $uid, $script_name, $variables);

  // The following channel string should match the one in voip_drupal.conf
//  $channel_string = 'Local/%number@vd-dial_out/n'; // This is the goal.  But it's not working...
  $channel_string = 'Local/%number@vd-dial_out'; // This seems to be working... (note the lack of /n at the end
  $channel = str_replace("%number", $number, $channel_string); // Channel from which to originate the call

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
  $async = 1; 

  // The request identifier. It allows you to identify the response to this
  // request. You may use a number or a string. Useful when you make several
  // simultaneous requests. 
  $action_id = $call_id;

  $result = _voip_asterisk_manager()->Originate(
     $channel, $exten, $context, $priority, $application, $application_data,
     $timeout, $caller_id, $var_string, $account, $async, $action_id);


  if($result['Response'] != 'Success'){
    voip_error("AMI command Originate failed: " . print_r($result, TRUE));
    return FALSE;
  }

  _voip_asterisk_manager()->disconnect();
  return $call_id;
}


/**
 * Check if the event handler is running
 */
function _voip_asterisk_event_handler_running() {
  $is_running = variable_get('voip_asterisk_event_handler_on', FALSE);
  return $is_running;
}


/**
 * Process AMI events
 *
 * @param array $server_config with the URL, username and password of the AMI server
 *
 * @param string $event_handler with the name of the function that will handle events
 *   It should be in the format foo($ecode, $data, $server, $port)
 *
 * @param boolean $allow_timeout to stop processing if the socket times out
 *
 * @return TRUE in case of success. Check voip_error() for processing errors.
 *
 */
function _voip_asterisk_process_events($server_config, $event_handler, $allow_timeout=false)
{
// TODO: Ideally, event handler should use its own socket to the server...
// TODO: how long is the timeout? This function seems to stop by itself after a while...

  $ami_port = $server_config['ami_port'];
  $ami_host = $server_config['ami_host'];
  $ami_host = ($ami_port)? ($ami_host.':'.$ami_port) : $ami_host;
  $ami_user = $server_config['ami_user'];
  $ami_pass = $server_config['ami_password'];

  voip_error_reset();

  $ami = _voip_asterisk_manager();
  $result = $ami->connect($ami_host, $ami_user, $ami_pass);
  if(voip_error()) {
    voip_error("Connection to Asterisk manager failed: " . voip_error_message());
    return FALSE;
  }

  // add event handler to take care of all events received from AMI
//  $ami->add_event_handler('*','_voip_asterisk_event_handler');
  $ami->add_event_handler('*',$event_handler);

  // start processing events from the server
  // Note: the following functionality was based on phpagi_asmanager.php->wait_response()
  $timeout = false;
  do
  {
    $type = NULL;
    $parameters = array();

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
        $timeout = $allow_timeout;
        break;
      case 'event':
            $ami->process_event($parameters);
            break;
      case 'response':
            break;
      default:
        $ami->log('Unhandled response packet from Manager: ' . print_r($parameters, true));
        break;
    }
  } while(!$timeout && _voip_asterisk_event_handler_running());

  // disconnect from the server
  $ami->disconnect();
  return TRUE;
}

