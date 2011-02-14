<?php
// $Id: nodedeletebytype.drush.inc,v 1.1.2.8 2010/09/16 03:03:39 supakitk Exp $

/**
 * @file
 *   Drush support for nodedeletebytype.
 */

/**
 * Implementation of hook_drush_command().
 */
function nodedeletebytype_drush_command() {
  $items = array();

  $items['nodedeletebytype'] = array(
    'callback' => 'nodedeletebytype_drush_callback_delete_type',
    'description' => "Delete all nodes in the given content type.",
    'arguments' => array(
      'type' => 'The content type to delete.',
    ),    
    'examples' => array(
      'drush nodedeletebytype story' => 
        'Delete all nodes of type \'story\'.',
    ),
    'drush nodedeletebytype story' => 
      'Delete all nodes of type \'story\'.',
  );

  return $items;
}

/**
 * Implementation of hook_drush_help().
 *
 * This function is called whenever a drush user calls
 * 'drush help <name-of-your-command>'
 *
 * @param
 *   A string with the help section (prepend with 'drush:')
 *
 * @return
 *   A string with the help text for your command.
 */
function nodedeletebytype_drush_help($section) {
  switch ($section) {
    case 'drush:nodedeletebytype':
      return dt("Delete nodes. Usage: 'drush nodedeletebytype <nodetype>'.");
  }
}

function nodedeletebytype_node_delete_standard($nids) {
  foreach ($nids as $nid) {
    $node = node_load($nid);

    db_query('DELETE FROM {node} WHERE nid = %d', $node->nid);
    db_query('DELETE FROM {node_revisions} WHERE nid = %d', $node->nid);

    // Call the node-specific callback (if any):
    node_invoke($node, 'delete');
    node_invoke_nodeapi($node, 'delete');

    // Remove this node from the search index if needed.
    if (function_exists('search_wipe')) {
      search_wipe($node->nid, 'node');
    }
  }
  cache_clear_all();
}

/**
 * Drush comand callback.
 *
 * Export nodes of the provided type.
*/
function nodedeletebytype_drush_callback_delete_type($type) {
  if ($type == NULL) {
    drush_set_error('DRUSH_NOT_COMPLETED', 'Please supply the type of nodes you want to delete.');
  }

  $sql = "SELECT nid FROM {node} WHERE type = '%s'";
  $result = db_query($sql, $type);

  $nids = array();
  while ($row = db_fetch_array($result)) {
    $nids[] = $row['nid'];
  }

  if (!empty($nids)) {
    nodedeletebytype_node_delete_standard($nids);
  }
  else {
    drush_set_error('DRUSH_NOT_COMPLETED', "No nodes found of type $type.");
  }
}

