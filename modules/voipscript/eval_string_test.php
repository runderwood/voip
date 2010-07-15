<?php

function _expand_variables($str, $vars) {
    $eval = substr($str, 0, 1) == '^';
    $regex = '/\%(\w+)/e';
    $replacement = "_replace_variable(\$1, \$vars, \$eval);" ;
    $res = preg_replace($regex, $replacement ,$str);
    if($eval) {
        ob_start();
        $expr = substr($res, 1);
        if(eval('$res = ' . $expr . ';')===false) {
            ob_end_clean();
            die('Invalid PHP-Expression: '.$expr);
//            watchdog('voipscript', 'Invalid PHP expression: @expr', array('@expr' => $expr), WATCHDOG_ERROR);
        }
        ob_end_clean();
    }
    return $res;
}

function _replace_variable($var_name, $vars, $eval) {
  if(isset($vars[$var_name])) {
    $expanded = _expand_variables($vars[$var_name], $vars);
      if($eval) {
        // Special handling since $str is going to be evaluated ..
        if(!is_numeric($expanded) || (substr($expanded . '', 0, 1)==='0'
          && strpos($expanded . '', '.')===false)) {
          $expanded = "'$expanded'";
      }
    }
      return $expanded;
  } else {
    // Variable does not exist in $vars array
    if($eval) {
      return 'null';
    }
//  return "$var_name";
    return '';
  }
}


$vars['a'] = 'This is a string';
$vars['b'] = '0123';
    $vars['d'] = '%c';
    $vars['e'] = '^5 + %d';
    $vars['f'] = '^11 + %e + %b*2';
    $vars['g'] = '^date(\'l\')';
    $vars['h'] = 'Today is %g.';
    $vars['input_digits'] = '*****';
    $vars['code'] = '%input_digits';
$vars['i'] = 'Zip: %j';
$vars['j'] = "01234";

echo _expand_variables('^1 + %c',$vars);
echo("\n");
echo _expand_variables('^%a != NULL',$vars);
echo("\n");
echo _expand_variables('^3+%f + 3',$vars);
echo("\n");
echo _expand_variables('%h',$vars);
echo("\n");
echo _expand_variables('Your code is: %code',$vars);
echo("\n");
echo _expand_variables('%i', $vars);
echo("\n");
echo _expand_variables('Some info: %i', $vars);
echo("\n");
echo _expand_variables('My name %i is %d', $vars);
echo("\n");
echo _expand_variables('^xyz(%i,)', $vars);
echo("\n");

