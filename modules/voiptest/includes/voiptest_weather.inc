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
      'end_key' => array(VOIPCALL_VAL => ''),
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

// ------ Analize user input
  // If timeout or invalid zip code
  $script[] = array(
    VOIPCALL_CMD_ID => VOIPCALL_GOTOIF,
    VOIPCALL_CMD_BODY => array(
      'label_name' => array(VOIPCALL_VAL => 'invalid_zip'),
      'expression' => array(VOIPCALL_EVAL => "@<gather_digits>@ == '" . VOIPCALL_NO_INPUT . "'"),
    ),
  );


// ---- Play the forecast and/or error message
  // get the forecast
  $script[] = array(
    VOIPCALL_CMD_ID => VOIPCALL_SET,
    VOIPCALL_CMD_BODY => array(
      'var_name' => array(VOIPCALL_VAL => "forecast_result"),
      // NOTE: Do not add a ';' at the end of the function call.  That will result in a runtime error.
      'var_value' => array(VOIPCALL_EVAL => "_voiptest_get_forecast(@<zip_code>@)"),
    ),
  );
  // process invalid input
  $script[] = array(
    VOIPCALL_CMD_ID => VOIPCALL_GOTOIF,
    VOIPCALL_CMD_BODY => array(
      'label_name' => array(VOIPCALL_VAL => 'invalid_zip'),
      'expression' => array(VOIPCALL_EVAL => "@<forecast_result>@ == NULL"),
    ),
  );
  // say the forecast
  $script[] = array(
    VOIPCALL_CMD_ID => VOIPCALL_LABEL,
    VOIPCALL_CMD_BODY => array(
      'label_name' => array(VOIPCALL_VAL => 'forecast'),
    ),
  );
  $script[] = array(
    VOIPCALL_CMD_ID => VOIPCALL_SAY,
    VOIPCALL_CMD_BODY => array(
      'text' => array(VOIPCALL_VAR => "forecast_result"),
    ),
  );

// ------ Play final options
      // play final options
      $script[] = array(
        VOIPCALL_CMD_ID => VOIPCALL_LABEL,
        VOIPCALL_CMD_BODY => array(
          'label_name' => array(VOIPCALL_VAL => 'options'),
        ),
      );
      $script[] = array(
        VOIPCALL_CMD_ID => VOIPCALL_GATHER,
        VOIPCALL_CMD_BODY => array(
          'text' => array(VOIPCALL_VAL => 'To repeat this report, press the star key.  To restart, press the pound key.'),
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
          'label_name' => array(VOIPCALL_VAL => 'forecast'),
          // Note: variable names have to be enclosed in between '@<'and '>@'
          'expression' => array(VOIPCALL_EVAL => "@<gather_digits>@ == '*'"),
        ),
      );

      // #, start all over again
      $script[] = array(
        VOIPCALL_CMD_ID => VOIPCALL_GOTOIF,
        VOIPCALL_CMD_BODY => array(
          'label_name' => array(VOIPCALL_VAL => 'start'),
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

//------ handle invalid zip codes
      // handle invalid zip codes
      $script[] = array(
        VOIPCALL_CMD_ID => VOIPCALL_LABEL,
        VOIPCALL_CMD_BODY => array(
          'label_name' => array(VOIPCALL_VAL => 'invalid_zip'),
        ),
      );
    $script[] = array(
      VOIPCALL_CMD_ID => VOIPCALL_SAY,
      VOIPCALL_CMD_BODY => array(
        'text' => array(VOIPCALL_VAL => "Invalid zip code received. Please try again."),
      ),
    );
    $script[] = array(
      VOIPCALL_CMD_ID => VOIPCALL_GOTO,
      VOIPCALL_CMD_BODY => array(
        'label_name' => array(VOIPCALL_VAL => 'start'),
      ),
    );


// ------ end the call
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
 * Return string with weather forecast for given zip code
 */
function _voiptest_get_forecast($zip) {
  $forecast = NULL;

  $woeid = _voiptest_getWoeidFromZip($zip);
  if(!$woeid) {
    // invalid zip code
  }
  else {
    $unit = 'f';
    $info = _voiptest_get_weather_info($woeid, $unit);
    if(!$info) {
      // processing error
    }
    else {
      $degrees = $info['CURRENT']['TEMP'];
      $conditions = $info['CURRENT']['TEXT'];
      $location = $info['CITY'];
      $forecast = "It is currently $degrees Fahrenheit and $conditions in $location.";
    }
  }

  return $forecast;

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

function _voiptest_get_weather_info($woeid, $unit='f') {
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



