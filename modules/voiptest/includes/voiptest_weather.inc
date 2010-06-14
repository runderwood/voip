<?php

/**
 * @file provide a voip script to retrieve local weather information
 */

/*
 * Returns a voip script with local weather information
 *
 * @return FALSE in case of failure, or the script array in case of success
 */
function _voiptest_get_weather_report() {

  $woeid = _voiptest_getWoeidFromZip($zip);

//echo("\nthe woeid for $zip is: $woeid\n");
//$forecast = _voiptest_get_weather($woeid, $unit);

//------------
  $script = array();

  $script[] = array(
    VOIPCALL_CMD_ID => VOIPCALL_LABEL,
    VOIPCALL_CMD_BODY => array(
      'label_name' => array(VOIPCALL_VAL => 'start'),
    ),
  );

  // read zip code from user
  $script[] = array(
    VOIPCALL_CMD_ID => VOIPCALL_GATHER,
    VOIPCALL_CMD_BODY => array(
      'text' => array(VOIPCALL_VAL => 'Local weather by phone. Please type your 5-digit zip code.'),
      'num_digits' => array(VOIPCALL_VAL => 5),
      'timeout' => array(VOIPCALL_VAL => 5),
    ),
  );

  $script[] = array(
    VOIPCALL_CMD_ID => VOIPCALL_SAY,
    VOIPCALL_CMD_BODY => array(
      'text' => array(VOIPCALL_VAL => "You pressed: "),
    ),
  );
  $script[] = array(
    VOIPCALL_CMD_ID => VOIPCALL_SAY,
    VOIPCALL_CMD_BODY => array(
      'text' => array(VOIPCALL_VAR => "gather_digits"),
    ),
  );
  $script[] = array(
    VOIPCALL_CMD_ID => VOIPCALL_SET,
    VOIPCALL_CMD_BODY => array(
      'var_name' => array(VOIPCALL_VAL => "zip_code"),
      'var_value' => array(VOIPCALL_VAR => "gather_digits"),
    ),
  );
  $script[] = array(
    VOIPCALL_CMD_ID => VOIPCALL_SAY,
    VOIPCALL_CMD_BODY => array(
      'text' => array(VOIPCALL_VAL => "The variable 'zip_code' is now set to "),
    ),
  );

  $script[] = array(
    VOIPCALL_CMD_ID => VOIPCALL_SAY,
    VOIPCALL_CMD_BODY => array(
      'text' => array(VOIPCALL_VAR => "zip_code"),
    ),
  );
/********
//-------
      // If user presses a button during the main message, handle the options
      $script[] = array(
        VOIPCALL_CMD_ID => VOIPCALL_GOTOIF,
        VOIPCALL_CMD_BODY => array(
          'label_name' => array(VOIPCALL_VAL => 'process_options'),
          // Note: variable names have to be enclosed in between '@<'and '>@'
          'expression' => array(VOIPCALL_EVAL => "@<gather_digits>@ != '" . VOIPCALL_NO_INPUT . "'"),
        ),
      );

      // play options
      $script[] = array(
        VOIPCALL_CMD_ID => VOIPCALL_LABEL,
        VOIPCALL_CMD_BODY => array(
          'label_name' => array(VOIPCALL_VAL => 'options'),
        ),
      );
      $script[] = array(
        VOIPCALL_CMD_ID => VOIPCALL_GATHER,
        VOIPCALL_CMD_BODY => array(
          'text' => array(VOIPCALL_VAL => 'To repeat this forecast, press the star key.  To end the call, press the pound key.'),
          'num_digits' => array(VOIPCALL_VAL => 1),
          'end_key' => array(VOIPCALL_VAL => ""),
        ),
      );

      // process options
      $script[] = array(
        VOIPCALL_CMD_ID => VOIPCALL_LABEL,
        VOIPCALL_CMD_BODY => array(
          'label_name' => array(VOIPCALL_VAL => 'process_options'),
        ),
      );

      // Go to user's selected option.
      // * replay previous message 
      $script[] = array(
        VOIPCALL_CMD_ID => VOIPCALL_GOTOIF,
        VOIPCALL_CMD_BODY => array(
          'label_name' => array(VOIPCALL_VAL => 'start'),
          // Note: variable names have to be enclosed in between '@<'and '>@'
          'expression' => array(VOIPCALL_EVAL => "@<gather_digits>@ == '*'"),
        ),
      );

      // #, end call
      $script[] = array(
        VOIPCALL_CMD_ID => VOIPCALL_GOTOIF,
        VOIPCALL_CMD_BODY => array(
          'label_name' => array(VOIPCALL_VAL => 'end call'),
          // Note: variable names have to be enclosed in between '@<'and '>@'
          'expression' => array(VOIPCALL_EVAL => "@<gather_digits>@ == '#'"),
        ),
      );

      // If invalid option selected, go back to the options menu
      $script[] = array(
        VOIPCALL_CMD_ID => VOIPCALL_SAY,
        VOIPCALL_CMD_BODY => array(
          'text' => array(VOIPCALL_VAL => "Invalid option selected. Please try again."),
        ),
      );
      $script[] = array(
        VOIPCALL_CMD_ID => VOIPCALL_GOTO,
        VOIPCALL_CMD_BODY => array(
          'label_name' => array(VOIPCALL_VAL => 'options'),
        ),
      );

*****/
      // end call (hang up)
      $script[] = array(
        VOIPCALL_CMD_ID => VOIPCALL_LABEL,
        VOIPCALL_CMD_BODY => array(
          'label_name' => array(VOIPCALL_VAL => 'end call'),
        ),
      );
      $script[] = array(
        VOIPCALL_CMD_ID => VOIPCALL_HANGUP
      );

  return $script;
}

/**
 * Retrieves weather foreacast from Yahoo Weather
 *
 *  Note: The following function is based on http://www.phpclasses.org/package/2665-PHP-Retrieve-information-from-Yahoo-weather-RSS-feeds.html
 *
 * @param $woeid, the WOEID of the desired location
 * 
 * @param $unit, string with either 'c' for Celsius or 'f' for Fahrenheit
 */

function _voiptest_get_weather($woeid, $unit='f') {
  require_once('class.xml.parser.php');

  $yahoo_url = "http://weather.yahooapis.com/forecastrss?w=$woeid&u=$unit";
  $yahoo_ns = "http://xml.weather.yahoo.com/ns/rss/1.0";

  $weather_feed = file_get_contents($yahoo_url);

  $parser = new xmlParser();
  $parser->parse($yahoo_url);
  $content=&$parser->output[0]['child'][0]['child'];
  foreach ($content as $item) {
    switch ($item['name']) {
      case 'TITLE':
      case 'LINK':
      case 'DESCRIPTION':
      case 'LANGUAGE':
      case 'LASTBUILDDATE':
        $forecast[$item['name']]=$item['content'];
        break;
      case 'YWEATHER:LOCATION':
      case 'YWEATHER:UNITS':
      case 'YWEATHER:ASTRONOMY':
        foreach ($item['attrs'] as $attr=>$value)
          $forecast[$attr]=$value;
        break;
      case 'IMAGE':
        break;
      case 'ITEM':
        foreach ($item['child'] as $detail) {
          switch ($detail['name']) {
            case 'GEO:LAT':
            case 'GEO:LONG':
            case 'PUBDATE':
              $forecast[$detail['name']]=$detail['content'];
              break;
            case 'YWEATHER:CONDITION':
              $forecast['CURRENT']=$detail['attrs'];
              break;
            case 'YWEATHER:FORECAST':
              array_push($forecast,$detail['attrs']);
              break;
          }
        }
        break;
    }
  }

  return $forecast;

}


/**
 * Returns the WOEID (Where on Earth Id) associated with the given zip code
 *
 * Note: This function is based on http://arguments.callee.info/2010/03/26/convert-zip-code-to-yahoo-woeid/
 */
function _voiptest_getWoeidFromZip($zip) {
  static $woeidFromZip = array();

  $woeid = $woeidFromZip[$zip];

  if(!$woeid) {
    $q = "select woeid from geo.places where text='$zip' limit 1";
    $ch = curl_init('http://query.yahooapis.com/v1/public/yql?format=json&q=' . urlencode($q));
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
    $response = curl_exec($ch);
    if (!$response) {
      $woeid = FALSE;
    }
    else {
      try {
        $response = json_decode($response, true);
        $woeid = intval($response['query']['results']['place']['woeid']);
        
        // store response in cache
        if ($woeid) {
          $woeidFromZip[$zip] = $woeid;
        }
      }
      catch(Exception $ex) {
        $woeid = FALSE;
      }
    }
  }
  return $woeid;
}



