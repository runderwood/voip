<?php
$weather_feed = file_get_contents("http://weather.yahooapis.com/forecastrss?p=14607&u=f");
$weather = simplexml_load_string($weather_feed);
if(!$weather) die('weather failed');
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

var_dump($yw_forecast)
?>

