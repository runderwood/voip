<?php
/**
 * Retrieves weather foreacast from Yahoo Weather
 *
 *  Note: The following function is based on http://pkarl.com/articles/parse-yahoo-weather-rss-using-php-and-simplexml-al/
 *
 * @param $woeid, the WOEID of the desired location
 * 
 * @param $unit, string with either 'c' for Celsius or 'f' for Fahrenheit
 */
function _get_weather($woeid, $unit='f') {
  $yahoo_url = "http://weather.yahooapis.com/forecastrss?w=$woeid&u=$unit";
  $weather_feed = file_get_contents($yahoo_url);
  $weather = simplexml_load_string($weather_feed);

  if(!$weather) {
    return FALSE;
  }
echo "\n-----------------\n";
print_r($weather);
echo "\n-----------------\n";

  $copyright = $weather->channel->copyright;
  
  $channel_yweather = $weather->channel->children("http://xml.weather.yahoo.com/ns/rss/1.0");
  
  foreach($channel_yweather as $x => $channel_item) 
    foreach($channel_item->attributes() as $k => $attr) 
		  $yw_channel[$x][$k] = $attr;

  $item_yweather = $weather->channel->item->children("http://xml.weather.yahoo.com/ns/rss/1.0");

  foreach($item_yweather as $x => $yw_item) {
    foreach($yw_item->attributes() as $k => $attr) {
      if($k == 'day') $day = $attr;
      if($x == 'forecast') { $yw_forecast[$x][$day . ''][$k] = $attr;	} 
      else { $yw_forecast[$x][$k] = $attr; }
    }
  }
 
  return $yw_forecast;
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

