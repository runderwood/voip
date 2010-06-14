<?php
/**
 * Retrieves weather foreacast from Yahoo Weather
 *
 *  Note: The following function is based on http://pkarl.com/articles/parse-yahoo-weather-rss-using-php-and-simplexml-al/
 *
 * @param $zip_code, the 5-digit zip code of the desired location
 * 
 * @param $unit, string with either 'c' for Celsius or 'f' for Fahrenheit
 */
function _get_weather($zip_code, $unit='f') {
  $yahoo_url = "http://weather.yahooapis.com/forecastrss?p=$zip_code&u=$unit";
  $weather_feed = file_get_contents($yahoo_url);
  $weather = simplexml_load_string($weather_feed);

  if(!$weather) {
    return FALSE;
  }

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

$zip = '02478';
$unit = 'f';
$forecast = _get_weather($zip, $unit);
var_dump($forecast)
?>

