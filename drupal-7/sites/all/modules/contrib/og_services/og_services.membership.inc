<?php

function _og_membership_services_resource_create($data) {

  // Validate incoming data.
  $valid = og_services_membership_has_valid_params($data);
  if ($valid !== TRUE) { return $valid; }

  // Load the target entity.
  $entity = entity_load_single($data['entity_type'], $data['etid']);
  if (!$entity) {
    return services_error(t('Invalid target entity during creation.'), 406);
  }

  // Add the user to the group. Use the incoming data, and cleanse it.
  $group_type = $data['group_type'];
  $gid = $data['gid'];
  $etid = $data['etid'];
  $data['entity type'] = $data['entity_type'];
  $data['entity'] = $entity;
  $data['membership type'] = isset($data['type']) ? $data['type'] : OG_MEMBERSHIP_TYPE_DEFAULT;
  if (isset($data['type'])) { unset($data['type']); }
  $roles = isset($data['roles']) ? $data['roles'] : NULL;
  unset($data['entity_type']);
  unset($data['etid']);
  unset($data['group_type']);
  unset($data['gid']);

  // Set aside any field values to save via a wrapper later.
  $query = db_select('og_membership_type', 'og')
    ->fields('og', array('name'))
    ->condition('og.id', $data['membership type'])
    ->execute();
  $result = $query->fetchObject();
  $fields = array();
  foreach ($data as $key => $value) {
    if (field_info_instance('og_membership', $key, $result->name)) {
      $fields[$key] = $value;
    }
  }
  if (!empty($fields)) {
    foreach($fields as $name => $value) {
      unset($data[$name]);
    }
  }

  // Finally save the membership.
  $membership = og_group($group_type, $gid, $data);

  // Set any incoming roles.
  if ($roles && !empty($roles)) {
    foreach ($roles as $rid => $name) {
      if (!$name) { continue; }
      og_role_grant($group_type, $gid, $etid, $rid);
    }
  }

  // Save any field data.
  if (!empty($fields)) {
    foreach ($fields as $name => $value) {
      $membership->{$name} = $value;
    }
    $membership->save();
  }

  return SAVED_NEW;
}

function _og_membership_services_resource_retrieve($id) {
  $membership = og_membership_load($id);
  if ($membership->entity_type == 'user') {
    $roles = og_get_user_roles($membership->group_type, $membership->gid, $membership->etid, FALSE);
    if (!empty($roles)) { $membership->roles = $roles; }
  }
  return $membership;
}

function _og_membership_services_resource_update($id, $data) {

  // Load the membership.
  $membership = og_membership_load($id);

  // Place the incoming data onto the membership object and change it.
  foreach ($data as $key => $value) {
    $membership->{$key} = $value;
  }
  $membership->save();

  // Grab the membership's roles, if any.
  $current_roles =  og_get_user_roles($data['group_type'], $data['gid'], $data['etid'], FALSE);
  $has_role = !empty($current_roles);

  // Add/remove any incoming roles...
  if (isset($data['roles'])) {

    // If there are some incoming roles...
    if (!empty($data['roles'])) {
      $granted = []; // Track which roles have been, or already were, granted.

      // For each incoming role...
      foreach ($data['roles'] as $rid => $name) {

        if (!$name) { continue; }

        // If they already have the role, skip it.
        if ($has_role && in_array($rid, array_keys($current_roles))) {
          $granted[] = $rid;
          continue;
        }

        // They don't already have the role, grant it.
        if ($membership->state == 1) {
          og_role_grant($data['group_type'], $data['gid'], $data['etid'], $rid);
          $granted[] = $rid;
        }

      }

      // Revoke any roles that were not granted.
      foreach ($current_roles as $rid => $name) {
        if (!in_array($rid, $granted)) {
          og_role_revoke($data['group_type'], $data['gid'], $data['etid'], $rid);
        }
      }
    }

    // Otherwise if they have an current roles, revoke them all.
    else if ($has_role) {
      foreach ($current_roles as $rid => $name) {
        og_role_revoke($data['group_type'], $data['gid'], $data['etid'], $rid);
      }
    }

  }

  return SAVED_UPDATED;
}

function _og_membership_services_resource_delete($id) {
  $membership = og_membership_load($id);
  if (!$membership) {
    return services_error(t('There is no group membership with ID @id.', array('@id' => $id)), 404);
  }
  entity_delete('og_membership', $membership->id);
  return SAVED_DELETED;
}
