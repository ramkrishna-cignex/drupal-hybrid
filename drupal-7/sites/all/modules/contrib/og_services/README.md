# Usage Examples

## Group Membership

Below are examples on how to C.R.U.D. Organic Group Memberships with JavaScript and JSON.

*Legend*:
- `type`: When using `POST`, this is the id for the `og_membership_type`, defaults to `1`. Otherwise it will be equal to the machine name of the membership type.
- `etid`: The target entity id, usually a `uid`.
- `entity_type`: The target entity type, usually `user`.
- `gid`: The group entity id, usually a `nid`.
- `group_type`: The group entity type, usually `node`.
- `state`: `1` for active, `2` for pending, `3` for blocked
- `roles`: If `entity_type` is user, this refers to the roles on the membership. It can only be set when `state` is equal to `1`.

### Create

HTTP **POST**: `[endpoint]/og_membership.json`

```
{
  "type": "1",
  "etid": "7",
  "entity_type": "user",
  "gid": "63",
  "group_type": "node",
  "state": "1",
  "roles":{"3":"administrator member"},
  "field_foo":{"und":[{"value":"123"}]}
}
```

Returns a `[1]` (aka `SAVED_NEW`) when successful, otherwise returns a `406` explaining why. You can also send the role id in place of the role name for simplicity, e.g. `"roles":{"3":"3"}`

### Retrieve

HTTP **GET**: `[endpoint]/og_membership/2.json`

```
{
  "id": "2",
  "type": "og_membership_type_default",
  "etid": "7",
  "entity_type": "user",
  "gid": "63",
  "group_type": "node",
  "state": "1", 
  "created": "1472135480",
  "field_name": "og_user_node",
  "language": "en",
  "og_membership_request": [],
  "rdf_mapping": [],
  "roles":{
    "3": "administrator member"
  },
  "field_foo":{"und":[{"value":"123"}]}
}
```

Returns a `JSON` object representation of the group membership via `GET`.

### Update

HTTP **PUT**: `[endpoint]/og_membership/2.json`
Content-type: application/json

```
{
  "etid": "7",
  "entity_type": "user",
  "gid": "63",
  "group_type": "node",
  "state": "3",
  "roles":{
    "3": 0,  // Revokes role 3.
    "4": "4" // Grants role 4.
  },
  "field_foo":{"und":[{"value":"456"}]}
}
```

Returns a `[2]` (aka `SAVED_UPDATED`) when successful, otherwise returns a `406` explaining why.

### Delete

HTTP **DELETE**: `[endpoint]/og_membership/2.json`

Returns a `[3]` (aka `SAVED_DELETED`) when successful, otherwise returns a `406` explaining why.

