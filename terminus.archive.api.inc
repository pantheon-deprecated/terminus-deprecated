<?php

/**
 * API Call to Check Archive Status
 */
function terminus_api_archive_check($site_uuid) {
  $realm = 'site';
  $uuid = $site_uuid;
  $path = 'archive';
  $method = 'GET';
  $data = NULL;
  return terminus_request($realm, $uuid, $path, $method, $data);
}

/**
 * API Call to Get Archive Download Info
 */
function terminus_api_archive_download_info($site_uuid, $archive) {
  $realm = 'site';
  $uuid = $site_uuid;
  $path = 'archive/' . str_replace('/', '%2F', $archive);
  $method = 'GET';
  $data = NULL;
  return terminus_request($realm, $uuid, $path, $method, $data);
}

/**
 * API Function to Make a Backup
 */
function terminus_api_environment_site_archive($site_uuid, $environment, $folder, $code = TRUE, $db = TRUE, $files = TRUE) {
  $realm = 'site';
  $uuid = $site_uuid;
  $path = 'environments/' . $environment . '/export';
  $method = 'POST';
  $data = array(
      'folder' => ($folder == 'downloads') ? $folder . '/' . 'latest/' . $environment : $folder. '/' . time() . '/' . $environment,
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
