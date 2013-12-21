<?php

/* require(drupal_get_path('module', 'webform_ext'). '/vpgh_pp/PPBootStrap.php'); */
require_once(drupal_get_path('module', 'webform_ext'). '/vpgh_pp2/PPBootStrap.php');
require_once(drupal_get_path('module', 'webform_ext'). '/includes/webform_ext_consts.inc');
require_once(drupal_get_path('module', 'webform_ext'). '/includes/webform_ext_funcs.inc');


function PaypalWPSButtonCreate()
{
   $protocol = (IS_SSL ? HTTPS : HTTP);
   
   $serverName = $_SERVER['SERVER_NAME'];
   $serverPort = $_SERVER['SERVER_PORT'];
   
   $url = dirname($protocol. $serverName.':'.$serverPort .$_SERVER['REQUEST_URI']);

   $buttonVar = array();
   $buttonVar[] = 'item_name=' . 'Room Stay';		
   $buttonVar[] = 'return=' . WEBFORM_EXT_PP_RETURN_URL;
   $buttonVar[] = 'business=' . (WEBFORM_EXT_TEST_PP_USE_SANDBOX ? WEBFORM_EXT_PP_BUSINESS_TEST : WEBFORM_EXT_PP_BUSINESS);
   $buttonVar[] = 'amount=1.00';
   $buttonVar[] = 'subtotal=1.00';
   $buttonVar[] = 'notify_url=' . WEBFORM_EXT_PP_NOTIFY_URL;
   $buttonVar[] = 'cancel_return='. WEBFORM_EXT_PP_RETURN_URL;
   $buttonVar[] = 'currency_code='. WEBFORM_EXT_PP_TRANSACTION_CURRENCY;
   //$buttonVar[] = 'paymentaction='. WEBFORM_EXT_PP_TRANSACTION_TYPE;
					
   //$buttonVar[] = "notify_url=" . $_REQUEST['notifyURL'];				
   /* $buttonVar[] = "first_name=Mark O.";				
   $buttonVar[] = "last_name=McFarlane";			
   $buttonVar[] = "address_override=1";
   $buttonVar[] = "address1=222 Anywhere St";	
   $buttonVar[] = "city=Toronto";			
   $buttonVar[] = "country=CA";			
   $buttonVar[] = "state=ON";			
   $buttonVar[] = "zip=L8K7J6";	 */	
   $buttonVar[] = "email=". variable_get('site_mail', '');    
   $buttonVar[] = "address1=222 Anywhere St";   
   $buttonVar[] = "billing_first_name=Mark";				
   $buttonVar[] = "billing_last_name=McFarlane";				
   $buttonVar[] = "billing_address1=222 Anywhere St";	
   $buttonVar[] = "billing_address2=Suite 222";	
   $buttonVar[] = "billing_city=Toronto";		
   $buttonVar[] = "billing_state=ON";	
   $buttonVar[] = "billing_country=CA";	
   $buttonVar[] = "billing_zip=L8K7J6";
   $buttonVar[] = "night_phone_a=";
   $buttonVar[] = "night_phone_b=905";			
   $buttonVar[] = "night_phone_c=1111111";
   $buttonVar[] = "showBillingAddress=true";
   $buttonVar[] = "showShippingAddress=false";
   $buttonVar[] = "showBillingEmail=true"; 
   $buttonVar[] = "showBillingPhone=true"; 
   $buttonVar[] = "showCustomerName=true"; 
   $buttonVar[] = "showCardInfo=true"; 
   /* $buttonVar[] = "bn=". WEBFORM_EXT_PP_TRANSACTION_CURRENCY;  */
   $buttonVar[] = "cbt=". 'Island Vacation Inn';
       
   //PAYMENT
   $createButtonRequest = new BMCreateButtonRequestType();
   $createButtonRequest->ButtonCode = WEBFORM_EXT_PP_STANDARD_BTN_CODE;
   $createButtonRequest->ButtonVar = $buttonVar;
   $createButtonRequest->ButtonType = WEBFORM_EXT_PP_STANDARD_BTN_TYPE;
    
   $createButtonReq = new BMCreateButtonReq();
   $createButtonReq->BMCreateButtonRequest = $createButtonRequest;	
 
   $paypalService = new PayPalAPIInterfaceServiceService();
   try{
      $createButtonResponse = $paypalService->BMCreateButton($createButtonReq);
   }catch (Exception $ex) {
   	// include_once("../Error.php");
   include_once(drupal_get_path('module', 'webform_ext'). '/vpgh_pp2/Error.php');
      exit;
   }
   if(isset($createButtonResponse)) {
      echo "<table>";
      echo "<tr><td>Ack :</td><td><div id='Ack'>$createButtonResponse->Ack</div> </td></tr>";
      echo "<tr><td>HostedButtonID :</td><td><div id='HostedButtonID'>". $createButtonResponse->HostedButtonID ."</div> </td></tr>";
      echo "<tr><td>Email :</td><td><div id='Email'>". $createButtonResponse->Email ."</div> </td></tr>";
      echo "</table>";
      		
      echo "<pre>";
      print_r($createButtonResponse);
      echo "</pre>";
   }
   //require_once '../Response.php';
   require(drupal_get_path('module', 'webform_ext'). '/vpgh_pp2/Response.php');
					
}

function PaypalWPSIPNCallBack()
{
   $ipnMessage = new PPIPNMessage();
   foreach($ipnMessage->getRawData() as $key => $value) {
   	 error_log("IPN: $key => $value");
   }
   
   print_r($ipnMessage->getRawData());
   
   if($ipnMessage->validate()) {
   	  error_log("Success: Got valid IPN data");		
   }else{
   	  error_log("Error: Got invalid IPN data");	
   }
}

function PayWithPayPal()
{
   $retVal = '';
   $protocol = (IS_SSL ? HTTPS : HTTP);
   
   $serverName = $_SERVER['SERVER_NAME'];
   $serverPort = $_SERVER['SERVER_PORT'];
   /* $url = $protocol. $serverName.':'.$serverPort .$_SERVER['REQUEST_URI']; */
   $url = $protocol. WEBFORM_EXT_HOST;
   
   if (!isset($_SESSION['reservation']) || count($_SESSION['reservation']) == 0){
      drupal_goto('<front>');
   }
   
   $res = $_SESSION['reservation'][0];
   $arrival_date = $res->ArrivalDate;
   $departure_date = $res->DepartureDate;
   $billinginfo = $_SESSION['billing_info'];

   $buttonVar = array();
   $buttonVar[] = "item_name={$billinginfo->ItemName}";
   $buttonVar[] = "item_number={$res->ReservationUid}";   
   $buttonVar[] = 'return=' . $billinginfo->ReturnURL;
   $buttonVar[] = "business={$billinginfo->RegisteredBusinessEmail}";
   $buttonVar[] = "amount={$billinginfo->OrderTotal}";
   $buttonVar[] = "subtotal={$billinginfo->OrderTotal}";
   $buttonVar[] = 'notify_url=' . $billinginfo->NotifyURL;
   $buttonVar[] = "cancel_return={$billinginfo->CancelUrl}";
   $buttonVar[] = "currency_code={$billinginfo->CurrencyCode}";
   $buttonVar[] = "billing_first_name={$res->FirstName}";				
   $buttonVar[] = "billing_last_name={$res->LastName}";		
   $buttonVar[] = "email={$res->Email}";   
   $buttonVar[] = "billing_address1={$res->StreetAddress1}";	
   $buttonVar[] = "billing_address2={$res->StreetAddress2}";	
   $buttonVar[] = "billing_city={$res->CityTown}";		
   $buttonVar[] = "billing_state={$res->ProvState}";	
   $buttonVar[] = "billing_country={$res->Country}";	
   $buttonVar[] = "billing_zip={$res->PostalZip}";
   PaypalParseTelNum(&$buttonVar, $res);
   $buttonVar[] = "showBillingAddress=true";
   $buttonVar[] = "showShippingAddress=false";
   $buttonVar[] = "showBillingEmail=true"; 
   $buttonVar[] = "showBillingPhone=true"; 
   $buttonVar[] = "showCustomerName=true"; 
   $buttonVar[] = "showCardInfo=true";    
   //send over room numbers appended to "custom" string e.g. 1|2|3|4
   $custom_rec = "{$res->ReservationUid}|$arrival_date|$departure_date";
   if (isset($res->rooms) && count($res->rooms)){
      foreach($res->rooms as $key => $room){ 
        $custom_rec .= "|{$room->RoomNumber}";
	  }
   }
   $buttonVar[] = "custom={$custom_rec}";    
   //PAYMENT
   $createButtonRequest = new BMCreateButtonRequestType();
   $createButtonRequest->ButtonCode = WEBFORM_EXT_PP_STANDARD_BTN_CODE;
   $createButtonRequest->ButtonVar = $buttonVar;
   $createButtonRequest->ButtonType = WEBFORM_EXT_PP_STANDARD_BTN_TYPE;
    
   $createButtonReq = new BMCreateButtonReq();
   $createButtonReq->BMCreateButtonRequest = $createButtonRequest;	
   
   //print_r($buttonVar);
 
   $paypalService = new PayPalAPIInterfaceServiceService();
   try{
      $createButtonResponse = $paypalService->BMCreateButton($createButtonReq);
   }catch (Exception $ex) {
      // include_once("../Error.php");
      //include_once(drupal_get_path('module', 'webform_ext'). '/vpgh_pp2/Error.php');
	  watchdog(WEBFORM_EXT_MODULE_NAME,  t($ex->getMessage()), array(), WATCHDOG_ERROR);
      //exit;
	  $retVal = t($ex->getMessage());
   }	
   if (isset($createButtonResponse) && strtoupper($createButtonResponse->Ack) == WEBFORM_EXT_SUCCESS) {
	  /* Website code for Button type: $createButtonResponse->Website */
	  /* Button ID: $createButtonResponse->HostedButtonID */
	  /* Payment URL: $createButtonResponse->Email */
	  $retVal = $createButtonResponse->Email;
	  drupal_goto($retVal);
   }
   
   return $retVal;
}

function PaypalParseTelNum(&$buttonVar, $res)
{
   if ($res->ContactNumCountryCode == '1'){
   	  $buttonVar[] = "night_phone_a={$res->ContactNumAreaCode}"; 
   	  $_3_digit_prefix = drupal_substr($res->ContactNumLineNum, 0, 3);	
   	  $buttonVar[] = "night_phone_b={$_3_digit_prefix}";
   	  $_4_digit_line = drupal_substr($res->ContactNumLineNum, -4);
   	  $buttonVar[] = "night_phone_c={$_4_digit_line}";
   }else{
   	  $buttonVar[] = "night_phone_a={$res->ContactNumCountryCode}"; 
   	  $buttonVar[] = "night_phone_b={$res->ContactNumAreaCode}{$res->ContactNumLineNum}"; 	
   }
}

function PaypalPPSIPNCallBackLive()
{
   $_SESSION['rates'] = _get_current_rates();
   $ipnMessage = new PPIPNMessage();
   $ipn_raw_data = array(); 
   if ($ipnMessage->validate()) {
   	  error_log("Success: Got valid IPN data");
      $ipn_raw_data = $ipnMessage->getRawData(); 
	  
	  $res = new StdClass();
	  $res->Rooms = array();
	  $res->Custom = explode('|', $ipn_raw_data['transaction_subject']);
      $res->FirstName = $ipn_raw_data['first_name'];
	  $res->LastName = $ipn_raw_data['last_name'];
	  $res->Email = $ipn_raw_data['payer_email'];
	  $res->StreetAddress1 = $ipn_raw_data['address_street'];
	  $res->CityTown = $ipn_raw_data['address_city'];
	  $res->ProvState = $ipn_raw_data['address_state'];
	  $res->PostalZip = $ipn_raw_data['address_zip'];
	  $res->Country = $ipn_raw_data['address_country_code'];
	  $res->TransactionDate = date('Y-m-d H:i:s',strtotime($ipn_raw_data['payment_date']));
      $res->TransactionNumber = $ipn_raw_data['txn_id'];
	  $res->IPNTrackingId = $ipn_raw_data['ipn_track_id'];
	  $res->RequestStatus = strtoupper($ipn_raw_data['payment_status']);
	  $res->ArrivalDate = $res->Custom[1];
	  $res->DepartureDate = $res->Custom[2];
	  $res->NumNights = round((strtotime($res->DepartureDate) - strtotime($res->ArrivalDate))/DAY_IN_SECS, 0);
	  $res->DailyRate = $_SESSION['rates']['rate'];
	  $res->DailyRateExtra = $_SESSION['rates']['rate_extra'];
	  $res->TransactionAmt = $ipn_raw_data['payment_gross'];
      $res->Note = '';
	  $res->ContactNumber_1 = '';
      $res->RoomCount = 0;	  
	  /* $res->ReservationUid = $ipn_raw_data['item_number']; */
	  $res->ReservationUid = $res->Custom[0];
	  $rows = _get_reservation_by_uid($res->ReservationUid);	  
	  if ($rows){
	     $res->NumGuests = _get_num_guests_by_uid($res->ReservationUid);
	     while($row = $rows->fetchAssoc()) {		
		    $res->Note = $row['note'];
		    $res->ContactNumber_1 = $row['contact_number_1'];			
			error_log('Email from registration: ' . $row['email']);
		    if (WEBFORM_EXT_USE_TEST_EMAIL){
			  $res->Email = $row['email'];	
            }			
		    break; // we only want the first record
		 }
	  }
	  //Room numbers appended to "custom" string e.g. 1|2|3|4
	  for($ctr = 3; $ctr < count($res->Custom); $ctr++){
	     $res->Rooms[] = $res->Custom[$ctr];
		 $res->RoomCount++;
		 error_log('Room #: ' . $res->Custom[$ctr]);
	  }
	  error_log('Custom: ' . $res->Custom[0]);	 
      error_log('Request Status: '. $res->RequestStatus);	  
	  _webform_ext_wrap_up_IPN_interaction($res);
   }else{
   	  error_log("Error: Got invalid IPN data");	
   } 
   if (WEBFORM_EXT_TEST_PP_LOG_IPN_DATA){   
	  foreach($ipnMessage->getRawData() as $key => $value) {
   	     error_log("IPN: $key => $value");
      }     
   } 
}