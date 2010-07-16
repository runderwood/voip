<?php

require_once('voipscript.inc');

function watchdog($module, $string, $vars, $severity) {
  echo("$module($severity): " . strtr($string, $vars) . "\n");
}

$s = new VoipScript('test');
$s->setVar('a', 'This is a string');
$s->setVar('b', '0123');
$s->setVar('d', '%c');
$s->setVar('e', '^5 + %d');
$s->setVar('f', '^11 + %e + %b*2');
$s->setVar('g', '^date(\'l\')');
$s->setVar('h', 'Today is %g.');
$s->setVar('input_digits', '*****');
$s->setVar('code', '%input_digits');
$s->setVar('i', 'Zip: %j');
$s->setVar('j', "01234");

echo $s->evalString('^1 + %c');
echo("\n");
echo $s->evalString('^%a != NULL');
echo("\n");
echo $s->evalString('^3+%f + 3');
echo("\n");
echo $s->evalString('%h');
echo("\n");
echo $s->evalString('Your code is: %code');
echo("\n");
echo $s->evalString('%i');
echo("\n");
echo $s->evalString('Some info: %i');
echo("\n");
echo $s->evalString('My name %i is %d');
echo("\n");
echo $s->evalString('^(%d == FALSE)');
echo("\n");
echo $s->evalString('^xyz(%i,)');
echo("\n");

