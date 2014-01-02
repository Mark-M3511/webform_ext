<?php
  
//  _webform_ext_build_reservation_form
// require_once(drupal_get_path('module', 'webform_ext'). '/includes/webform_ext_consts.inc'); 
// require_once(drupal_get_path('module', 'webform_ext'). '/includes/webform_ext_funcs.inc');

module_load_include('inc', 'webform_ext', 'includes/webform_ext_consts');
module_load_include('inc', 'webform_ext', 'includes/webform_ext_funcs');

function _webform_ext_reservation_form_1($form, &$form_state)
{
   $form = array();
   
   $d = new DateTime(date('Y-m-d', mktime(0,0,0)), new DateTimeZone(WEBFORM_EXT_DEFAULT_TIMEZONE));
   $d->modify(WEBFORM_EXT_MIN_DATE_FOR_RESERVATION);
   $arrival_date = !empty($form_state['values']['arrival_date']) ? $form_state['values']['arrival_date'] : $d->format('Y-m-d');
   
   $d->modify(WEBFORM_EXT_MIN_DATE_FOR_RESERVATION_2);
   $departure_date = !empty($form_state['values']['departure_date']) ? $form_state['values']['departure_date'] : $d->format('Y-m-d');
   
   $_SESSION['num_rooms_available_sa'] = 0;
   $_SESSION['num_rooms_available_pa'] = 0;
   
   $errors = array();
   if (_webform_ext_check_date_errors($arrival_date, $departure_date, $errors) == 0){
      _query_num_rooms_available($arrival_date, $departure_date);
   }
   
   $form['fs_availability_info'] = array(
     /* '#title' => t("Length of stay:"), */
	 '#title' => t("Step 1:"),
	 '#prefix' => '<div id="box">',
     '#suffix' => '</div>', 
     '#type' => 'fieldset',
   );
   
   $form['fs_availability_info']['arrival_date'] = array(
      '#title' => t('Arrival Date'),
      '#type' => 'date_popup',
	  '#default_value' => $arrival_date,
	  '#date_format' => 'Y-m-d',
	  '#prefix' => '<div id="edit-arrival-date-container">',
      '#suffix' => '</div>',
   );
   
   $form['fs_availability_info']['departure_date'] = array(
      '#title' => t('Departure Date'),
      '#type' => 'date_popup',  
	  '#default_value' => $departure_date,
	  '#date_format' => 'Y-m-d',
	  '#prefix' => '<div id="edit-departure-date-container">',
      '#suffix' => '</div>',
   );  
   
   $form['fs_availability_info']['check_availability_msg'] = array(
      '#type' => 'markup', 
	  '#markup' => "<ol><li>Enter 'Arrival' and 'Departure' dates then click <span style=\"font-weight: bold;\">'Check Room Availability'</span>.</li><li>Select rooms with private or shared baths to reserve</li><li>Select up to 4 guests per room.</li><li>Click <span style=\"font-weight: bold;\">'Reserve Now'</span> </li></ol>",      
// 	  '#prefix' => '<div id="check-availability-msg">',
	  '#prefix' => '<div class="instructions">',
      '#suffix' => '</div>',
   );
      
   $form['fs_availability_info']['check_availability'] = array(
      '#type' => 'submit',
	  '#name' => 'ca',
	  '#executes_submit_callback' => FALSE,
      '#ajax' => array(
         'callback' => 'ajax_load_available_rooms_callback',
		 'wrapper' => 'box-msg',
         'name' => 'submit_ca',		 
       ),
      '#value' => t('Check Room Availability'),
	  '#prefix' => '<div id="check-availability">',
      '#suffix' => '</div>',
   );
   
   $form['fs_availability_info']['check_blocked_dates'] = array(	  
      '#type' => 'markup', 
      '#markup' => _get_check_blocked_dates_link(strtotime($arrival_date)),
	  '#prefix' => '<div id="check-blocked-dates">',
      '#suffix' => '</div>',
   );
   
   $form['fs_rooms'] = array(
	 '#title' => t("Step 2 - Room types available:"),
// 	 '#description' => t("Select number of rooms to reserve."),
	 '#prefix' => '<div id="rooms-div">',
     '#suffix' => '</div>',
     '#type' => 'fieldset',
   ); 
   
   // error_log("Res Form: \$arrival_date = {$arrival_date} | \$departure_date = {$departure_date}");
   
   $form['fs_rooms']['box_msg'] = array(
	  '#markup' => _get_availability_msg(),
      '#type' => 'markup', 
	  '#prefix' => '<div id="box-msg">',
      '#suffix' => '</div>',
   );
   
	// Render room selection controls    
   _webform_ext_reservation_pa_selector($form, $form_state);      
   _webform_ext_reservation_sa_selector($form, $form_state);    
   
   // Render room lists sections
   _webform_ext_reservation_pa_list_section($form, $form_state);   
   _webform_ext_reservation_sa_list_section($form, $form_state);
   
   
   $form['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Reserve Now'),
	  '#weight' => 99,
	  '#validate' => array('_webform_ext_reservation_validate_reservation'),
    );
   
   return $form;
}

function _webform_ext_reservation_sa_list_section(&$form, $form_state) 
{
   $form['fs_rooms_sa'] = array(
     '#type' => 'fieldset', 
	 '#title' => t("Step 3B - Rooms with Shared Baths:"),
	 '#description' => t(WEBFORM_EXT_CHECKED_ROOMS_MSG),
	 '#prefix' => '<div id="rooms-div-sa">',
     '#suffix' => '</div>',     
   ); 
   
   $num_rooms_sa = !empty($form_state['values']['select_num_rooms_sa']) ?
      $form_state['values']['select_num_rooms_sa'] : 0;	 
   
   $num_available = $_SESSION['num_rooms_available_sa'];
   if ($num_available == 0){
   	  $num_rooms_sa = 0;    
   }
   // if we have 0 rooms with private bath remianing, check if there are any with shared bath
   if ($_SESSION['num_rooms_available_pa'] == 0){ 
      if ($num_available > 0 && $num_rooms_sa == 0){
         $num_rooms_sa = 1; // auto-select at least 1 room
      }  
   }
   
   $is_checked = 1;
   for ($i = 1; $i <= $num_rooms_sa; $i++) {   
      _webform_ext_reservation_build_room_selector($form, 'sa', $i, $is_checked);
   }
}

function _webform_ext_reservation_pa_list_section(&$form, $form_state) 
{
   $form['fs_rooms_pa'] = array(
     '#type' => 'fieldset', 
	 '#title' => t("Step 3A - Rooms with Private Baths:"),
	 '#description' => t(WEBFORM_EXT_CHECKED_ROOMS_MSG),
	 '#prefix' => '<div id="rooms-div-pa">',
     '#suffix' => '</div>',     
   ); 
   
   // Initialize $num_rooms_pa to value stored in respective form_state values element 
   $num_rooms_pa = !empty($form_state['values']['select_num_rooms_pa']) ?
      $form_state['values']['select_num_rooms_pa'] : 0;
     
      
   // However it is required to reset $num_rooms_pa to 0 or 1 depending on the value stored in $_SESSION['num_rooms_available_pa'] 
   $num_available = $_SESSION['num_rooms_available_pa'];      
   if ($num_available == 0){
      $num_rooms_pa = 0;
   }else if ($num_available > 0 && $num_rooms_pa == 0){ 	  
 	  $num_rooms_pa = 1; // auto-select at least 1 room
   }
   
   $is_checked = 1;
   for ($i = 1; $i <= $num_rooms_pa; $i++) {
      _webform_ext_reservation_build_room_selector($form, 'pa', $i, $is_checked);
   }
}


function _webform_ext_reservation_pa_selector(&$form, $form_state)
{
   $room_options_pa = array();   
   $init_val = ($_SESSION['num_rooms_available_pa']) ? 1 : 0;
   for ($ctr = $init_val; $ctr <= $_SESSION['num_rooms_available_pa']; $ctr++){	  
	   $room_options_pa[$ctr] = $ctr;
   }
   
   $default_pa = !empty($form_state['values']['select_num_rooms_pa']) ?
      $form_state['values']['select_num_rooms_pa'] : 0;
   
   $form['fs_rooms']['select_num_rooms_pa'] = array(
	 '#title' => t('Number of rooms to reserve:'),
     '#type' => 'select',
	 '#options' => $room_options_pa,
     '#default_value' => $default_pa,
	 '#prefix' => '<div id="rooms-select-pa">',
     '#suffix' => '</div>',
     '#ajax' => array(
         'callback' => 'ajax_rooms_checkboxes_callback_pa',
         'wrapper' => 'rooms-div-pa',		
         'effect' => 'slide',
         'progress' => array('type' => 'throbber', 'speed' => 'slow'),
     ),
   ); 
}


function _webform_ext_reservation_sa_selector(&$form, $form_state)
{
   $room_options_sa = array();
   $init_val = ($_SESSION['num_rooms_available_pa']) ? 0 : 1;
   if ($_SESSION['num_rooms_available_sa'] == 0){
   	  $init_val = 0;
   }
   for ($ctr = $init_val; $ctr <= $_SESSION['num_rooms_available_sa']; $ctr++){
	   $room_options_sa[$ctr] = $ctr;
   }
   
   $default_sa = !empty($form_state['values']['select_num_rooms_sa']) ?
      $form_state['values']['select_num_rooms_sa'] : 0;
   error_log("Initial value of \$default_sa: {$default_sa}");   
      
   if ($_SESSION['num_rooms_available_pa'] == 0 && $_SESSION['num_rooms_available_sa'] > 0){
      if (empty($_SESSION['fully_booked_pa'])){
   	  	 $default_sa = 1;
   	  	 $_SESSION['fully_booked_pa'] = TRUE;   	  	 
   	  }
   	  error_log("Value of \$default_sa inside if block: {$default_sa}");   
   }else{
      if (!empty($_SESSION['fully_booked_pa'])){
         $_SESSION['fully_booked_pa'] = FALSE;
         $default_sa = 0;
      }
      error_log("Value of \$default_sa inside else block: {$default_sa}");   
   } 
   
   $form['fs_rooms']['select_num_rooms_sa'] = array(
	 '#title' => t('Number of rooms to reserve:'),
     '#type' => 'select',
	 '#options' => $room_options_sa,
     '#default_value' => $default_sa,
	 '#prefix' => '<div id="rooms-select-sa">',
     '#suffix' => '</div>',
     '#ajax' => array(
         'callback' => 'ajax_rooms_checkboxes_callback_sa',
         'wrapper' => 'rooms-div-sa',		
         'effect' => 'slide',
         'progress' => array('type' => 'throbber', 'speed' => 'slow'),
     ),
   );
   error_log("Final value of \$default_sa: {$default_sa}");	
}


function _webform_ext_reservation_build_room_selector(&$form, $suffix, $index, $is_checked, $reset = FALSE)
{
   if ($reset and !empty( $form["fs_rooms_{$suffix}"])){
     $form["fs_rooms_{$suffix}"] = array();
     return 0; 
   }
   $form["fs_rooms_{$suffix}"]["room_num_{$suffix}_{$index}"] = array(	  
	  '#type' => 'hidden', 
	  '#default_value' => $_SESSION["room_ids_{$suffix}"][$index-1]['rid'], 	  
   );
   $form["fs_rooms_{$suffix}"]["checkbox_room_{$suffix}_{$index}"] = array(
      '#type' => 'checkbox',
	  '#title' => "Room {$index}",
	  '#default_value' => (int)$is_checked,
   );
   $form["fs_rooms_{$suffix}"]["num_adults_{$suffix}_{$index}"] = array(
      '#type' => 'select',
      '#title' => "Adults/Guests (9 years or older):",
	  '#options' => array(1 => 1, 2, 3, 4,),
   );
   $form["fs_rooms_{$suffix}"]["num_children_{$suffix}_{$index}"] = array(
      '#type' => 'select',
      '#title' => "Guests (8 years or younger):",
	  '#options' => array(0, 1, 2, 3, 4,),
   );
      
   return count($form["fs_rooms_{$suffix}"]); 	
}

function _get_check_blocked_dates_link($departure_date)
{
   $dest_uri = base_path() . 'calendar-blocked-date/month/'. date('Y-m', $departure_date);
   $month_name = date('F', $departure_date);
   $msg = "<a href=\"{$dest_uri}\" title=\"". t("Check availability calendar"). "\">". t("See blocked dates for: {$month_name}"). "</a>";
   return $msg;
}

/*
*  Ajax call back
*/
function ajax_rooms_checkboxes_callback_pa($form, $form_state) {
   return $form['fs_rooms_pa'];
}

function ajax_rooms_checkboxes_callback_sa($form, $form_state) {
   return $form['fs_rooms_sa'];
}

 
function ajax_load_available_rooms_callback($form, $form_state)
{
   
   $commands = array();
     
   $selector = "#box-msg";   
   $element = $form['fs_rooms']['box_msg'];
   $element['#markup'] = _get_availability_msg();
   $commands[] = ajax_command_replace($selector, render($element));
   
   $selector = "#rooms-select-pa";
   $element = $form['fs_rooms']['select_num_rooms_pa'];
   $commands[] = ajax_command_replace($selector, render($element));
   
   $selector = "#rooms-select-sa";
   $element = $form['fs_rooms']['select_num_rooms_sa'];
   $commands[] = ajax_command_replace($selector, render($element));   
   
   $selector = "#rooms-div-pa";
   $element = $form['fs_rooms_pa'];
   $commands[] = ajax_command_replace($selector, render($element));
   
   $selector = "#rooms-div-sa";
   $element = $form['fs_rooms_sa'];
   $commands[] = ajax_command_replace($selector, render($element));
   
   $selector = "#check-blocked-dates";
   $element = $form['fs_availability_info']['check_blocked_dates'];
   $commands[] = ajax_command_replace($selector, render($element));
   
   $errors = array();
   $arrival_date = $form_state['values']['arrival_date'];
   $departure_date = $form_state['values']['departure_date'];
   _webform_ext_check_date_errors($arrival_date, $departure_date, $errors);
   _webform_ext_load_form_errors($errors);
   
   if (count($errors)){
	  $error_msgs = form_get_errors();
      $err_count = count($error_msgs);
      error_log(__FUNCTION__ ." says: \$err_count = $err_count");
      drupal_add_js(array('we_errors_list' => array('error_msgs' => $error_msgs)), 'setting');
      drupal_get_messages();
   }else{
      $error_msgs = array('no_errors' => '');
//       drupal_add_js(array('we_errors_list' => array('error_msgs' => $error_msgs)), 'setting');
   }   
   
   $form['#type'] = 'ajax';
   $form['#commands'] = $commands;
   
   return $form;
   
}

function _get_availability_msg()
{
   $msg = '<div id="count-pa"><span class="small-caption">With Private Bath</span><br /><span>'. $_SESSION['num_rooms_available_pa']. '</span><br /><span class="small-caption">Available</span></div>';
   
   $msg .= '<div id="count-sa"><span class="small-caption">With Shared Bath</span><br /><span>'. $_SESSION['num_rooms_available_sa']. '</span><br /><span class="small-caption">Available</span></div>';
   
   return $msg;
}

function _webform_ext_check_date_errors($arrival_date, $departure_date, &$errors) 
{

   $error_count = 0;	
   $today = mktime(0, 0, 0);
   $d = new DateTime(date('Y-m-d', $today), new DateTimeZone(WEBFORM_EXT_DEFAULT_TIMEZONE));
   $today = strtotime($d->format('Y-m-d')); 	  
	
   $good_dates_01 = strtotime($arrival_date);
   if ($good_dates_01 === FALSE){
   	  $error_count++;	
      $errors['date_error_01'] = t("'Arrival Date' does not appear to be in the correct format.");
   }
   
   $good_dates_05 = strtotime($departure_date);
   if ($good_dates_05 === FALSE){
      $error_count++;	
      $errors['date_error_05'] = t("'Departure Date' does not appear to be in the correct format.");
   }
   
   if ($error_count == 0){
	  $good_dates_10 = (strtotime($arrival_date) < strtotime($departure_date));  
   	  if (!$good_dates_10){
   	     $error_count++;
	     $errors['date_error_10'] = t("Planned 'Arrival Date' must be earlier than 'Departure Date'");
  	  }
  	  $good_dates_15 = (strtotime($arrival_date) >= strtotime(WEBFORM_EXT_MIN_DATE_FOR_RESERVATION, $today));
   	  if (!$good_dates_15){
   	     $error_count++;
      	 $errors['date_error_15'] = t("Planned 'Arrival Date' should not be earlier than: ". date('Y-F-d', strtotime(WEBFORM_EXT_MIN_DATE_FOR_RESERVATION)));
   	  }
   }
   
   if ($error_count == 0){	  
   	  $good_dates_20 = (strtotime($departure_date) - strtotime($arrival_date) <= 365 * DAY_IN_SECS);
   	  if (!$good_dates_20){
   	     $error_count++;
         $errors['date_error_20'] = t("Planned 'Departure Date' should not be more than 1 year from 'Arrival Date'.");
      }
   }
   
   if ($error_count == 0){		  
   	  $good_dates_25 = (strtotime($departure_date) <= strtotime('+1 year', $today) && strtotime($departure_date) <= strtotime('+1 year', $today));	  
      if (!$good_dates_25){
         $error_count++;
         $errors['date_error_25'] = t("Planned 'Arrival' and 'Departure' dates must be within 1 year of today.");
      }
   }   
   
   return $error_count;
}

function _webform_ext_reservation_validate_reservation($form, $form_state)
{   
   try{     
	  
	  $errors = array();
	  $arrival_date = $form_state['values']['arrival_date'];
      $departure_date = $form_state['values']['departure_date'];
      
      _webform_ext_check_date_errors($arrival_date, $departure_date, $errors);

	  $is_reserved = (strtolower($form_state['triggering_element']['#value']) == strtolower(WEBFORM_EXT_FORM_SUBMIT_BTN_CAPTION));
	  
	  if (!$errors && $is_reserved){
		 _webform_ext_reservation_form_1_calculate($form, $form_state, &$errors);
		 $order_total_non_zero = ($_SESSION['order_total'] > 0);	
		 $order_total_within_upper_limit = ($_SESSION['order_total'] <= WEBFORM_EXT_PP_CHARGE_LIMIT);
		 $num_guests_non_zero = ($_SESSION['num_guests'] > 0);
		 if (!$order_total_non_zero || !$num_guests_non_zero){
			$errors['zero_guests_or_order_total'] = t('Please select at least 1 room and a time span of at least 1 night.');
		 }
		 if (!$order_total_within_upper_limit){
		    $errors['order_total_over_limit'] = t('Order total must not exceed $!amount (!currency). Your order total is: $!order_total (!currency).', array('!currency' => WEBFORM_EXT_PP_TRANSACTION_CURRENCY, '!amount' => number_format(WEBFORM_EXT_PP_CHARGE_LIMIT, 2), '!order_total' => number_format($_SESSION['order_total'], 2),));
		 }
	  }	 
	    
   }catch(Exception $ex){
      watchdog(WEBFORM_EXT_MODULE_NAME,  t($ex->getMessage()), array(), WATCHDOG_ERROR);	  
   }       
   
   _webform_ext_load_form_errors($errors);
	
   $error_msgs = form_get_errors();
   drupal_add_js(array('we_errors_list' => array('error_msgs' => $error_msgs)), 'setting');
}

function _webform_ext_load_form_errors($errors)
{
   form_clear_error();
   if (count($errors)){
      $err_count = count($errors);
      error_log("\$err_count = $err_count");
      foreach($errors as $key => $value){
	     form_set_error($key, $value);
	     error_log("\$key = $key | \$value = $value");
	  }
   }
}

function _webform_ext_reservation_form_1_submit($form, &$form_state) {
   $_SESSION['billing_info'] = _webform_ext_reservation_billing_info($form, &$form_state);   
   $msg = WEBFORM_EXT_RES_SUCCESS_MSG_1;
   $protocol = IS_SSL ? HTTPS : HTTP; 
   drupal_goto($protocol. WEBFORM_EXT_HOST. '/'. WEBFORM_EXT_PAYMENT_PATH_ALIAS);
}

function _webform_ext_reservation_billing_info($form, $form_state)
{
   $protocol = (IS_SSL ? HTTPS : HTTP);
   
   $serverName = $_SERVER['SERVER_NAME'];
   $serverPort = $_SERVER['SERVER_PORT'];
   $url = $protocol. WEBFORM_EXT_HOST;
   
   $billing = new StdClass();   
   $billing->ItemName = format_string(WEBFORM_EXT_PP_ITEM_NAME, array('@num_guests' => $_SESSION['num_guests'], '@num_rooms' => count($_SESSION['reservation_room_picks']),'@from' =>  $form_state['values']['arrival_date'], '@to' => $form_state['values']['departure_date'], '@amount' => number_format($_SESSION['order_total'], 2),));
   
   $billing->ReturnURL = $url .'/'. WEBFORM_EXT_NOTIFICATION_SUCCESS_PAGE;
   $billing->NotifyURL = $protocol .$serverName.':'.$serverPort . WEBFORM_EXT_PP_NOTIFY_URL_FRAGMENT;;
   $billing->CancelUrl = $url .'/'. WEBFORM_EXT_PAYMENT_PATH_ALIAS;
   $billing->RegisteredBusinessEmail = (WEBFORM_EXT_TEST_PP_USE_SANDBOX ? WEBFORM_EXT_PP_BUSINESS_TEST : WEBFORM_EXT_PP_BUSINESS);
   $billing->OrderTotal = $_SESSION['order_total'];
   $billing->CurrencyCode = WEBFORM_EXT_PP_TRANSACTION_CURRENCY;
   
   return $billing;
}

function _webform_ext_reservation_form_1_calculate($form, $form_state, &$errors){  
   // bof: initialize session variables  
   $_SESSION['reservation_room_picks'] = array();
   $_SESSION['order_total'] = 0;
   $_SESSION['num_guests'] = 0;
   $_SESSION['rates'] = _get_current_rates();
   // eof: initialize session variables
   $room_options = array('Private Bath' => 'pa', 'Shared Bath' => 'sa');   
   $order_total = 0;
   $num_guests = 0;
   $arrival_date = $form_state['values']['arrival_date'];
   $departure_date = $form_state['values']['departure_date'];
   _query_num_rooms_available($arrival_date, $departure_date);
   foreach($room_options as $key => $suffix){    
   	  $_SESSION[$key] = 0;  
      if (!empty($_SESSION["num_rooms_available_{$suffix}"])){
         $rooms = ($suffix == 'pa') ? $_SESSION['room_ids_pa'] : $_SESSION['room_ids_sa'];
         $room_ctr = 0;
	  	 for ($ctr = 1; $ctr <= $_SESSION["num_rooms_available_{$suffix}"]; $ctr++){	  
	      	 $is_checked = !empty($form_state['values']["checkbox_room_{$suffix}_{$ctr}"]); 
	         $is_checked = ($is_checked && $form_state['values']["checkbox_room_{$suffix}_{$ctr}"]);
	      	 if ($is_checked){	         
	   	   	    $res = _webform_ext_reservation_form_1_init_res($suffix, $form_state, $ctr);
	            $_SESSION['reservation_room_picks'][] = $res;	      
	            $res->LineTotal = _calculate_sub_total($res->ArrivalDate, $res->DepartureDate, $res->NumGuestsPaying, $res->DailyRate, $res->DailyRateExtra);
	   	        $order_total += $res->LineTotal;
	   	        $num_guests += $res->NumGuests;
     		    if ($room_ctr < count($rooms)){
     		       $res->RoomNumber = $rooms[$room_ctr]['rid']; 
     		       $res->RoomType = $key;
			       error_log("Room Number: {$res->RoomNumber} Room Type: {$res->RoomType}");
	   	           if ($res->TooManyGuests){
	                  $errors["room{$ctr}"] = t("Room $ctr with [$key] has exceeded the maximum number of 4 allowed guests");
	               }
	         	}else{
	               if (empty($errors["room_{$suffix}"])){
	               	    $errors["room_{$suffix}"] = t("Not enough rooms with  '@amenities' remaining. Please review and re-enter your reservation.", array('@amenities' => $key));
	               }
	         	}
	         	$room_ctr++;	         	
	      	 }	  
      	 }  
      	 $_SESSION[$key] = $room_ctr;      	 
      }          
   }
	
   $order_total = (WEBFORM_EXT_TEST_ORDER_AMT_1_00 ? 1 : $order_total);	
   $order_total = (WEBFORM_EXT_TEST_ORDER_AMT_EXCESS ? WEBFORM_EXT_PP_CHARGE_LIMIT + 0.01 : $order_total);	
   
   $_SESSION['order_total'] = $order_total;
   $_SESSION['num_guests'] = $num_guests;

   return $order_total;
}

function _webform_ext_reservation_form_1_init_res($suffix, $form_state, $ctr){
   $res = new StdClass();  
   $res->ArrivalDate = $form_state['values']['arrival_date'];
   $res->DepartureDate = $form_state['values']['departure_date'];
   $res->NumGuestsPaying = (int)$form_state['values']["num_adults_{$suffix}_{$ctr}"];
   $res->NumGuests = (int)$form_state['values']["num_adults_{$suffix}_{$ctr}"] + (int)$form_state['values']["num_children_{$suffix}_{$ctr}"];
   $res->DailyRate = $_SESSION['rates']['rate'];
   $res->DailyRateExtra = $_SESSION['rates']['rate_extra'];
   $res->PrivateAmenities = 0; 
   $res->LineTotal = 0;
   $res->TooManyGuests = ($res->NumGuests > 4); 
   $res->RoomType = '';
   $res->RoomNumber = 0;
   return $res;
}