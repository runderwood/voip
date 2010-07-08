<?php

        
class VoipScript {
  private $name;
  private $commands;
  private $index; // index of the current command
  public $variables; 
  private $stack; // stack of active subroutines 
  protected $is_error;
  protected $error_message;
            
  /*
   * Constants
   */

  const NO_INPUT='no_input';


  /*
   * Constructors
   */

  function __construct($name, $variables=array()) {
    $this->name = $name;
    $this->commands = array();
    $this->index = 0;
    $this->variables = $variables;
    $this->stack = array();
    $this->is_error = FALSE;
    $this->error_message = NULL;
  }


  /*
   * Public methods
   */

  function getName() {
    return $this->name;
  }

  function getVar($name) {
    return $this->variables[$name];
  }

  function setVar($name, $value) {
    $this->variables[$name] = $value;
  }


  // Evaluate the given string based on the current value of script variables.
  // Notes regarding the way input strings are handled: 
  // * strings that start with '^' are treated as expressions to be
  //   evaluated
  // * placeholders for script variables start with '%'
  // * when defining expressions that include string variables, make sure the
  //   variable placeholder is enclosed in " or '.  For instance,
  //      Correct form:   "^echo('The content is ' . '%msg');"
  //      Incorrect form: "^echo('The content is ' . %msg);"
  //   The incorrect form might produce a parser error if the variable msg
  //   contains characters such as a space, math sign, etc... It might also
  //   produce undesirable results if the variable starts with 0.
  function evalString($string) {
    $vars = $this->variables;
    // replace placeholders by the contents of the associated variable name
    // since variables might refer to other variables, run the loop until all
    // replacements are done.
    $pattern = "/%(\w+)/e";
    $count = 0;
    do {
      $value = "\$vars['\$1']";
      $quoted_value = "'\"' . $value . '\"'";
      $replacement = "
          ((!$value)
            ? 'NULL'
            : ((\$vars['\$1'][0] == '^')
                ? \$this->evalString($value)
                : (is_numeric($value)
                    ? $value
                    : ($count
                         ? $value
                         : $quoted_value
                      )
                  )
              )
          )";
      $val = preg_replace($pattern, $replacement, $string, -1, $count);
echo("\nval: $val\n");
      $string = $val;
    } while ($count);

    //check if $val contains a function or expression to be processed
    if($val[0] == '^') {
      $expression = substr($val,1);
      $val = eval("return ($expression);");
    }
    return $val;
  }

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

