<?php
/**
 * @file
 *  Preprocess functions for voip_extension templates (*.tpl.php files).
 */

/**
 * Implemenation of template_preprocess().
 */
function template_preprocess_voip_extension(&$variables) {
  $node = $variables['node'];

  // extension number
  if ($variables['teaser']) {
    $ext = l("Extension Number $node->ext", "node/$node->nid");
  } else { // page view
    $ext = "Extension Number $node->ext";
  }
  $variables['ext'] = $ext; 

  // related node
  $r_nid = $node->r_nid;
  if ($r_nid) {
    $r_node = node_load($r_nid);
    $r_nid = l($r_node->title, "node/$r_nid"); 
    $variables['related_node'] = $r_nid; 
  }

  // related user
  $r_uid = $node->r_uid;
  if ($r_uid) {
    $r_user = user_load($r_uid);
    $r_uid = l($r_user->name, "user/$r_uid");
    $variables['related_user'] = $r_uid;
  }
  
  $r_type = $node->r_type;
  $variables['r_type'] = $r_type; 

  $is_active = ($node->is_active) ? "Active" : "Not Active";
  $variables['is_active'] = $is_active;

  $r_script = l("$node->r_script", "node/$node->r_script");
  $variables['r_script'] = $r_script;
}
