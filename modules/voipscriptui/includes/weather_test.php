<?php

/**
 * Dependencies
 */

require_once('voiptest_weather.inc');

$zip = 'x12345679?';
$forecast = _voiptest_get_forecast($zip);

print_r($forecast);
