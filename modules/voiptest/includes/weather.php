<?php

/**
 * Dependencies
 */

require_once('voiptest_weather.inc');

$zip = '02478';
$unit = 'f';

$script = _voiptest_get_weather_report($zip, $unit);

print_r($script);
