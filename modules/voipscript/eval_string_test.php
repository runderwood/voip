<?php

require_once('voipscript.inc');        


$s = new VoipScript('test');
$s->setVar('a', 'This is a string');
$s->setVar('b', '123');
$s->setVar('d', '%c');
$s->setVar('e', '^5 + %d');
$s->setVar('f', '^11 + %e');
$s->setVar('g', '^date(\'l\')');
$s->setVar('h', 'Today is %g.');

$str = '^1 + %c';
$rc = $s->evalString($str);
echo("\neval $str: $rc\n");

$str = '^%a != NULL';
$rc = $s->evalString($str);
echo("\neval $str: $rc\n");

$str = '^3+%f + 3';
$rc = $s->evalString($str);
echo("\neval $str: $rc\n");

$str = '%h';
$rc = $s->evalString($str);
echo("\neval $str: $rc\n");

echo("\n--- New test ---\n");
$s->setVar('input_digits','*****');
$s->setVar('zip_code','%input_digits');
$str = 'Say: %zip_code';
$rc = $s->evalString($str);
echo("\neval $str: $rc\n");
$str = '^get_forecast(%zip_code)';
$rc = $s->evalString($str);
echo("\neval $str: $rc\n");


function get_forecast($str) {
  return 'la la la';
}
