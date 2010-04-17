<?php
/**
 * @file voipext.tpl.php
 *   Default template for voip extension nodes ($node->content).
 *
 * Available variables: 
 * - $nid, extension node's nid
 * - $ext, Extension Number X (in teaser view, this is a link to the node)
 * - $related_node, link to related node 
 * - $related_user, link to related user
 * - $r_type, related type (type of node or "user")
 * - $is_active, extension status
 * - $r_script, link to related script node
 */
?>
<div class='voipext ext'><?php print $ext ?></div>
  <?php if ($related_node) :?>
    <div class='voipext related-node'>Related Node: <?php print $related_node ?></div>
  <?php else: ?>
    <div class='voipext related-user'>Related User: <?php print $related_user ?></div>
  <?php endif ?>
<div class='voipext related-type'>Type: <?php print $r_type ?></div>
<div class='voipext ext-status'>Status: <?php print $is_active ?></div>
<div class='voipext script'>Script: <?php print $r_script ?></div>
