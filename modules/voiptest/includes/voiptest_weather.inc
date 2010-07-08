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

  $script = new VoipScript('voiptest_weather_report');

  $script->addLabel('start');

  // read zip code
  $text = t('Local weather by phone. Please type your 5-digit zip code.');
  $url = '';
  $timeout = 5;
  $end_key = '';
  $num_digits = 5;
  $script->addGetInput($text, $url, $timeout, $end_key, $num_digits);

//$script->addSay('You pressed: \'%input_digits\'');

  $script->addSet('zip_code', '%input_digits');

  // If timeout or invalid zip code
// TODO: remove spaces from all constants?
  $script->addGotoIf('invalid_zip', "^'%input_digits' == '" . VoipScript::NO_INPUT . "'");

  // get the report
  $script->addSet('forecast_result', '^_voiptest_get_forecast("%zip_code")');

  // process invalid input
  $script->addGotoIf('invalid_zip', '^"%forecast_result" == NULL');

  // say the weather report
  $script->addLabel('forecast');
  $script->addSay('%forecast_result');

  // play final options
  $script->addLabel('options');

  $text = t('To repeat this report, press the star key.  To restart, press the pound key.');
  $url = '';
  $timeout = 5;
  $end_key = '';
  $num_digits = 1;
  $script->addGetInput($text, $url, $timeout, $end_key, $num_digits);

//$script->addSay('You pressed: \'%input_digits\'');


  // process options
  $script->addLabel('process_options');

  // Go to user's selected option.
  // * replay previous message 
  $script->addGotoIf('forecast', "^'%input_digits' == '*'");

  // #, start all over again
  $script->addGotoIf('start', "^'%input_digits' == '#'");

  // If invalid option selected, go back to the options menu
  $script->addSay("Invalid option selected. Please try again.");
  $script->addGoto('options');

  // handle invalid zip codes
  $script->addLabel('invalid_zip');
  $script->addSay("Invalid zip code received. Please try again.");
  $script->addGoto('start');

  // end call (hang up)
  $script->addLabel('end call');
  $script->addHangup();

  return $script;
}

/**
 * Return string with weather report for given zip code
 */
function _voiptest_get_forecast($zip) {

watchdog('forecast', "zip: $zip");

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
      $forecast = "It is currently $degrees degrees Fahrenheit and $conditions in $location.";
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



