<?php

/**
 * Dependencies
 */

require_once('voiptest_weather.inc');

$script = _voiptest_get_weather_report();

print_r($script);
