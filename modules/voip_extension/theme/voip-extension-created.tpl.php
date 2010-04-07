<?php
/**
 * @file voip-extension-created.tpl.php
 *   Default template for voip extension info presented by drupal_set_message().
 * 
 *  This template is used when nodes are automatically created upon creation of 
 *  new voip-enabled nodes.
 *
 * Available variables: 
 * - $nid, extension node's nid
 * - $ext, Extension Number X (this is a link to the node, except when viewed as a page)
 * - $related_node, link to related node 
 * - $related_user, link to related user
 * - $r_type, related type (type of node or "user")
 * - $is_active, extension status
 * - $r_script, link to related script node
 * 
 * - $edit, edit link
 * - $msg, message to user about new extension
 */
?>
<div class='voip-ext notification'><?php print $msg ?></div>
<div class='voip-ext ext'><?php print $ext ?></div>
  <?php if ($related_node) :?>
    <div class='voip-ext related-node'>Related Node: <?php print $related_node ?></div>
  <?php else: ?>
    <div class='voip-ext related-user'>Related User: <?php print $related_user ?></div>
  <?php endif ?>
<div class='voip-ext related-type'>Type: <?php print $r_type ?></div>
<div class='voip-ext ext-status'>Status: <?php print $is_active ?></div>
<div class='voip-ext script'>Script: <?php print $r_script ?></div>
<div class='voip-ext edit'><?php print $edit ?></div>
