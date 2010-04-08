<?php
/**
 * @file voipext-ext.tpl.php
 *   Default template for voip extension info presented by drupal_set_message().
 * 
 * This template is used to display extension info along with the extension's 
 * corresponding node or user. 
 *
 * Available variables: 
 * - $nid, extension node's nid
 * - $ext, Extension Number X (this is a link to the node, except when viewed as a page)
 * - $related_node, link to related node 
 * - $related_user, link to related user
 * - $r_type, related type (type of node or "user")
 * - $is_active, extension status
 * - $r_script, link to related script node
 */
?>
<div class='voipext voipext-info'>
  <div class='voipext ext'><?php print $ext ?></div>
  <div class='voipext ext-status'>Status: <?php print $is_active ?></div>
  <div class='voipext script'>Script: <?php print $r_script ?></div>
</div> 
