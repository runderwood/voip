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
  // Notes regarding the way values are handled: 
  // * placeholders for script variables start with '%'
  // * strings that start with '^' are treated as expressions to be
  //   evaluated
  // * when defining expressions that include string variables, make sure the
  //   variable placeholder is enclosed in " or '.  For instance,
  //      Correct form:   "^echo('The content is ' . '%msg');"
  //      Incorrect form: "^echo('The content is ' . %msg);"
  //   The incorrect form might produce a parser error if the variable msg
  //   contains characters such as a space, math sign, etc... It might also
  //   produce undesirable results if the variable starts with 0.
  function evalString($string) {
    $variables = $this->variables;
    // replace placeholders by the contents of the associated variable name
    // since variables might refer to other variables, run the loop until all
    // replacements are done.
    $pattern = "/%(\w+)/e";
    $count = 0;
    do {
/******
      // if the string is to be evaluated, make sure NULL variables are replaced
      // by the text 'NULL' (rather than ''). That will help prevent eventual
      // syntax errors in the evaluation.
      if($string[0] == '^') {
//ORIGINAL:        $replacement = '($this->variables[\'$1\']?$this->variables[\'$1\']:"NULL")';
      // if the string is to be evaluated, enclose the replacement within "" to
      // prevent syntax errors derived from variables that have special symbols
      //(spaces, etc.) within them. Also, make sure NULL variables are replaced
      // by the text 'NULL' (rather than '').
        $quoted = "'\"' . \$this->variables['\$1'] . '\"'";
        $unquoted = "\$this->variables['\$1']";
//        $replacement = "(\$this->variables['\$1']?$quoted:NULL)";
        $replacement = "(\$this->variables['\$1']?($count?$unquoted:$quoted):NULL)";
      }
      else {
        $replacement = '$this->variables[\'$1\']';
      }
******/
//------
      $vars = $this->variables;
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
// TODO: I don't think we need the $count loop anymore...
//------
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

  function evalString_new($string) {

    $pattern = "/%(\w+)/e";
//    $pattern = "/%(\w+)/";

      $vars = $this->variables;
      $value = "\$vars['\$1']";
      $replacement = "
          ((!$value)
            ? 'NULL'
            : ((\$vars['\$1'][0] == '^')
                ? \$this->evalString($value)
                : (is_numeric($value)
                    ? $value
                    :  '\"' . $value . '\"'
                  )
              )
          )";

    $val = preg_replace(
             $pattern,
             $replacement, //"VoipScript::process_variables",
             $string);
echo("\nval: $val\n");

    //check if $val contains a function or expression to be processed
    if($val[0] == '^') {
      $expression = substr($val,1);
      $val = eval("return ($expression);");
    }
    return $val;
  }
 
  static function process_variables ($matches) {
    $var_name = $matches[1];
      $var_value = "\$this->variables['$var_name']";
      $parser = "
          ((!$var_value)
            ? 'NULL'
            : ((\$this->variables['$var_name'][0] == '^')
                ? \$this->evalString($var_value)
                : (is_numeric($var_value)
                    ? $var_value
                    :  '\"' . $var_value . '\"'
                  )
              )
          )";
      $rc = eval('return $parser;');
echo("\nvar_name: $var_name, var_value: $var_value\n");
echo("\nrc: $rc\n");
    return $rc;
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
$rc = $s->evalString('^1 + %c');
echo($rc);
$rc = $s->evalString('^%a != NULL');
echo($rc);
$rc = $s->evalString('^3+ %f + 3');
echo($rc);
$rc = $s->evalString('%h');
echo($rc);

