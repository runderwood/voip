<?php
/**
 * @file voip-extension.tpl.php
 *   Default template for voip extension nodes ($node->content).
 *
 * Available variables: 
 * - $ext, Extension Number X (in teaser view, this is a link to the node)
 * - $related_node, link to related node 
 * - $related_user, link to related user
 * - $r_type, related type (type of node or "user")
 * - $is_active, extension status
 * - $r_script, link to related script node
 */
?>
<div class='voip-ext ext'><?php print $ext ?></div>
  <?php if ($related_node) :?>
    <div class='voip-ext related-node'>Related Node: <?php print $related_node ?></div>
  <?php else: ?>
    <div class='voip-ext related-user'>Related User: <?php print $related_user ?></div>
  <?php endif ?>
<div class='voip-ext related-type'>Type: <?php print $r_type ?></div>
<div class='voip-ext ext-status'>Status: <?php print $is_active ?></div>
<div class='voip-ext script'>Script: <?php print $r_script ?></div>
