<?php
 /**
 * @file
 *  Terminus: programatic access to the Pantheon Platform via Drush.
 */

/**
 * Some constants we use.
 */
define('TERMINUS_HOST', 'terminus.getpantheon.com');
define('TERMINUS_PORT', 443);

/**
 * An easy function to override constants to allow development internally.
 *
 * TODO: use for other settings.
 */
function terminus_bootstrap() {
  $data = terminus_session_data();
  if (drush_get_option('onebox')) {
    define('TERMINUS_HOST', 'onebox.getpantheon.com');
  }
}

/**
 * Implementation of hook_drush_command().
 *
 * List of available terminus commands.
 *
 * @return
 *   An associative array describing your command(s).
 */
function terminus_drush_command() {
  $items = array();

  // The 'pantheon-authenticate' command
  $items['pantheon-auth'] = array(
    'description' => "Authenticate against the pantheon dashboard. Required before doing anything else.",
    'arguments' => array(
      'email' => 'Email address of dashboard account',
    ),
    'options' => array(
      'password' => array(
        'description' => 'Optional: include password for script-friendly use.',
        'example-value' => 'mypassword',
      ),
      'nocache' => array(
        'description' => 'Force a refresh of cached authentication session.',
      ),
    ),
    'examples' => array(
      'drush pauth josh@getpantheon.com --password=mypassword' => 'Get authentication token.',
    ),
    'aliases' => array('pauth'),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH, // No bootstrap at all.
  );

  $items['pantheon-logout'] = array(
    'description' => "Clear any stored session data.",
    'examples' => array(
      'drush plogout',
    ),
    'aliases' => array('plogout'),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH, // No bootstrap at all.
  );

  // The 'pantheon-sitelist' command
  $items['pantheon-sites'] = array(
    'description' => "List your sites.",
    'options' => array(
      'nocache' => array(
        'description' => 'Force a refresh of cached authentication session.',
      ),
      'json' => array(
        'description' => 'Return output in JSON. Useful for scripting.',
      ),
    ),
    'examples' => array(
      'drush psites --nocache' => 'Get a fresh list of sites.',
    ),
    'aliases' => array('psites'),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH, // No bootstrap at all.
  );

  $items['pantheon-hostname-add'] = array(
    'description' => "Add a hostname to a site you control.",
    'arguments' => array(
      'site' => 'The site UUID you are working on.',
      'environment' => 'The environemnt (e.g. "live").',
      'hostname' => 'The hostname (e.g. "www.mysite.com")',
    ),
    'aliases' => array('phost-add'),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH, // No bootstrap at all.
  );

  $items['pantheon-hostname-remove'] = array(
    'description' => "Remove a hostname from a site you control.",
    'arguments' => array(
      'site' => 'The site UUID you are working on.',
      'environment' => 'The environemnt (e.g. "live").',
      'hostname' => 'The hostname (e.g. "www.mysite.com")',
    ),
    'aliases' => array('phost-remove'),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH, // No bootstrap at all.
  );

  $items['pantheon-products'] = array(
    'description' => "Get the list of available Drupal product start-states.",
    'options' => array(
      'nocache' => array(
        'description' => 'Force a refresh of cached authentication session.',
      ),
      'json' => array(
        'description' => 'Return output in JSON. Useful for scripting.',
      ),
    ),
    'aliases' => array('pproducts'),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH, // No bootstrap at all.
  );

  $items['pantheon-site-create'] = array(
    'description' => "Create a new site on Pantheon",
    'arguments' => array(
      'name' => 'Shortname of the site to create. Will be part of the url.',
    ),
    'options' => array(
      'label' => array(
        'description' => 'Human-friendly site label.',
      ),
      'product' => array(
        'description' => 'UUID of the product you want to start with (see pantheon-products).',
      ),
      'organization' => array(
        'description' => 'UUID of an organization you want the site to be a part of.',
      ),
    ),
    'aliases' => array('psite-create'),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH, // No bootstrap at all.
  );

  $items['pantheon-site-delete'] = array(
    'description' => "Delete a site from Pantheon.",
    'arguments' => array(
      'site' => 'UUID of the site you want to delete.',
    ),
    'aliases' => array('psite-delete'),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH, // No bootstrap at all.
  );

  $items['pantheon-aliases'] = array(
    'description' => "Update the Pantheon Drush alias file at ~/.drush/pantheon.aliases.drushrc.php.",
    'options' => array(
      'destination' => array(
        'description' => 'Specify the destination to save the alias file.',
      ),
    ),
    'aliases' => array('paliases'),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH, // No bootstrap at all.
  );

  $items['pantheon-pp'] = array(
    'description' => "Direct pseudoproxy interface. JSON only, for debugging.",
    'arguments' => array(
      'realm' => 'Are you asking about users or sites?',
      'uuid' => 'The unique id of the thing you want to know about.',
      'path' => 'Optional: path extension for more specific commands.',
      'json' => 'Optional: return simple JSON; good for scripting.',
    ),
    'aliases' => array('pp'),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH, // No bootstrap at all.
  );

  $items['pantheon-whoami'] = array(
    'description' => "Information about you the user.",
    'aliases' => array('pme'),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH, // No bootstrap at all.
  );

  return $items;
}


/**
 * @group Drush Commands
 *
 * These commands exercise various aspects of the API.
 */

/**
 * User data about self.
 */
function drush_terminus_pantheon_whoami() {
  $data = terminus_session_data();
  if (!$data) {
    drush_log('You are not authenticated', 'failed');
    return FALSE;
  }
  drush_print(dt("User UUID: !uuid", array('!uuid' => $data['user_uuid'])));
  drush_print(dt("Email: !email", array('!email' => $data['email'])));
  return TRUE;
}

/**
 * Create a new site via the CLI.
 */
function drush_terminus_pantheon_site_create($name = FALSE) {
  $data = terminus_session_data();
  if (!$data) {
    drush_log('You are not authenticated', 'failed');
    return FALSE;
  }
  if (!$name) {
    drush_log('You must supply a site name', 'failed');
    return FALSE;
  }

  # Construct payload. We don't want empty/null values.
  $site_data = array();
  $site_data['name'] = preg_replace("/[^A-Za-z0-9 ]/", '', $name);
  if (drush_get_option('label')) {
    $site_data['label'] = drush_get_option('label');
  }
  if (drush_get_option('organization')) {
    $site_data['organization'] = drush_get_option('organization');
  }
  if (drush_get_option('product')) {
    $site_data['product'] = drush_get_option('product');
  }

  $result = terminus_request('user', $data['user_uuid'], 'sites', POST, $site_data);
  if (!$result) {
    drush_log("Site create failed", "failed");
    return FALSE;
  }
  if (drush_get_option('json')) {
    drush_print(json_decode($result['json']));
    return TRUE;
  }
  drush_log("Site is building now in your dashboard.", "ok");
  drush_print("The new site's UUID is '". json_decode($result['json']) ."'");
  drush_cache_clear_all('terminus-sites', 'pantheon');
  return TRUE;
}

/**
 * Create a new site via the CLI.
 */
function drush_terminus_pantheon_site_delete($site_uuid = FALSE) {
  $data = terminus_session_data();
  if (!$data) {
    drush_log('You are not authenticated', 'failed');
    return FALSE;
  }
  if (!$site_uuid) {
    drush_log('You must supply a site UUID', 'failed');
    return FALSE;
  }
  $confirm = drush_confirm("Are you sure you want to delete?");
  if (!$confirm) {
    return TRUE;
  }
  $result = terminus_request('site', $site_uuid, '', DELETE);
  drush_log("Site deleted.", "ok");
  drush_cache_clear_all('terminus-sites', 'pantheon');
  return TRUE;
}


/**
 * Update alias list.
 */
function drush_terminus_pantheon_aliases() {
  $data = terminus_session_data();
  $default_destination = FALSE;
  $home = drush_server_home();
  if ($home) {
    $default_destination = drush_normalize_path($home . '/.drush/pantheon.aliases.drushrc.php');
  }
  $destination = drush_get_option('destination', $default_destination);
  if ($destination == FALSE) {
    drush_log('Could not auto-determine destination. Please provide one using the --destination option.', 'failed');
    return FALSE;
  }
  $fh = fopen($destination, 'w');
  if ($fh === FALSE) {
    drush_log('Cannot write aliases to "'. $destination .'" - verify this location is writable.', 'failed');
    return FALSE;
  }
  if (!$data) {
    drush_log('You are not authenticated', 'failed');
    return FALSE;
  }
  drush_log('Loading aliases from Pantheon.', 'notice');
  // This one is a little wonky still. Goes to Drupal.
  $ch = curl_init();
  $url = 'https://'. TERMINUS_HOST .'/users/'. $data['user_uuid'] .'/drushrc';
  curl_setopt($ch, CURLOPT_COOKIE, $data['session']);
  curl_setopt($ch, CURLOPT_URL, $url);
  curl_setopt($ch, CURLOPT_HEADER, 0);
  curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
  if (TERMINUS_HOST == 'onebox.getpantheon.com') {
    curl_setopt($ch, CURLOPT_SSL_VERIFYPEER, FALSE);
    curl_setopt($ch, CURLOPT_SSL_VERIFYHOST, FALSE);
  }

  $result = curl_exec($ch);
  if ($result === FALSE) {
    drush_log('Failed fetching aliases. Try again?');
    return FALSE;
  }
  drush_log('Writing aliases to "'. $destination .'".', 'notice');
  fwrite($fh, $result);
  fclose($fh);
  drush_log('Pantheon aliases updated!.', 'ok');

  return TRUE;
}


/**
 * Get a list of sites.
 */
function drush_terminus_pantheon_sites() {
  $user_uuid = terminus_session_data('user_uuid');
  $nocache = drush_get_option('nocache');
  $cid = 'terminus-sites';
  $json_output = drush_get_option('json');
  if (!$user_uuid) {
    return drush_set_error("No user UUID found. Please authenticate first: drush pauth");
  }
  if ($nocache || !$cache = drush_cache_get($cid, 'pantheon')) {
    drush_log('Loading site data from Pantheon.', 'notice');
    $result = terminus_request('user', $user_uuid, 'sites');
    if ($result === FALSE) {
      return FALSE;
    }
    $sites = $result['json'];
    drush_cache_set($cid, $sites, 'pantheon');
  }
  else {
    drush_log('Loaded site data from cache.', 'notice');
    $sites = $cache->data;
  }
  if ($json_output) {
    drush_print($sites);
    return TRUE;
  }

  // Display tabular output.
  $sites = json_decode($sites);
  $rows = array();
  foreach ($sites as $site_uuid => $data) {
    $i = $data->information;
    $rows[$i->name] = array($i->name, $i->service_level, $site_uuid);
  }
  ksort($rows);
  array_unshift($rows, array("Site", "Service Level", "UUID"));
  drush_print_table($rows, TRUE);
}

/**
 * API Debugging command. Try out anything you like.
 */
function drush_terminus_pantheon_pp($realm, $uuid, $path=FALSE) {
  $result = terminus_request($realm, $uuid, $path);
  if ($result === FALSE) {
    return FALSE;
  }
  return TRUE;
}

/**
 * Add a hostname.
 */
function drush_terminus_pantheon_hostname_add($site, $environment, $hostname) {
  $data = array('site' => $site, 'environment' => $environment);
  $path = 'environments/'. $environment .'/hostnames/'. rawurlencode($hostname);
  $result = terminus_request('site', $site, $path, 'PUT', $data);
  if ($result === FALSE) {
    return FALSE;
  }
  drush_log("Added '$hostname' to the $environment environment", 'ok');
  return TRUE;
}

/**
 * Remove a hostname.
 */
function drush_terminus_pantheon_hostname_remove($site, $environment, $hostname) {
  $path = 'environments/'. $environment .'/hostnames/'. rawurlencode($hostname);
  $result = terminus_request('site', $site, $path, 'DELETE');
  if ($result === FALSE) {
    return FALSE;
  }
  drush_log("Removed '$hostname' from the $environment environment", 'ok');
  return TRUE;
}

/**
 * Get the list of products.
 */
function drush_terminus_pantheon_products() {
  $nocache = drush_get_option('nocache');
  $json_output = drush_get_option('json');
  $cid = 'terminus-products';
  if ($nocache || !$cache = drush_cache_get($cid, 'pantheon')) {
    drush_log('Loading site data from Pantheon.', 'notice');
    $result = terminus_request('public', 'public', 'products');
    if ($result === FALSE) {
      return FALSE;
    }
    $products = $result['json'];
    drush_cache_set($cid, $products, 'pantheon');
  }
  else {
    drush_log('Loaded site data from cache.', 'notice');
    $products = $cache->data;
  }
  if ($json_output) {
    drush_print($products);
    return TRUE;
  }

  // Display tabular output.
  $products = json_decode($products);
  $rows = array();
  foreach ($products as $product_uuid => $data) {
    $i = $data->attributes;
    if ($i->organization_uuid == '') {
      $rows[$i->machinename] = array($i->longname, $i->author, $product_uuid);
    }
  }
  ksort($rows);
  array_unshift($rows, array("Product", "Maker", "UUID"));
  drush_print_table($rows, TRUE);
}

/**
 * Get rid of the current session.
 */
function drush_terminus_pantheon_logout() {
  drush_cache_clear_all('*', 'pantheon', TRUE);
  drush_log('Removed session token and cleared all locally cached data.', 'ok');
}

/**
 * Get or set current session data from cache.
 */
function terminus_session_data($key = FALSE) {
  static $cache = FALSE;
  if (!$cache) {
    $cache = drush_cache_get('terminus-current-session', 'pantheon');
    if (time() - $cache->created > 3600) {
      // Only allow cached auth for an hour.
      drush_cache_clear_all('*', 'pantheon', TRUE);
      return FALSE;
    }
  }
  if (!$cache) {
    // No session data. That's not good.
    return FALSE;
  }
  if ($key && isset($cache->data[$key])) {
    return $cache->data[$key];
  }
  elseif($key) {
    return FALSE;
  }
  return $cache->data;
}

/**
 * Validate cached session data.
 */
function terminus_validate_session($data) {
  $result = terminus_request('user', $data['user_uuid'], 'email');
  if ($result === FALSE) {
    return FALSE;
  }
  $email = @json_decode($result['json']);
  return $email == $data['email'];
}

/**
 * Make a request to the Dashbord's internal API.
 *
 * @param $realm
 *    Permissions realm for data request: currently "user" or "site" but in the
 *    future this could also be "orgnaization" or another high-level business
 *    object (e.g. "product" for managing your app). Can also be "public" to
 *    simply pull read-only data that is not privileged.
 *
 * @param $uuid
 *    The uuid of the item in the realm you want to access.
 *
 * @param $method
 *    HTTP method (verb) to use.
 *
 * @param $data
 *    A native php data structure (int, string, arary or simple object) to be
 *    sent along with the request. Will be encoded as JSON for you.
 */
function terminus_request($realm, $uuid, $path = FALSE, $method = 'GET', $data = NULL) {
  static $ch = FALSE;
  if (!$ch) {
    $ch = curl_init();
  }
  $headers = array();
  $session  = terminus_session_data('session');
  // TODO: don't use pp.php.
  // We will need something else we can lock down if needed.
  $url = 'https://'. TERMINUS_HOST .'/terminus.php?'. $realm .'='. $uuid;
  if ($path) {
    $url .= '&path='. urlencode($path);
  }
  if ($data) {
    // $data for POSTs, PUTs, DELETEs are sent as JSON.
    if ($method === 'POST' || $method === 'PUT' || $method === 'DELETE') {
      $data = json_encode(array('data' => $data));
      curl_setopt($ch, CURLOPT_POST, 1);
      curl_setopt($ch, CURLOPT_POSTFIELDS, $data);
      curl_setopt($ch, CURLOPT_BINARYTRANSFER, TRUE);
      array_push($headers, 'Content-Type: application/json', 'Content-Length: '. strlen($data));
    }
    // $data for GETs is sent as querystrings.
    else if ($method === "GET") {
      $url .= "?" . http_build_query($data);
    }
  }
  // set URL and other appropriate options
  $opts = array(
    CURLOPT_URL => $url,
    CURLOPT_HEADER => 1,
    CURLOPT_PORT => TERMINUS_PORT,
    CURLOPT_RETURNTRANSFER => 1,
    CURLOPT_TIMEOUT => 5,
    CURLOPT_CUSTOMREQUEST => $method,
    CURLOPT_COOKIE => $session,
    CURLOPT_HTTPHEADER => $headers,
  );
  curl_setopt_array($ch, $opts);

  if (TERMINUS_HOST == 'onebox.getpantheon.com') {
    // Allow debugging to work against a onebox.
    curl_setopt($ch, CURLOPT_SSL_VERIFYPEER, FALSE);
    curl_setopt($ch, CURLOPT_SSL_VERIFYHOST, FALSE);
  }

  // grab URL and pass it to the browser
  drush_log('Making api request.', 'notice');
  drush_log($url, 'notice');
  $result = curl_exec($ch);
  list($headers_text, $json) = explode("\r\n\r\n", $result, 2);
  // Work around extra 100 Continue headers - http://stackoverflow.com/a/2964710/1895669
  if(strpos($headers_text," 100 Continue")!==false){
    list($headers_text, $json) = explode( "\r\n\r\n", $json , 2);
  }

  if (curl_errno($ch) != 0) {
    $error = curl_error($ch);
    curl_close($ch);
    drush_log("CONNECTION ERROR: $error", 'failed');
    return FALSE;
  }
  $info = curl_getinfo($ch);
  if ($info['http_code'] > 399) {
    drush_log('Request failed: '. $info['http_code']. ' '. $json, 'failed');
    return FALSE;
  }

  // TODO: nice to have a timer here for how long the request took.
  drush_log('Request complete.', 'notice');
  drush_log("Respnse headers:\n" .$headers_text, 'notice');
  drush_log("JSON response:\n" . $json, 'notice');
  return array('headers' => $headers_text, 'json' => $json);
}

/**
 * Clean up output so there are consistent service level names.
 */
function _terminus_service_level_name($level) {
  $levels = array(
    'legacy_basic' => 'personal',
    'basic' => 'personal',
    'legacy_pro' => 'pro',
  );
  if (isset($levels[$level])) {
    return $levels[$level];
  }
  return $level;
}


/**
 * This is a curl-based tool to start a session in the dashboard.
 *
 * It doesn't follow the normal pattern since it's working off Drupal's login
 * forms directly. This will be refactored when there's a direct CLI auth
 * mechanism in the API itself.
 *
 * Many thanks to Amitai and the gang at: https://drupal.org/node/89710
 *
 */
function drush_terminus_pantheon_auth($email=FALSE) {
    // Flush existing data.
    drush_cache_clear_all('*', 'pantheon', TRUE);
    if (!$email) {
      $email = drush_prompt(dt('Pantheon account email address'), NULL, TRUE);
    }
    $nocache = drush_get_option('nocache');
    $session_data = terminus_session_data();
    if (!$nocache && $session_data) {
      drush_log(dt('Cached auth found for !email', array('!email' => $email)), 'notice');
      if (terminus_validate_session($session_data)) {
        drush_log('Session still valid. Good to go!', 'ok');
        return TRUE;
      }
    }
    $password = drush_get_option('password');
    if (!$password) {
      // Hide password input in the clear.
      // TODO: consider an alternative ways to start a session using a shared
      // secret key. This would allow us to differentiate between dashboard
      // users and drush users.
      /*
      drush_print("Pantheon dashboard password: ");
      system('stty -echo');
      $password = trim(fgets(STDIN));
      system('stty echo');
      */
      $password = drush_prompt(dt('Pantheon dashboard password'), NULL, TRUE, TRUE);
    }
    drush_log(dt('Authenticating as !email', array('!email' => $email)), 'ok');
    $ch = curl_init();
    $url = 'https://'. TERMINUS_HOST .'/login';
    // set URL and other appropriate options
    curl_setopt($ch, CURLOPT_URL, $url);
    curl_setopt($ch, CURLOPT_HEADER, 0);
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
    if (TERMINUS_HOST == 'onebox.getpantheon.com') {
      curl_setopt($ch, CURLOPT_SSL_VERIFYPEER, FALSE);
      curl_setopt($ch, CURLOPT_SSL_VERIFYHOST, FALSE);
    }

    // grab URL and pass it to the browser
    $result = curl_exec($ch);

    if (curl_errno($ch) != 0) {
      $err = curl_error($ch);
      return drush_set_error("Dashboard unavailable", "Dashboard unavailable: $err");
    }

    // Parse form build ID.
    $DOM = new DOMDocument;
    @$DOM->loadHTML($result);
    $login_form = $DOM->getElementById('atlas-login-form');
    if (!$login_form) {
      return drush_set_error("Dashboard unavailable", "Dashboard unavailable: login endpoint not found.");
    }

    foreach ($login_form->getElementsByTagName('input') as $input) {
      if ($input->getAttribute('name') == 'form_build_id') {
        $form_build_id = $input->getAttribute('value');
        break;
      }
    }

    // Now login.
    $login_data = array('email' => $email,
                        'password' => $password,
                        'form_build_id' => $form_build_id,
                        'form_id' => 'atlas_login_form',
                        'op' => 'Login');
    curl_setopt($ch, CURLOPT_POST, 1);
    curl_setopt($ch, CURLOPT_POSTFIELDS, $login_data);
    curl_setopt($ch, CURLOPT_HEADER, 1);
    $result = curl_exec($ch);

    if (curl_errno($ch) != 0) {
      $err = curl_error($ch);
      return drush_set_error("Dashboard unavailable", "Dashboard unavailable: $err");
    }

    $set_cookie_header = terminus_parse_drupal_headers($result, 'Set-Cookie');
    if (!$set_cookie_header) {
      return drush_set_error("Failure!", 'Authentication failed. Please check your credentials and try again.');
    }
    $session = FALSE;
    $set_cookie = explode('; ', $set_cookie_header);
    foreach ($set_cookie as $cookie) {
      if (strpos($cookie, 'SSESS') === 0) {
        $session = $cookie;
      }
    }
    if (!$session) {
      return drush_set_error("Failure!", 'Session not found. Please check your credentials and try again.');
    }

    // Grab the UUID Now
    curl_setopt($ch, CURLOPT_COOKIE, $session);
    curl_setopt($ch, CURLOPT_URL, 'https://'. TERMINUS_HOST .'/account');
    $result = curl_exec($ch);
    $location_header = terminus_parse_drupal_headers($result, 'Location');
    if (!$location_header) {
      return drush_set_error("Failure!", 'Could not determine user UUID. Please check your credentials and try again.');
    }
    $user_uuid = array_pop(explode('/', $location_header));

    drush_log('Success!', 'ok');
    drush_log(dt('User UUID: !user_uuid', array('!user_uuid' => $user_uuid)), 'notice');

    // Start socking away new data.
    $data = array('user_uuid' => $user_uuid, 'session' => $session, 'email' => $email);
    drush_cache_set('terminus-current-session', $data, 'pantheon');

    // Store the session in drush cache.
    // close cURL resource, and free up system resources
    curl_close($ch);
}


/**
 * Helper function for parsing Drupal headers for login.
 */
function terminus_parse_drupal_headers($result, $target_header='Set-Cookie') {
  // Check that we have a 302 and a session.
  list ($headers_text, $html) = explode("\r\n\r\n", $result, 2);
  if (strpos($headers_text, "100 Continue") !== false) {
    list ($headers_text, $html) = explode("\r\n\r\n", $html , 2);
  }
  $header_lines = explode("\r\n", $headers_text);
  $status = array_shift($header_lines);
  if (strpos($status, "302 Moved Temporarily") === FALSE) {
    return FALSE;
  }
  $headers = array();
  foreach ($header_lines as $line) {
    $parts = explode(': ', $line);
    if (isset($parts[1])) {
      $headers[$parts[0]] = $parts[1];
    }
  }
  if (isset($headers[$target_header])) {
    return $headers[$target_header];
  }

  return FALSE;
}
