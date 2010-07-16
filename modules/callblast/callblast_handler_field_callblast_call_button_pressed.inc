<?php
// $Id$
/**
 * Field handler to present callblast title 
 * related to the displayed voipcall node.
 */
class callblast_handler_field_callblast_call_button_pressed extends views_handler_field {
  function render($values) {
    $voipcall_nid = $values->callblast_call_voipcall_nid;
    // todo This can probably be optimized by querying the CCk table directly, rather than getting this info from a node_load().
    $node = node_load($voipcall_nid);
    $vars = unserialize($node->field_voipcall_script_variables[0]['value']);
    $pressed = $vars['gather_digits'];
    $pressed = check_plain($pressed); // this should be an integer

    return $pressed;
  }
}
