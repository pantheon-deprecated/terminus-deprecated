<?php

/**
 * API call to get the backup catalog.
 */
function terminus_api_backup_catalog($site_uuid, $environment) {
  $realm = 'site';
  $uuid = $site_uuid;
  $path = 'environments/'. $environment .'/backups/catalog';
  $method = 'GET';
  $data = NULL;
  return terminus_request($realm, $uuid, $path, $method, $data);
}

/**
 * API call to get a single backup item's S3 token
 */
function terminus_api_backup_download_url($site_uuid, $environment, $bucket, $element) {
  $realm = 'site';
  $uuid = $site_uuid;
  $path = 'environments/'. $environment .'/backups/catalog/'. $bucket .'/'. $element .'/s3token';
  $method = 'POST';
  $data = array('method' => 'GET');
  return terminus_request($realm, $uuid, $path, $method, $data);
}

/**
 * API Function to make a backup.
 */
function terminus_api_environment_site_backup($site_uuid, $environment, $entry_type = 'export', $code = TRUE, $db = TRUE, $files = TRUE) {
  $realm = 'site';
  $uuid = $site_uuid;
  $path = 'environments/' . $environment . '/backups/create';
  $method = 'POST';
  $data = array(
      'entry_type' => $entry_type,
      'scheduled_for' => time(),
      'code' => ($code) ? 1 : 0,
      'database' => ($db) ? 1 : 0,
      'files' => ($files) ? 1 : 0,
  );
  return terminus_request($realm, $uuid, $path, $method, $data);
}

/**
 * API Call to get the backup schedule.
 */
function terminus_api_schedule_backup_list($site_uuid, $environment) {
  $realm = 'site';
  $uuid = $site_uuid;
  $path = 'environments/' . $environment . '/backups/schedule';
  $method = 'GET';
  return terminus_request($realm, $uuid, $path, $method);
}

/**
 * API Call to PUT a single backup schedule.
 *
 * $day = integer 0 to 6 (0 = sunday).
 *
 * $hour = hour of day to perform the backup.
 *
 * $ttl = time to keep in seconds.
 */
function terminus_api_schedule_backup($site_uuid, $environment, $day, $hour, $ttl) {
  $realm = 'site';
  $uuid = $site_uuid;
  $path = 'environments/' . $environment . '/backups/schedule';
  $method = 'PUT';
  $data = json_encode(
    array($day => array('hour' => $hour, 'ttl' => $ttl))
  );
  return terminus_request($realm, $uuid, $path, $method, $data);
}

/**
 * API Call to accept an array of backup days. Should be like so:
 *
 * $batch = array(0 => array('hour' => 12, 'ttl' => 86400))
 */
function terminus_api_schedule_backup_batch($site_uuid, $environment, $batch) {
  $realm = 'site';
  $uuid = $site_uuid;
  $path = 'environments/' . $environment . '/backups/schedule';
  $method = 'PUT';
  $data = json_encode($batch);
  return terminus_request($realm, $uuid, $path, $method, $data);
}

/**
 * API Call to DELETE the backup schedule.
 *
 * $day = the day to delete. If not specified, all schedules are wiped!
 */
function terminus_api_schedule_backup_delete($site_uuid, $environment, $day = FALSE) {
  $realm = 'site';
  $uuid = $site_uuid;
  $path = 'environments/' . $environment . '/backups/schedule';
  if ($day) {
    $path .= "?day=$day";
  }
  $method = 'DELETE';
  return terminus_request($realm, $uuid, $path, $method);
}
