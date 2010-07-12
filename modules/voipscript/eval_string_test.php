<?php

require_once('voipscript.inc');        

function expand_variables($string, $variables, $add_quotes=NULL) {
  $pattern = "/%(\w+)/e";

  if(is_null($add_quotes)){
    $add_quotes = ($string[0] == '^') ? TRUE : FALSE;
  }
  $add_quotes = ($add_quotes)? 'TRUE' : 'FALSE';
  $replacement = "_replace_variable(\$1, \$variables, \$add_quotes);" 

  $val = preg_replace($pattern, $replacement, $string);
  $string = $val;
//watchdog('voipscript', "val: $val");
echo("\nval: $val\n");

    //check if $val contains a function or expression to be processed
    if($val[0] == '^') {
      $expression = substr($val,1);
      $val = eval("return ($expression);");
//watchdog('voipscript',"\nexpression: return ($expression); output: $output\n");
echo("\nexpression: return ($expression); output: $output\n");
    }
    return $val;

}

function _replace_variable($var_name, $variables, $add_quotes) {
echo("\nin _replace_variable($var_name, $variables, $add_quotes)\n");
$replacement = 'bla';
/****
  $var_value = $variables[$var_name];

  if(!$var_value) {
    $replacement = 'NULL';
  }
  else if($var_value[0] == '^') {
    $replacement = expand_variables($value, $variables, $add_quotes);
  }
  else if($add_quotes) {
    $replacement = "'\"' . $var_value . '\"'";
  }
  else {
    $replacement = $var_value;
  }

*****/
  return $replacement;
}

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
