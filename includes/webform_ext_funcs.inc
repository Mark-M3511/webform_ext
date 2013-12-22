<?php

function _count_of_available_rooms()
{
   $retVal = 0;
   try{
      $query = db_select('zen_vupoint_rooms');
      $query->addExpression('COUNT(*)');
      $query->condition('available', 1 ,'=');
      $result = $query->execute();
      $retVal = $result->fetchField();
   }catch(Exception $e){
      watchdog(WEBFORM_EXT_MODULE_NAME,  t($e->getMessage()), array(), WATCHDOG_ERROR);
   }
   return $retVal;
}

function _count_of_available_rooms_by_type($room_type)
{
   $retVal = 0;
   try{
      $query = db_select('zen_vupoint_rooms');
      $query->addExpression('COUNT(*)');
      $query->condition('available', 1 ,'=');
	  /* $query->condition('code', $room_type ,'IN'); */
	  $query->condition('private_amenities', _webform_ext_room_has_amenities($room_type) ,'=');
      $result = $query->execute();
      $retVal = $result->fetchField();
   }catch(Exception $e){
      watchdog(WEBFORM_EXT_MODULE_NAME,  t($e->getMessage()), array(), WATCHDOG_ERROR);
   }
   return $retVal;
}

function _create_room_booking($rid, $next_available_date, $blocked_days)
{
   $retVal = NULL;
   try{
     _toggle_room_availability($rid, FALSE);
     $retVal = db_insert('zen_vupoint_room_bookings')
     ->fields(array(
   	  'rid' => $rid,
   	  'available_date' => $next_available_date,
   	  'blocked_days' => $blocked_days, 	
   	  ))->execute();
     _toggle_room_availability($rid, TRUE);
   }catch(Exception $e){
      $retVal = FALSE;
      watchdog(WEBFORM_EXT_MODULE_NAME,  t($e->getMessage()), array(), WATCHDOG_ERROR);
   }
   return $retVal;
}

function _create_room_booking_dates_detail($params, $arrival_date, $departure_date)
{
   $retVal = NULL;
   try{
	  $q = db_insert('zen_vupoint_booked_dates')
	     ->fields(array(
	   	  'rid',
	   	  'rbid',
	   	  'nid',
	   	  'booked_date', 	
		  'reservation_uid',
	   	));
	 $curr_date = $arrival_date;
	 while ($curr_date <= $departure_date){		  
		$params['booked_date'] = date('Y-m-d', $curr_date);
		$q->values($params);
	    $curr_date += DAY_IN_SECS;
	 }	    
	 $retVal = $q->execute();
   }catch(Exception $e){
      watchdog(WEBFORM_EXT_MODULE_NAME,  t($e->getMessage()), array(), WATCHDOG_ERROR);
   }
   return $retVal;
}

function _toggle_room_availability($rid, $available)
{
   try{ 
     db_update('zen_vupoint_rooms')
     ->fields(array(
     	'available' => ($available ? 1 : 0),
		'available_negation_code' => WEBFORM_EXT_NEG_CODE_REQUEST,
       )
     )
     ->condition('rid', $rid ,'=')
     ->execute();
   }catch(Exception $e){
      watchdog(WEBFORM_EXT_MODULE_NAME,  t($e->getMessage()), array(), WATCHDOG_ERROR);
   } 
}

function _set_reservation_status($success = TRUE)
{
	try{ 
       db_update('zen_vupoint_reservations')
       ->fields(array(
       	'request_status' => ($success ? WEBFORM_EXT_SUCCESS : WEBFORM_EXT_FAILURE),
         )
       )
       ->condition('nid', $_SESSION['reservation_node']->nid ,'=')
	   ->condition('sid', $_SESSION['reservation']->sid ,'=')
       ->execute();
   }catch(Exception $e){
      watchdog(WEBFORM_EXT_MODULE_NAME,  t($e->getMessage()), array(), WATCHDOG_ERROR);
   } 
}

function _is_room_available($rid)
{
  $retVal = FALSE;
  try{
    $rb = db_select('zen_vupoint_rooms', 'r')
	 ->fields('r', array('available'))
	 ->condition('r.rid', $rid ,'=')
	 ->orderBy('r.rid')	
	 ->range(0,1)
	 ->execute()
	 ->fetchAssoc();
	 $retVal = ((int)$rb['available'] ? TRUE : FALSE);
  }catch(Exception $e){
     watchdog(WEBFORM_EXT_MODULE_NAME,  t($e->getMessage()), array(), WATCHDOG_ERROR);
  }
  return $retVal;
}

function _toggle_room_availability_nid($nid, $available)
{
   try{ 
     db_update('zen_vupoint_rooms')
     ->fields(array(
     	  'available' => ($available ? 1 : 0),
       )
     )
     ->condition('nid', $nid ,'=')
     ->execute();
   }catch(Exception $e){
      watchdog(WEBFORM_EXT_MODULE_NAME,  t($e->getMessage()), array(), WATCHDOG_ERROR);
   } 
}

function _is_room_available_nid($nid)
{
   $retVal = FALSE;
   try{
     $rb = db_select('zen_vupoint_rooms', 'r')
	  ->fields('r', array('available'))
	  ->condition('r.nid', $nid ,'=')
	  ->orderBy('r.rid')	
	  ->range(0,1)
	  ->execute()
	  ->fetchAssoc();
	  $retVal = ((int)$rb['available'] ? TRUE : FALSE);
   }catch(Exception $e){
      watchdog(WEBFORM_EXT_MODULE_NAME,  t($e->getMessage()), array(), WATCHDOG_ERROR);
   }
   return $retVal;
}

function _calculate_sub_total($date_arrival, $date_departure, $num_guests, $daily_rate, $daily_rate_add)
{
   $retVal = 0.0;
   try
   {
   	  $num_days = strtotime($date_departure) - strtotime($date_arrival);
   	  $num_days /= DAY_IN_SECS;
   	  $num_days = round($num_days,0); // added 2013-09-24
       // calculate num of days
   	  if ($num_days < 1){
   	     throw new Exception('Length of stay is less than 24hrs.');	
   	  }
   	  $retVal = (double)$daily_rate * (double)$num_days;
   	  if ((int)$num_guests > MIN_OCCUPANCY_COUNT){
   	     $retVal += $daily_rate_add * ($num_guests - MIN_OCCUPANCY_COUNT) * $num_days;
   	  }
   }catch(Exception $e){
      watchdog(WEBFORM_EXT_MODULE_NAME,  t($e->getMessage()), array(), WATCHDOG_ERROR);
   }
   
   $retVal = round($retVal,2); // added 2013-09-24
   
   return $retVal;
}

function _create_booking_node($rid, $res)
{
  try{
    $new_node = new stdClass(); 
	$new_node->type = NODE_TYPE_BOOKING;
	node_object_prepare($new_node);
	$new_node->title = "Trx #: {$res->TransactionNumber} Dates: A{$res->ArrivalDate}D{$res->DepartureDate}";
	$new_node->language = LANGUAGE_NONE;
	/* bof: custom data */
	$new_node->body[$new_node->language][0]['value']  = NODE_TYPE_NEW_MSG. " Ref #: {$res->ReservationUid}";
    $new_node->body[$new_node->language][0]['summary'] = text_summary(NODE_TYPE_NEW_MSG);
    $new_node->body[$new_node->language][0]['format']  = 'filtered_html';
	$new_node->field_sid[$new_node->language][0]['value'] = 0;
	$new_node->field_nid[$new_node->language][0]['value'] = 0;
	$new_node->field_arrival_date[$new_node->language][0]['value'] = $res->ArrivalDate;
	$new_node->field_arrival_date[$new_node->language][0]['value2'] = $res->DepartureDate;
	$new_node->field_room_code[$new_node->language][0]['value'] = $rid;
	/* eof: custom data */
	node_save($new_node);
  }catch(Exception $e){
     watchdog(WEBFORM_EXT_MODULE_NAME,  t($e->getMessage()), array(), WATCHDOG_ERROR);
  }
}


function _create_availability_blocker_node($blocked_date, $room_type = '')
{  
  $start_time = strtotime($blocked_date);
  $m = date('m', $start_time);
  $d = date('d', $start_time);
  $y = date('Y', $start_time);
  $end_time = mktime(23, 59, 59, $m, $d, $y);
  /* Set Node Values */
  $room_type_desc = 'Fully booked - '. date('Y-m-d', $start_time);  
  $blocked_date_start = $blocked_date;
  $blocked_date_end = date('Y-m-d H:i:s', $end_time);
  try{
    $new_node = new stdClass(); 
	$new_node->type = NODE_TYPE_AVAILABILITY_BLOCKER;
	node_object_prepare($new_node);
	$new_node->title = $room_type_desc;
	$new_node->language = LANGUAGE_NONE;
	/* bof: custom data */
	$new_node->body[$new_node->language][0]['value']  = NODE_TYPE_NEW_MSG;
    $new_node->body[$new_node->language][0]['summary'] = text_summary(NODE_TYPE_NEW_MSG);
    $new_node->body[$new_node->language][0]['format']  = 'filtered_html';
	$new_node->field_blocked_date[$new_node->language][0]['value'] = $blocked_date_start;
	$new_node->field_blocked_date[$new_node->language][0]['value2'] = $blocked_date_end;
	/* eof: custom data */
	node_save($new_node);
  }catch(Exception $e){
     watchdog(WEBFORM_EXT_MODULE_NAME,  t($e->getMessage()), array(), WATCHDOG_ERROR);
  }
}

function _webform_ext_room_type_desc($room_type)
{
   $room_type_desc = 'All rooms';
   switch($room_type){
      case 'PB': 
	    $room_type_desc = 'Rooms with Private Bath';
	    break;
	  case 'SB': 
	    $room_type_desc = 'Rooms with Shared Bath';
	    break; 
   } 
   return $room_type_desc;   
}

function _search_availability_blocker_node_for_date($blocked_date)
{
  $retVal = FALSE;
  try{
    /* update node programmatically */
	$q = new EntityFieldQuery();
	$entity = $q->entityCondition('entity_type', 'node')
	->entityCondition('bundle', NODE_TYPE_AVAILABILITY_BLOCKER)
	->fieldCondition('field_blocked_date', 'value', $blocked_date, '=')
	->fieldCondition('field_blocked_date', 'value2', $blocked_date, '=')
    ->range(0,1)
    ->execute();
	$retVal = (!empty($entity['node'])); 
  }catch(Exception $e){
     watchdog(WEBFORM_EXT_MODULE_NAME,  t($e->getMessage()), array(), WATCHDOG_ERROR);
  }  
  return $retVal;
}

function _get_reservation_by_uid($reservation_uid)
{
   $retVal = NULL;
   try{
      $query = db_select('zen_vupoint_reservations', 'r')
      ->fields('r')
      ->condition('r.reservation_uid', $reservation_uid, '=');
      $retVal = $query->execute();
   }catch(Exception $e){
      watchdog(WEBFORM_EXT_MODULE_NAME,  t($e->getMessage()), array(), WATCHDOG_ERROR);
   }
   return $retVal;
}

function _get_num_guests_by_uid($reservation_uid)
{
   $retVal = NULL;
   try{
      $query = db_select('zen_vupoint_room_bookings', 'r')
	  ->fields('r', array('reservation_uid',));
      $query->addExpression('SUM(num_guests)', 'guest_count');
      $query->condition('r.reservation_uid', $reservation_uid, '=');
	  $query->groupBy('r.reservation_uid');
      $result = $query->execute();
	  $row = $result->fetchAssoc();
	  $retVal = $row['guest_count'];
   }catch(Exception $e){
      watchdog(WEBFORM_EXT_MODULE_NAME,  t($e->getMessage()), array(), WATCHDOG_ERROR);
   }
   return $retVal;
}

function _count_bookings_for_date($date)
{
   $retVal = 0;
   try{
      $query = db_select('zen_vupoint_booked_dates', 'bd')
				->fields('bd', array('booked_date'))
				->condition('bd.booked_date', array(date('Y-m-d', $date), date('Y-m-d', $date)), 'BETWEEN');
      $retVal = $query->countQuery()->execute()->fetchField();
   }catch(Exception $e){
      watchdog(WEBFORM_EXT_MODULE_NAME,  t($e->getMessage()), array(), WATCHDOG_ERROR);
   }
   return $retVal;
}

function get_dates_to_block($date_start, $date_end)
{  
   $retVal = 0;
   try{
   
      $num_available_rooms = _count_of_available_rooms(); 
      
      $subselect_blocked_dates = db_select('zen_vupoint_blocked_dates', 'bd')
	  ->fields('bd', array('blocked_date',));
	  $subselect_blocked_dates->condition('bd.booked_date', array("$date_start", "$date_end"), 'BETWEEN');
	  $subselect_blocked_dates->orderBy('bd.blocked_date');
	  
	  $select = db_select('zen_vupoint_booked_dates', 'bd')
	  ->fields('bd', array('booked_date',));	
      $select->AddExpression('COUNT(confirmed)', 'confirm_count');
      $select->condition('bd.booked_date', array("$date_start", "$date_end"), 'BETWEEN');
	  $select->condition('bd.booked_date', $subselect_blocked_dates, 'NOT IN');
	  // $select->condition('bd.confirmed', 1, '=');
      $select->groupBy('bd.booked_date');	
      $select->havingCondition('confirm_count', $num_available_rooms,'>=');	  
	  $retVal = $select->execute();
	  /* dpq($select); */
   }catch(Exception $e){
      watchdog(WEBFORM_EXT_MODULE_NAME,  t($e->getMessage()), array(), WATCHDOG_ERROR);
   }
   return $retVal;
}

function _get_room_code_by_rid($rid = 0)
{
  $retVal = '';
  try{
    $rb = db_select('zen_vupoint_rooms', 'r')
	 ->fields('r', array('code'))
	 ->condition('r.rid', $rid ,'>=')
	 ->orderBy('rid')	
	 ->range(0,1)
	 ->execute()
	 ->fetchAssoc();
	 $retVal = $rb['code'];
  }catch(Exception $e){
     watchdog(WEBFORM_EXT_MODULE_NAME,  t($e->getMessage()), array(), WATCHDOG_ERROR);
  }
  return $retVal;
}

function _search_availaility_blocker_node($booked_dates)
{
  $retVal = 0;
  try{
    /* update node programatically */
	$q = new EntityFieldQuery();
	$entity = $q->entityCondition('entity_type', 'node')
	->entityCondition('bundle', NODE_TYPE_AVAILABILITY_BLOCKER)
	->fieldCondition('field_blocked_date', 'value', $booked_dates)
    ->range(0,1)
    ->execute();
	if (!empty($entity['node'])) {
	   $node = node_load(array_shift(array_keys($entity['node'])));
	   /* node_object_prepare($node); */
	   $retVal = $node->nid;
	   /* eof: custom data */
    }
  }catch(Exception $e){
     watchdog(WEBFORM_EXT_MODULE_NAME,  t($e->getMessage()), array(), WATCHDOG_ERROR);
  }
  
  return $retVal;
}

function _webform_ext_finalize_booking_process_v2($reservations)
{
   try{
      $txn = db_transaction();
      _webform_ext_save_reservation_to_db($reservations);
	  _webform_ext_save_room_bookings_to_db($reservations);
	  
      unset($_SESSION['reservation'], $_SESSION['order_value'], $_SESSION['reservation_room_picks'],$_SESSION['num_guests']);
	  
   }catch(Exception $e){
      $txn->rollback();
      watchdog(WEBFORM_EXT_MODULE_NAME,  t($e->getMessage()), array(), WATCHDOG_ERROR);
   }
}

function _webform_ext_save_reservation_to_db($reservations)
{
   try {
	  $q = db_insert('zen_vupoint_reservations')
	  	  ->fields(array(
	  	  'nid', 
	  	  'sid', 
	  	  'create_date',
	  	  'number_of_guests',
	  	  'date_of_arrival',
	  	  'date_of_departure',
	  	  'salutation',
	  	  'first_name',
	  	  'last_name',
	  	  'email',
	  	  'contact_number_1',
	  	  'street_address_1',
	  	  'street_address_2',
	  	  'city',
	  	  'countries',
	  	  'prov_state_terr_f',
	  	  'postal_zip_code',
	  	  'sub_total',
	  	  'daily_rate',
	  	  'daily_rate_add',
		  'room_type',
	  	  'note',
		  'reservation_uid',
		  'request_status',
	    ));	 
        foreach($reservations as $key => $res){
          $q->values(array(
	  	      'nid' => $res->nid, 
	  	      'sid' => $res->sid, 
	  	      'create_date' => date('Y-m-d H:i:s'),
	  	      'number_of_guests' => $res->NumGuests,
	  	      'date_of_arrival' => $res->ArrivalDate,
	  	      'date_of_departure' => $res->DepartureDate,
	  	      'salutation' => $res->Salutation,
	  	      'first_name' => $res->FirstName,
	  	      'last_name' => $res->LastName,
	  	      'email' => $res->Email,
	  	      'contact_number_1' => $res->ContactNum,
	  	      'street_address_1' => $res->StreetAddress1,
	  	      'street_address_2' => $res->StreetAddress2,
	  	      'city' => $res->CityTown,
	  	      'countries' => $res->Country,
	  	      'prov_state_terr_f' => $res->ProvState,
	  	      'postal_zip_code' => $res->PostalZip,
			  'sub_total' => $res->TotalCharge,
	  	      'daily_rate' => $res->DailyRate,
	  	      'daily_rate_add' => $res->DailyRateExtra,
		      'room_type' => $res->RoomType,
	  	      'note' => $res->Note,
			  'reservation_uid' => $res->ReservationUid,
			  'request_status' => $res->RequestStatus,
		    )
		  );
        }
	    $q->execute();			  	  
    }catch(Exception $e){
       watchdog(WEBFORM_EXT_MODULE_NAME,  t($e->getMessage()), array(), WATCHDOG_ERROR);
	   throw $e;
    }
}

function _webform_ext_update_reservation_from_IPN($res)
{
   try{   
       db_update('zen_vupoint_reservations')
       ->fields(array(
		   'first_name' => $res->FirstName,
	  	   'last_name' => $res->LastName,
		   'email' => $res->Email,
		   'street_address_1' => $res->StreetAddress1,
		   'city' => $res->CityTown,
		   'countries' => $res->Country,
		   'prov_state_terr_f' => $res->ProvState,
	  	   'postal_zip_code' => $res->PostalZip,
		   'request_status' => $res->RequestStatus,
	       'ipn_track_id' => $res->IPNTrackingId,
         )
       )
       ->condition('reservation_uid', $res->ReservationUid,'=')
       ->execute();  
   }catch(Exception $ex){
      watchdog(WEBFORM_EXT_MODULE_NAME,  t($ex->getMessage()), array(), WATCHDOG_ERROR);
   }
}

function _webform_ext_admin_transaction($res)
{
	$res->TransactionDate = date('Y-m-d H:i:s',time());
    $res->TransactionNumber = $res->ReservationUid;
	$res->IPNTrackingId = $res->ReservationUid;
	$res->RequestStatus = WEBFORM_EXT_COMPLETED;
    $res->TransactionAmt = number_format((double)$_SESSION['order_total'], 2);
    $res->ContactNumber_1 = $res->ContactNumCountryCode; 
    $res->ContactNumber_1 .= $res->ContactNumAreaCode;
    $res->ContactNumber_1 .= $res->ContactNumLineNum; 
	_webform_ext_wrap_up_IPN_interaction($res);
}

function _webform_ext_wrap_up_IPN_interaction($res)
{   
   if ($res->RequestStatus == WEBFORM_EXT_COMPLETED){
	  _webform_ext_update_room_booking_dates($res->ReservationUid, TRUE);
	  // create a booking for each room
	  if (isset($res->Rooms) && count($res->Rooms)){
	     foreach($res->Rooms as $key => $room_number){
	       _create_booking_node($room_number, $res);
	     }
	  }else{
	     _create_booking_node(0, $res);
	  }
	  
	  // bof: send 2 emails > (1) to guest (2) to admin
	  $admin_email = (WEBFORM_EXT_USE_TEST_EMAIL ? WEBFORM_EXT_TEST_EMAIL : variable_get('site_mail', WEBFORM_EXT_PP_BUSINESS));
	  $recp = array($res->Email,  $admin_email);
	  _webform_ext_send_confirmation_mail($res, $recp);
	  // eof: send 2 emails > (1) to guest (2) to admin
	  _webform_ext_init_blocked_dates_queue();  	  
   }
   _webform_ext_update_reservation_from_IPN($res);   
}


function _webform_ext_init_blocked_dates_queue()
{
   $_date->starttime = date('Y-m-d',strtotime("-3 months"));
   $_date->endtime =  date('Y-m-d', strtotime("+1 year"));
   $queue = DrupalQueue::get(WEBFORM_EXT_QUEUE_CREATE_BLOCKING_NODES);
   $queue->createItem($_date);
}

function _webform_ext_send_confirmation_mail($res, $recp)
{
   $params = array(); 
   $params['from'] = variable_get('site_mail', WEBFORM_EXT_PP_BUSINESS);
   $params['subject'] = WEBFORM_EXT_NOTIFICATION_EMAIL_SUBJECT;
   $params['body_language'] = language_default();
   $params['message_body'] = '';
   $params['invoice_data'] = $res;
   if (WEBFORM_EXT_QUEUE_MAIL_SEND){
      $queue = DrupalQueue::get(WEBFORM_EXT_QUEUE_CONFIRMATION_EMAILS);
      $_params->params = $params;
	  $_params->recp = $recp;
	  $queue->createItem($_params);
   }else{
      foreach($recp as $email){
        $params['to'] = $email;
        $mail_result = _webform_ext_send_mail($params);
        if (!$mail_result['result']){
	       watchdog(WEBFORM_EXT_MODULE_NAME, WEBFORM_EXT_NOTIFICATION_EMAIL_ERROR, array(), WATCHDOG_ERROR);
        }  
      }
   }
}

function _webform_ext_send_mail($params)
{
    $mail_result = NULL;
	try{ 
       $mail_result = drupal_mail(WEBFORM_EXT_MODULE_NAME, WEBFORM_EXT_EMAIL_KEY, $params['to'], 
          $params['body_language'], $params, $params['from']);
	}catch(Exception $e){
       watchdog(WEBFORM_EXT_MODULE_NAME,  t($e->getMessage()), array(), WATCHDOG_ERROR);
    }
	
	return $mail_result;
}

function _webform_ext_queue_send_mail($params)
{
    error_log('Inside function: '. __FUNCTION__ );
    $mail_result = NULL;
	try{ 
	  foreach($params->recp as $to){
	    $params->params['to'] = $to;
	    error_log('Processing email for: '.$params->params['to']);
        $mail_result = drupal_mail(WEBFORM_EXT_MODULE_NAME, WEBFORM_EXT_EMAIL_KEY, $to, 
          $params->params['body_language'], $params->params, $params->params['from']);
	  }
	}catch(Exception $e){
       watchdog(WEBFORM_EXT_MODULE_NAME,  t($e->getMessage()), array(), WATCHDOG_ERROR);
    }
	
	return $mail_result;
}

function _webform_ext_processed_blocked_dates($data)
{
   // error_log('Inside function: '. __FUNCTION__ );
//    error_log('Start Date: '. $data->starttime );
//    error_log('End Date: '. $data->endtime );
   try{
     $blocked_dates = get_dates_to_block($data->starttime, $data->endtime);
     while($rows = $blocked_dates->fetchAssoc()){
	    $booked_date = $rows['booked_date']; 
	    _create_availability_blocker_node($booked_date);
		error_log("Blocked Date: $booked_date");
	 }
   }catch(Exception $ex){
      watchdog(WEBFORM_EXT_MODULE_NAME,  t($e->getMessage()), array(), WATCHDOG_ERROR);
   }
}  

function _webform_ext_save_room_bookings_to_db($reservations)
{
   try{
     foreach($reservations as $key => $res){
        $q = db_insert('zen_vupoint_room_bookings')
          ->fields(array(
   	        'rid',
   	        'available_date',
   	        'blocked_days',
	    	'line_total',
	    	'num_guests',
	    	'date_of_arrival',
	    	'date_of_departure',
	    	'reservation_uid',
   	     ));
	     foreach($res->rooms as $key => $room){
	        $q->values(array(
	     	      'rid' => 0, 
	     	      'available_date' => $res->DepartureDate,
	    		  'date_of_arrival' => $res->ArrivalDate,
	    		  'date_of_departure' => $res->DepartureDate,
	    		  'line_total' => $room->LineTotal,
	    		  'num_guests' => $room->NumGuests,
				  'blocked_days' => $res->NumNights,
	    		  'reservation_uid' => $res->ReservationUid,
	    		)  
	    	 );
         }
	     $q->execute();
	 }
   }catch(Exception $e){
      watchdog(WEBFORM_EXT_MODULE_NAME,  t($e->getMessage()), array(), WATCHDOG_ERROR);
	  throw $e;
   }
}

function _insert_new_room($node)
{    
   try{
      $lang_room_code_rf = field_language('node', $node, 'field_room_code_rf');
      $lang_private_amenities = field_language('node', $node, 'field_private_amenities');
      db_insert('zen_vupoint_rooms')
	  ->fields(array(
   	     'nid' => $node->nid,
   	     'code' => $node->field_room_code_rf[$lang_room_code_rf][0]['value'],
   	     'name' => $node->title,
   	     'available' => ($node->status ? 1 : 0),
   	     'private_amenities' => ($node->field_private_amenities[$lang_private_amenities][0]['value'] ? 1 : 0),
		 'available_negation_code' => (!$node->status ? WEBFORM_EXT_NEG_CODE_ADMIN : NULL),
		 'available_negation_date' => (!$node->status ? date('Y-m-d H:i:s') : NULL),
      ))->execute();
    }catch(Exception $e){
        watchdog(WEBFORM_EXT_MODULE_NAME,  t($e->getMessage()), array(), WATCHDOG_ERROR);
    }
}

function _update_room($node)
{
   try{ 
	 $lang_room_code_rf = field_language('node', $node, 'field_room_code_rf');
	 $lang_private_amenities = field_language('node', $node, 'field_private_amenities');
	 db_update('zen_vupoint_rooms')
	 ->fields(array(
	 	'code' => $node->field_room_code_rf[$lang_room_code_rf][0]['value'],
	 	'name' => $node->title,
	 	'available' => ($node->status ? 1 : 0),
	 	'private_amenities' => ($node->field_private_amenities[$lang_private_amenities][0]['value'] ? 1 : 0),
		'available_negation_code' => (!$node->status ? WEBFORM_EXT_NEG_CODE_ADMIN : NULL),
		'available_negation_date' => (!$node->status ? date('Y-m-d H:i:s') : NULL),
	   )
	 )
	 ->condition('nid', $node->nid)
	 ->execute();
  }catch(Exception $e){
     watchdog(WEBFORM_EXT_MODULE_NAME,  t($e->getMessage()), array(), WATCHDOG_ERROR);
  } 	
}

function _insert_blocked_date($node)
{    
	try{
	  node_object_prepare($node); 
	  $lang_field_blocked_date = field_language('node', $node, 'field_blocked_date'); 
	  db_insert('zen_vupoint_blocked_dates')
	  ->fields(array(
		'nid' => $node->nid,
		'create_date' => date('Y-m-d'),
		'change_date' => date('Y-m-d H:i:s'),
		'blocked_date' => $node->field_blocked_date[$lang_field_blocked_date][0]['value'],
		'blocked' => 1,
	  ))->execute();
    }catch(Exception $e){
       watchdog(WEBFORM_EXT_MODULE_NAME,  t($e->getMessage()), array(), WATCHDOG_ERROR);
    }
}

function _update_blocked_date($node)
{
   try{ 
     node_object_prepare($node); 
	 $lang_field_blocked_date = field_language('node', $node, 'field_blocked_date'); 
	 db_update('zen_vupoint_blocked_dates')
	 ->fields(array(
	    'change_date' => date('Y-m-d H:i:s'), 
		'blocked_date' => $node->field_blocked_date[$lang_field_blocked_date][0]['value'],
	 	'blocked' => ($node->status ? 1 : 0),
	   )
	 )
	 ->condition('nid', $node->nid)
	 ->execute();
  }catch(Exception $e){
     watchdog(WEBFORM_EXT_MODULE_NAME,  t($e->getMessage()), array(), WATCHDOG_ERROR);
  } 	
}

function _delete_blocked_date($node)
{
   try{ 
      node_object_prepare($node); 	  
	  db_delete('zen_vupoint_blocked_dates')
	  ->condition('nid', $node->nid)
	  ->execute();
   }catch(Exception $e){
      watchdog(WEBFORM_EXT_MODULE_NAME,  t($e->getMessage()), array(), WATCHDOG_ERROR);
   } 	
}

function _retrieve_mail_message($title, $notification_type = 'NORMAL')
{
  $node = NULL;
  try{
    // update node programatically
	$q = new EntityFieldQuery();
	$entity = $q->entityCondition('entity_type', 'node')
	->entityCondition('bundle', NODE_TYPE_EMAIL_MSG)
	->propertyCondition('title', $title)
	->fieldCondition('field_notification_type', 'value', $notification_type, '=')
    ->range(0,1)
    ->execute();
	if (!empty($entity['node'])) {
	   $node = node_load(array_shift(array_keys($entity['node'])));
	   node_object_prepare($node);	   	   
    }
  }catch(Exception $e){
     watchdog(WEBFORM_EXT_MODULE_NAME,  t($e->getMessage()), array(), WATCHDOG_ERROR);
  }
  
  return $node;
}

function _get_current_rates()
{
  $retVal = array('rate' => VPGH_DAILY_RATE, 'rate_extra' => VPGH_DAILY_RATE_EXTRA,);
  try{
	$q = new EntityFieldQuery();
	$entity = $q->entityCondition('entity_type', 'node')
	->entityCondition('bundle', NODE_TYPE_RATES)
    ->range(0,1)
    ->execute();
	if (!empty($entity['node'])) {
	   $node = node_load(array_shift(array_keys($entity['node'])));
	   node_object_prepare($node);
	   $field_lang = field_language('node', $node, 'field_regular_rate'); 
       $retVal['rate'] = $node->field_regular_rate[$field_lang][0]['value'];
	   $field_lang = field_language('node', $node, 'field_rate_additional');
	   $retVal['rate_extra'] = $node->field_rate_additional[$field_lang][0]['value'];
    }
  }catch(Exception $e){
     watchdog(WEBFORM_EXT_MODULE_NAME,  t($e->getMessage()), array(), WATCHDOG_ERROR);
  }  
  return $retVal;
}

function _webform_ext_destination_url($target)
{
   $protocol = HTTPS;
   $retVal = $target;
   if (IS_SSL){
      $retVal = $protocol. WEBFORM_EXT_HOST. '/'. $target; 
   }   
   return $retVal;
}

function _webform_ext_room_has_amenities($room_type)
{
   return (strtoupper($room_type) == 'PB' ? 1 : 0);
}


function new_reservation_object_default(){
   $res = new StdClass();
   $res->nid = 0;
   $res->sid = 0;
   $res->FirstName = '';
   $res->LastName = '';
   $res->Salutation = '';
   $res->StreetAddress1 = '';
   $res->StreetAddress2 = '';
   $res->CityTown = '';
   $res->ProvState = '';
   $res->PostalZip = '';
   $res->Country = WEBFORM_EXT_COUNTRY_DEFAULT;
   $res->Email = '';  
   $res->ContactNumCountryCode = WEBFORM_EXT_COUNTRY_CODE_DEFAULT;
   $res->ContactNumAreaCode = '';
   $res->ContactNumPrefixCode = '';
   $res->ContactNumLineNum = '';
   $res->ContactNum = '';
   $res->TransactionDate = date('Y-m-d H:i:s');
   $res->TransactionNumber = '';
   $res->NumGuests = 0;
   $res->DailyRate = 0;
   $res->DailyRateExtra = 0;      
   $res->ArrivalDate = date('Y-m-d');
   $res->DepartureDate = date('Y-m-d');
   $res->NumNights = 0;
   $res->TotalCharge = 0;
   $res->CardType = '';
   $res->CCLast4 = '';
   $res->RoomType = '';
   $res->RequestStatus = 'UNKNOWN';
   $res->Note ='';   
   $res->ReservationUid = strtoupper(uniqid('VPGH', FALSE));
   /*------------------------------------------------------------*/
   $res->BillingInfo = NULL;
   $res->rooms = array();
   $res->creditcard = array();
   $res->creditcard[] = new_credit_card_object_default($res); 
   return $res;
}

function new_credit_card_object_default($res){
   $cc = new StdClass();
   $cc->ReservationUid = $res->ReservationUid;
   $cc->CreditCardNumber = '';
   $cc->CreditCardType = WEBFORM_EXT_SELECT_DEFAULT;
   $cc->ExpYear = date('Y');
   $cc->ExpMonth = '01';
   $cc->CVV2 = '';   
   
   return $cc;   
}  

function _webform_ext_check_availability_v2($date_start, $date_end, $private_amenities)
{
   error_log("_webform_ext_check_availability_v2: \$date_start = {$date_start} | \$date_end = {$date_end}");
   $retVal = NULL;
   try{      	  
	  $c = db_select('zen_vupoint_blocked_dates', 'bd');
	  $c->AddExpression('count(*)', 'bd_count');
	  $c->condition('bd.blocked_date', array("$date_start", "$date_end"), 'BETWEEN');
	  $result = $c->Execute();
	  $bd_count = $result->fetchField();
   
      $subselect_rooms = db_select('zen_vupoint_rooms', 'r2')
	  ->fields('r2', array('rid',));
	  $subselect_rooms->leftJoin('zen_vupoint_booked_dates', 'bd', 'r2.rid = bd.rid');
	  $subselect_rooms->condition('r2.private_amenities', $private_amenities, 'IN');
	  $subselect_rooms->condition('r2.available', 1, '=');
	  $subselect_rooms->condition('bd.booked_date', array("$date_start", "$date_end"), 'BETWEEN');
	  $subselect_rooms->condition('bd.confirmed', 1, '=');
	  $subselect_rooms->orderBy('r2.rid');	  
	  
	  $r = db_select('zen_vupoint_rooms', 'r')
	  ->fields('r', array('rid','private_amenities',));
	  $r->condition('r.private_amenities', $private_amenities, 'IN');
	  $r->condition('r.available', 1, '=');
	  $r->condition('r.rid', $subselect_rooms, 'NOT IN');
	  // Let's make sure that if we have any blocked dates in the range we return 0 rows
  	  $r->condition(0, $bd_count, '=');
	  $r->orderBy('r.private_amenities', 'DESC');
	  $r->orderBy('r.rid');
	  $retVal = $r->execute();
	  // $debug_msg = dpq($r);
 	  // watchdog(WEBFORM_EXT_MODULE_NAME,  $debug_msg, array(), WATCHDOG_INFO); 
   }catch(Exception $e){
      watchdog(WEBFORM_EXT_MODULE_NAME,  t($e->getMessage()), array(), WATCHDOG_ERROR);
   }
   return $retVal;
}

function _query_num_rooms_available($arrival_date, $departure_date)
{
   error_log("_query_num_rooms_available: \$arrival_date = {$arrival_date} | \$departure_date = {$departure_date}");
   
   $rows = _webform_ext_check_availability_v2($arrival_date, $departure_date, array(0,1,));
   
   $_SESSION['room_ids_pa'] = array();
   $_SESSION['room_ids_sa'] = array();
   $_SESSION['num_rooms_available_pa'] = 0;
   $_SESSION['num_rooms_available_sa'] = 0;
   $_SESSION['num_rooms_available'] = $rows->rowCount();
   error_log("\$_SESSION['num_rooms_available']: {$_SESSION['num_rooms_available']}");
   while($row = $rows->fetchAssoc()){
      if ($row['private_amenities']){
  	     $_SESSION['num_rooms_available_pa']++;
  	     $_SESSION['room_ids_pa'][] = array('list_order' => $_SESSION['num_rooms_available_pa'], 'rid' => $row['rid']);
      }else{
  	     $_SESSION['num_rooms_available_sa']++;
  	     $_SESSION['room_ids_sa'][] = array('list_order' => $_SESSION['num_rooms_available_sa'], 'rid' => $row['rid']);
      }     
   }
   error_log("\$_SESSION['num_rooms_available_pa']: {$_SESSION['num_rooms_available_pa']}");
}


function _webform_ext_delete_room_booking_dates($reservation_uid)
{
   try{      
	  $r = db_delete('zen_vupoint_booked_dates');
	  $r->condition('reservation_uid', $reservation_uid, '=');  
	  $retVal = $r->execute();
	  // dpq($r);
   }catch(Exception $e){
      watchdog(WEBFORM_EXT_MODULE_NAME,  t($e->getMessage()), array(), WATCHDOG_ERROR);
   }
}

function _webform_ext_update_room_booking_dates($reservation_uid, $confirmed)
{
   try{      
	  db_update('zen_vupoint_booked_dates')
     ->fields(array(
     	'confirmed' => ($confirmed ? 1 : 0),
       )
     )
     ->condition('reservation_uid', $reservation_uid ,'=')
     ->execute();
	 // dpq($r); 
   }catch(Exception $e){
      watchdog(WEBFORM_EXT_MODULE_NAME,  t($e->getMessage()), array(), WATCHDOG_ERROR);
   }
}

function _webform_ext_load_country_names()
{
   $retVal = NULL;
   $country_list = array();
   try{      
	  $r = db_select('zen_vupoint_country_list', 'c')
	  ->fields('c', array('iso2', 'country_name_friendly',));
	  $r->orderBy('c.country_name_friendly');
	  $retVal = $r->execute();
	 /*  dpq($r); */
	  while ($row = $retVal->fetchAssoc()){
	     $iso2 = $row['iso2'];
		 $country = $row['country_name_friendly'];
	     if ($row['iso2'] == 'CA' || $row['iso2'] == 'US'){
		    $iso2 = 'US-CAN';
			$country = 'Canada/United States';
		 }
	     $country_list[$iso2] = $country;
	  }
   }catch(Exception $e){
      watchdog(WEBFORM_EXT_MODULE_NAME,  t($e->getMessage()), array(), WATCHDOG_ERROR);
   }
   return $country_list;
}

function _webform_ext_load_country_codes()
{
   $retVal = NULL;
   $country_list = array();
   try{      
	  $r = db_select('zen_vupoint_country_list', 'c')
	  ->fields('c', array('country_code', 'country_name_friendly',));
	  $r->condition('c.country_code', 0, '>');
	  $r->orderBy('c.country_code');
	  $retVal = $r->execute();
	 // dpq($r); 
	  while ($row = $retVal->fetchAssoc()){
		 $country_list[$row['country_code']] = $row['country_code'];;
	  }
   }catch(Exception $e){
      watchdog(WEBFORM_EXT_MODULE_NAME,  t($e->getMessage()), array(), WATCHDOG_ERROR);
   }
   return $country_list;
}

function _webform_ext_load_states_prov_NA()
{
   $retVal = NULL;
   $states_prov = array();
   try{      
	  $r1 = db_select('zen_vupoint_us_states', 'us')
	  ->fields('us', array('abbreviation', 'state',));
	  $r1->AddExpression('1', 'prefix');
	  $r2 = db_select('zen_vupoint_canadian_provs', 'ca')
	  ->fields('ca', array('abbreviation', 'province',));
	  $r2->AddExpression('2', 'prefix');
	  $r1->union($r2, 'ALL');
	  $retVal = $r1->execute();
	 //  dpq($r);
	  while ($row = $retVal->fetchAssoc()){	     	     
		 $states_prov[$row['prefix'].'-'.$row['abbreviation']] = $row['state'];;
	  }
   }catch(Exception $e){
      watchdog(WEBFORM_EXT_MODULE_NAME,  t($e->getMessage()), array(), WATCHDOG_ERROR);
   }
   return $states_prov;
}

function  _webform_ext_get_reservation_values($form_state, &$res)
{
   try{ 
      $country_code = $form_state['values']['country'];
	  $prov_state = $form_state['values']['prov_state'];
	  if ($form_state['values']['country'] == WEBFORM_EXT_COUNTRY_DEFAULT){	    
		 if (substr($form_state['values']['prov_state_NA'], 0, 1) == '1'){
		     $country_code = 'US';			 
		 }else{
		     $country_code = 'CA'; 
		 }
		 $prov_state = substr($form_state['values']['prov_state_NA'], -2);
      }      
	  $res->FirstName = $form_state['values']['firstname'];
      $res->LastName = $form_state['values']['lastname'];
      $res->Salutation = $form_state['values']['salutation'];
      $res->StreetAddress1 = $form_state['values']['street_address_1'];
      $res->StreetAddress2 = $form_state['values']['street_address_2'];
      $res->CityTown = $form_state['values']['city_town'];
      $res->ProvState = $prov_state;
      $res->PostalZip = strtoupper($form_state['values']['postal_zip']);
      $res->Country = $country_code;
      $res->Email = $form_state['values']['email'];  
      $res->ContactNumCountryCode = $form_state['values']['country_code'];
      $res->ContactNumAreaCode = $form_state['values']['area_code'];
      $res->ContactNumPrefixCode = '';
      $res->ContactNumLineNum = $form_state['values']['line_code'];
      $res->ContactNum =  $res->ContactNumCountryCode. $res->ContactNumAreaCode. $res->ContactNumLineNum;
      $res->TransactionDate = date('Y-m-d H:i:s');
      $res->TransactionNumber = '';
      $res->NumGuests = (int)$_SESSION['num_guests'];
      $res->DailyRate = (float)$res->rooms[0]->DailyRate;
      $res->DailyRateExtra = (float)$res->rooms[0]->DailyRateExtra;      
      $res->ArrivalDate = $res->rooms[0]->ArrivalDate;
      $res->DepartureDate = $res->rooms[0]->DepartureDate;
      $res->NumNights = (strtotime($res->DepartureDate) - strtotime($res->ArrivalDate)) / DAY_IN_SECS;
      $res->TotalCharge = (WEBFORM_EXT_TEST_ORDER_AMT_1_00 ? 1 : $_SESSION['order_total']); 
      $res->RoomType = '';
      $res->RequestStatus = 'UNKNOWN';
      $res->Note = substr($form_state['values']['notes'], 0, 500);
      if (property_exists($res, 'creditcard') && !empty($form_state['values']['credit_card'])){
         $res->creditcard[0]->CreditCardType = $form_state['values']['credit_type'];
	     $res->creditcard[0]->CreditCardNumber = $form_state['values']['credit_card'];
         $res->CCLast4 = substr($res->creditcard[0]->CreditCardNumber,-4);
	     $res->CardType = $res->creditcard[0]->CreditCardType;
	     $res->creditcard[0]->ExpYear = $form_state['values']['cc_exp_year'];
	     $res->creditcard[0]->ExpMonth = $form_state['values']['cc_exp_month'];
	     $res->creditcard[0]->CVV2 = $form_state['values']['cc_cvv2'];
	  } 
	  /* dpm($_SESSION['reservation'][0]); */
   }catch(Exception $ex){
      watchdog(WEBFORM_EXT_MODULE_NAME,  t($e->getMessage()), array(), WATCHDOG_ERROR);
   }
   
}

function _webform_ext_configure_mandrill_merge_vars(&$mandrill_params, $message)
{
    
   $global_merge_vars = array();
   $res = $message['params']['invoice_data'];
   
   $global_merge_vars[] = array(
     'name' => 'subject',
	 'content' => $message['params']['subject'],
   );
   $global_merge_vars[] = array(
     'name' => 'from',
	 'content' => $message['params']['from'],
   );
   $global_merge_vars[] = array(
     'name' => 'current_year',
     'content' => date('Y'),
   );
   
   $mandrill_params['message']['global_merge_vars'] = $global_merge_vars;
   
   /* an array or arrays of per recipient values */
   $merge_vars = array();
   /* per recipient values */
   $vars = array();        
   /* We should use a simple object - set up recipient values */
   foreach($res as $name => $value){
      if (!is_array($value) && !is_object($value)){
         $vars[] = array(
	       'name' => $name,
           'content' => check_plain($value),
	     );
	  }
   }
   
   $merge_vars[] = array(
     'rcpt' => $message['params']['to'],
     'vars' => $vars,
   );  
   $mandrill_params['message']['merge_vars'] = $merge_vars;
}

function _webform_ext_user_is_content_admin()
{
	global $user;
	
	$account = $user;
	$is_registration_admin = in_array(WEBFORM_EXT_ROLE_PRINCIPAL, $account->roles);
	$is_registration_admin = ($is_registration_admin || user_access('administer webform_ext', $account));
	
	return $is_registration_admin;
}