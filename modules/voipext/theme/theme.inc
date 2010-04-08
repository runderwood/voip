<?php
/**
 * @file
 *  Preprocess functions for voipext templates (*.tpl.php files).
 */

/**
 * Implemenation of template_preprocess().
 */
function template_preprocess_voipext(&$variables) {
  $variables = _voipext_details($variables);
}

/** 
 * Implementation of template_preprocess().
 */
function template_preprocess_voipext_created(&$variables) {
  $variables = _voipext_details($variables);

  // edit link
  $txt = t('edit');
  $nid = $variables['nid'];
  $edit = l($txt, "node/$nid/edit");
  $variables['edit'] = $edit; 

  // message about new extension
  $variables['msg'] = t('A new extension has been created:'); 
}

/**
 * Provide variables for voipext .tpl.php files, including 
 * details about each of the fields in the voipext node.
 */
function _voipext_details($variables) {
  if ($variables['node']) {
    $node = $variables['node'];
  } else if ($variables['nid']) {
    $node = node_load($variables['nid']);
  }
   
  // extension node nid
  $variables['nid'] = $node->nid;
 
  // extension number
  $ext = $node->voip_extension['ext'];
  if ($variables['page']) {
    $ext = "Extension Number $ext";
  } else { // page view
    $ext = l("Extension Number $ext", "node/$node->nid");
  }
  $variables['ext'] = $ext; 

  // related node
  $r_nid = $node->voip_extension['r_nid'];
  if ($r_nid) {
    $r_node = node_load($r_nid);
    $r_nid = l($r_node->title, "node/$r_nid"); 
    $variables['related_node'] = $r_nid; 
  }

  // related user
  $r_uid = $node->voip_extension['r_uid'];
  if ($r_uid) {
    $r_user = user_load($r_uid);
    $r_uid = l($r_user->name, "user/$r_uid");
    $variables['related_user'] = $r_uid;
  }
  
  $r_type = $node->voip_extension['r_type'];
  $variables['r_type'] = $r_type; 

  $is_active = ($node->voip_extension['is_active']) ? "Active" : "Not Active";
  $variables['is_active'] = $is_active;

  $script_nid = $node->voip_extension['r_script'];
  $r_script = l("$script_nid", "node/$script_nid");
  $variables['r_script'] = $r_script;

  return $variables; 
}


