<?php

function test_regex() {
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
  return($result);
}

// ---------

foreach (array('voipscript.inc', 'voipscripthandler.inc') as $file) {
  require_once(dirname(__FILE__) . DIRECTORY_SEPARATOR . $file);
}

function hello_world(){
  $script = new VoipScript('hello world');
  $script->addLabel('start');
  $script->addSay('hi there!');
  $script->addGoto('xxxxx');
  $script->addLabel('end');
  $script->addHangup();
 
  return $script;
}

$script = hello_world();
$variables = array();
$stack = array();
$handler = new VoipScriptHandler($script, $variables, $stack);
$handler->run();
echo("\n handler isError: " . $handler->isError() .", message: " . $handler->getErrorMessage(). "\n");


?>

