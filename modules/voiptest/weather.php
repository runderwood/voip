<?php

/**
 * Retrieves weather foreacast from Yahoo Weather
 *
 *  Note: The following function is based on http://www.phpclasses.org/package/2665-PHP-Retrieve-information-from-Yahoo-weather-RSS-feeds.html
 *
 * @param $woeid, the WOEID of the desired location
 * 
 * @param $unit, string with either 'c' for Celsius or 'f' for Fahrenheit
 */

function _get_weather($woeid, $unit='f') {
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
 * Based on http://arguments.callee.info/2010/03/26/convert-zip-code-to-yahoo-woeid/
 */
function _getWoeidFromZip($zip) {
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







$unit = 'f';

$zip = '02478';
$woeid = _getWoeidFromZip($zip);
echo("\nthe woeid for $zip is: $woeid\n");

$zip = '02139';
$woeid = _getWoeidFromZip($zip);
echo("\nthe woeid for $zip is: $woeid\n");

$forecast = _get_weather($woeid, $unit);
print_r($forecast)
?>

