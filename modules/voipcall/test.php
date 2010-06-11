<?php

$variables['stuff'] = 'thing';
$variables['colorful'] = 'noisy';

$str = "Let's replace the %colorful %stuff";
/***
$words = explode(' ', $str);
for($i=0; $i<count($words); $i++) {
  $word = $words[$i];
  if($word[0] == '%') {
    $var_name = substr($word,1);
    $words[$i] = $variables[$var_name];
  }
}
$str = implode(' ', $words);
*****/
$pattern = "/%(\w+)/e";
$replacement = '$variables[\'$1\']';
$result = preg_replace($pattern, $replacement, $str);
echo ('result: ' . $result);
?>

