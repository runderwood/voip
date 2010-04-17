<?php
/**
 * @file
 *  Preprocess functions for voipext templates (*.tpl.php files).
 */

/**
 * Implemenation of template_preprocess().
 * 
 * Preprocess voipext node. 
 */
function template_preprocess_voipext(&$variables) {
  $variables = _voipext_details($variables['node']);
}

/** 
 * Implementation of template_preprocess().
 * 
 * Preprocess elements of message displayed to user where an extension is created. 
 */
function template_preprocess_voipext_created(&$variables) {
  // get general extension details
  $node = node_load($variables['nid']);
  $variables = _voipext_details($node);

  // edit link
  $txt = t('edit');
  $nid = $variables['nid'];
  $edit = l($txt, "node/$nid/edit");
  $variables['edit'] = $edit; 

  // message about new extension
  $variables['msg'] = t('A new extension has been created:'); 
}

/** 
 * Implementation of template_preprocess().
 *  
 * Preprocess extension to display along with related node or user. 
 */
function template_preprocess_voipext_ext(&$variables) {
  $variables = _voipext_details($variables['voipext_node']);
}

/**
 * @param $node
 *  voipext node
 * 
 * Pre-process variables for voipext .tpl.php files, including 
 * general details about extensions in the voipext node.
 */
function _voipext_details($node) {
  // extension node nid
  $variables['nid'] = $node->nid;
 
  // extension number
  $ext = $node->voipext_info['ext'];
  if ($variables['page']) {
    $ext = "Extension Number $ext";
  } else { // page view
    $ext = l("Extension Number $ext", "node/$node->nid");
  }
  $variables['ext'] = $ext; 

  // related node
  $r_nid = $node->voipext_info['r_nid'];
  if ($r_nid) {
    $r_node = node_load($r_nid);
    $r_nid = l($r_node->title, "node/$r_nid"); 
    $variables['related_node'] = $r_nid; 
  }

  // related user
  $r_uid = $node->voipext_info['r_uid'];
  if ($r_uid) {
    $r_user = user_load($r_uid);
    $r_uid = l($r_user->name, "user/$r_uid");
    $variables['related_user'] = $r_uid;
  }
  
  $r_type = $node->voipext_info['r_type'];
  $variables['r_type'] = $r_type; 

  $is_active = ($node->voipext_info['is_active']) ? "Active" : "Not Active";
  $variables['is_active'] = $is_active;

  $script_nid = $node->voipext_info['r_script'];
  $r_script = l("$script_nid", "node/$script_nid");
  $variables['r_script'] = $r_script;

  return $variables; 
}


