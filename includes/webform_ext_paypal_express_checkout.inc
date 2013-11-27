<?php

require(drupal_get_path('module', 'webform_ext'). '/vpgh_pp/PPBootStrap.php');
require_once(drupal_get_path('module', 'webform_ext'). '/includes/webform_ext_consts.inc');

/*
 * Digital goods payments combine JavaScript with the Express Checkout API to streamline the checkout process for buyers of digital goods.
* Digital goods are items such as e-books, music files, and digital images distributed in electronic format. The buyer can conveniently purchase digital goods during checkout with a minimum of clicks without leaving your website or interrupting their online activities
*
*/

function PaypalExpressCheckoutAPITest()
{

   $serverName = $_SERVER['SERVER_NAME'];
   $serverPort = $_SERVER['SERVER_PORT'];
   $url = dirname('http://'.$serverName.':'.$serverPort .$_SERVER['REQUEST_URI']);

   /*
    * URL to which the buyer's browser is returned after choosing to pay with PayPal. For digital goods, you must add JavaScript to this page to close the in-context experience.
    */
    // $returnUrl = $url."/DGdoExpressCheckout.php";
	// $returnUrl = $url."/DGdoExpressCheckout/";
	$returnUrl = $url."/PaypalDoExpressCheckoutPayment/";	
    /*
    * URL to which the buyer is returned if the buyer does not approve the use of PayPal to pay you. For digital goods, you must add JavaScript to this page to close the in-context experience
    */
    $cancelUrl = $url. "/DGsetEC.html.php" ;

    /*
     * Total cost of the transaction to the buyer. If shipping cost and tax charges are known, include them in this value. If not, this value should be the current sub-total of the order. If the transaction includes one or more one-time purchases, this field must be equal to the sum of the purchases. Set this field to 0 if the transaction does not include a one-time purchase such as when you set up a billing agreement for a recurring payment that is not immediately charged. When the field is set to 0, purchase-specific fields are ignored
     */
    $orderTotal = new BasicAmountType();
	$orderTotal->currencyID = 'USD';
    $orderTotal->value = '10.00';
    
    $taxTotal = new BasicAmountType();
    $taxTotal->currencyID = 'USD';
    $taxTotal->value = '0.0';
    
    $itemDetails = new PaymentDetailsItemType();
    $itemDetails->Name = 'VuPoint Guest Accommodation';
    

    $itemDetails->Amount = $orderTotal;
    /*
     * Item quantity. This field is required when you pass a value for ItemCategory. For digital goods (ItemCategory=Digital), this field is required. 
     */
    $itemDetails->Quantity = '1';
    /*
     * Indicates whether an item is digital or physical. For digital goods, this field is required and must be set to Digital
     */
    /* $itemDetails->ItemCategory =  'Digital'; */
	$itemDetails->ItemCategory =  'Physical';
    
    $PaymentDetails= new PaymentDetailsType();
    $PaymentDetails->PaymentDetailsItem[0] = $itemDetails;
    
    //$PaymentDetails->ShipToAddress = $address;
    $PaymentDetails->OrderTotal = $orderTotal;
    /*
     * How you want to obtain payment. When implementing parallel payments, this field is required and must be set to Order. When implementing digital goods, this field is required and must be set to Sale
     */
    $PaymentDetails->PaymentAction = 'Sale';
    /*
     * Sum of cost of all items in this order. For digital goods, this field is required. 
     */
    $PaymentDetails->ItemTotal = $orderTotal;
    $PaymentDetails->TaxTotal = $taxTotal;
    
    $setECReqDetails = new SetExpressCheckoutRequestDetailsType();
    $setECReqDetails->PaymentDetails[0] = $PaymentDetails;
    $setECReqDetails->CancelURL = $cancelUrl;
    $setECReqDetails->ReturnURL = $returnUrl;
    /*
     * Indicates whether or not you require the buyer's shipping address on file with PayPal be a confirmed address. For digital goods, this field is required, and you must set it to 0. It is one of the following values:
    
        0 – You do not require the buyer's shipping address be a confirmed address.
    
        1 – You require the buyer's shipping address be a confirmed address.
    
     */
    $setECReqDetails->ReqConfirmShipping = 0;
    /*
     * Determines where or not PayPal displays shipping address fields on the PayPal pages. For digital goods, this field is required, and you must set it to 1. It is one of the following values:
    
        0 – PayPal displays the shipping address on the PayPal pages.
    
        1 – PayPal does not display shipping address fields whatsoever.
    
        2 – If you do not pass the shipping address, PayPal obtains it from the buyer's account profile.
    
     */
    $setECReqDetails->NoShipping = 1;
    
    $setECReqType = new SetExpressCheckoutRequestType();
    $setECReqType->SetExpressCheckoutRequestDetails = $setECReqDetails;
    
    $setECReq = new SetExpressCheckoutReq();
    $setECReq->SetExpressCheckoutRequest = $setECReqType;

    // storing in session to use in DoExpressCheckout
    /* $_SESSION['amount'] = $_REQUEST['amount'];
    $_SESSION['currencyID'] = $_REQUEST['currencyId']; */
    
    
    /*
     * 	 ## Creating service wrapper object
    Creating service wrapper object to make API call and loading
    configuration file for your credentials and endpoint
    */
    $PayPal_service = new PayPalAPIInterfaceServiceService();
    $setECResponse = $PayPal_service->SetExpressCheckout($setECReq);
    // echo '<pre>';
    //print_r($setECResponse);
    // echo '</pre>';
    if($setECResponse->Ack == 'Success')
    {
    
    	$token = $setECResponse->Token;
		$PayPalURL = (WEBFORM_EXT_TEST_PP_USE_SANDBOX ? WEBFORM_EXT_TEST_PP_SANDBOX_URL : WEBFORM_EXT_TEST_LIVE_URL);
	    $PayPalURL = url($PayPalURL, array('query' => array('cmd' => '_express-checkout', 'token' => $token), 'https' => TRUE,));
	    /* $PayPalURL = url($url .'/PaypalDoExpressCheckoutPayment', array('query' => array('cmd' => '_express-checkout', 'token' => $token), 'https' => TRUE,)); */
		/* echo "<br /><br /><br /><br /><br /><br /><br /><br /><a href=\"$PayPalURL\">Click here to continue to $PayPalURL</a>";   */  
		drupal_goto($PayPalURL);
    }
    else {
    	var_dump($setECResponse);
    	echo "error in SetEC API call";
    }
}

function PaypalGetExpressCheckoutPayment()
{
  $query = drupal_get_query_parameters();
  $token = $query['token'];
  $PayerID = $query['PayerID'];
  // get request details
  $getExpressCheckoutDetailsRequest = new GetExpressCheckoutDetailsRequestType($token);
  $getExpressCheckoutReq = new GetExpressCheckoutDetailsReq();
  $getExpressCheckoutReq->GetExpressCheckoutDetailsRequest = $getExpressCheckoutDetailsRequest;
  //print_r($query);
  $paypalService = new PayPalAPIInterfaceServiceService();
  try {
	 /* wrap API method calls on the service object with a try catch */
	 $getECResponse = $paypalService->GetExpressCheckoutDetails($getExpressCheckoutReq);
  } catch (Exception $ex) {
	 // include_once("../Error.php");
	 include_once(drupal_get_path('module', 'webform_ext'). '/vpgh_pp/Error.php');
	 exit;
  }
  if(isset($getECResponse)) {
	echo "<table>";
	echo "<tr><td>Ack :</td><td><div id='Ack'>".$getECResponse->Ack."</div> </td></tr>";
	echo "<tr><td>Token :</td><td><div id='Token'>".$getECResponse->GetExpressCheckoutDetailsResponseDetails->Token."</div></td></tr>";
	echo "<tr><td>PayerID :</td><td><div id='PayerID'>".$getECResponse->GetExpressCheckoutDetailsResponseDetails->PayerInfo->PayerID."</div></td></tr>";
	echo "<tr><td>PayerStatus :</td><td><div id='PayerStatus'>".$getECResponse->GetExpressCheckoutDetailsResponseDetails->PayerInfo->PayerStatus."</div></td></tr>";
	echo "</table>";
	echo '<pre>';
	print_r($getECResponse);
	echo '</pre>';
  }
  //require_once '../Response.php';
  require(drupal_get_path('module', 'webform_ext'). '/vpgh_pp/Response.php');
}

function PaypalDoExpressCheckoutPayment()
{
  $query = drupal_get_query_parameters();
  $token = $query['token'];
  $PayerID = $query['PayerID'];
  
  $orderTotal = new BasicAmountType();
  $orderTotal->currencyID = 'USD';
  $orderTotal->value = '10.00';
  
  $paymentDetails= new PaymentDetailsType();
  $paymentDetails->OrderTotal = $orderTotal;
  
  $DoECRequestDetails = new DoExpressCheckoutPaymentRequestDetailsType();
  $DoECRequestDetails->PayerID = $PayerID;
  $DoECRequestDetails->Token = $token;
  $DoECRequestDetails->PaymentAction = 'Sale';
  $DoECRequestDetails->PaymentDetails[0] = $paymentDetails;
  
  $DoECRequest = new DoExpressCheckoutPaymentRequestType();
  $DoECRequest->DoExpressCheckoutPaymentRequestDetails = $DoECRequestDetails;

  $DoECReq = new DoExpressCheckoutPaymentReq();
  $DoECReq->DoExpressCheckoutPaymentRequest = $DoECRequest;
  
  $paypalService = new PayPalAPIInterfaceServiceService();
  try{
     $DoECResponse = $paypalService->DoExpressCheckoutPayment($DoECReq);
  }catch(Exception $ex){
    // include_once("../Error.php");
	 include_once(drupal_get_path('module', 'webform_ext'). '/vpgh_pp/Error.php');
	 exit; 
  }
  if(isset($DoECResponse)) {
	echo "<table>";
	echo "<tr><td>Ack :</td><td><div id='Ack'>$DoECResponse->Ack</div> </td></tr>";
	if(isset($DoECResponse->DoExpressCheckoutPaymentResponseDetails->PaymentInfo)) {
		echo "<tr><td>TransactionID :</td><td><div id='TransactionID'>". $DoECResponse->DoExpressCheckoutPaymentResponseDetails->PaymentInfo[0]->TransactionID."</div> </td></tr>";
	}
	echo "</table>";
	echo "<pre>";
	print_r($DoECResponse);
	echo "</pre>";
  }
  //require_once '../Response.php';
  require(drupal_get_path('module', 'webform_ext'). '/vpgh_pp/Response.php');
}

function PaypalDirectPaymentAuthTest()
{
   $serverName = $_SERVER['SERVER_NAME'];
   $serverPort = $_SERVER['SERVER_PORT'];
   $ipn_url = 'http://'.$serverName.':'.$serverPort .$_SERVER['REQUEST_URI'];	
	
   $firstName = 'HGTTest1';
   $lastName = 'HGTTest1';
   
   /*
    * shipping address
   */
   $address = new AddressType();
   $address->Name = "$firstName $lastName";
   $address->Street1 = '123 Any Street';
   $address->Street2 = '';
   $address->CityName = 'Any City';
   $address->StateOrProvince = 'ON';
   $address->PostalCode = 'M1N 1N0';
   $address->Country = 'CA';
   $address->Phone = '9999999999';
   
   $paymentDetails = new PaymentDetailsType();
   $paymentDetails->ShipToAddress = $address;   
   
   /*
    *  Total cost of the transaction to the buyer. If shipping cost and tax
   charges are known, include them in this value. If not, this value
   should be the current sub-total of the order.
   
   If the transaction includes one or more one-time purchases, this field must be equal to
   the sum of the purchases. Set this field to 0 if the transaction does
   not include a one-time purchase such as when you set up a billing
   agreement for a recurring payment that is not immediately charged.
   When the field is set to 0, purchase-specific fields are ignored.
   
   * `Currency Code` - You must set the currencyID attribute to one of the
   3-character currency codes for any of the supported PayPal
   currencies.
   * `Amount`
   */
   //$paymentDetails->OrderTotal = new BasicAmountType('USD', $_POST['amount']);
   $paymentDetails->OrderTotal = new BasicAmountType('USD', '1');
   /*
    * 		Your URL for receiving Instant Payment Notification (IPN) about this transaction. If you do not specify this value in the request, the notification URL from your Merchant Profile is used, if one exists.
   
   */
   if(isset($_REQUEST['notifyURL']))
   {
   	   $paymentDetails->NotifyURL = $_REQUEST['notifyURL'];
   }

   $personName = new PersonNameType();
   $personName->FirstName = $firstName;
   $personName->LastName = $lastName;
   
   //information about the payer
   $payer = new PayerInfoType();
   $payer->PayerName = $personName;
   $payer->Address = $address;
   /* $payer->PayerCountry = $_POST['country']; */
   $payer->PayerCountry = $address->Country;
   
   $cardDetails = new CreditCardDetailsType();
   $cardDetails->CreditCardNumber = '4714779266434933';
   
   /*
    *
   Type of credit card. For UK, only Maestro, MasterCard, Discover, and
   Visa are allowable. For Canada, only MasterCard and Visa are
   allowable and Interac debit cards are not supported. It is one of the
   following values:
   
   * Visa
   * MasterCard
   * Discover
   * Amex
   * Solo
   * Switch
   * Maestro: See note.
   `Note:
   If the credit card type is Maestro, you must set currencyId to GBP.
   In addition, you must specify either StartMonth and StartYear or
   IssueNumber.`
   */
   $cardDetails->CreditCardType = 'Visa'; //6/2018
   $cardDetails->ExpMonth = '06';
   $cardDetails->ExpYear = '2018';
   $cardDetails->CVV2 = '123';
   $cardDetails->CardOwner = $payer;
   
   $ddReqDetails = new DoDirectPaymentRequestDetailsType();
   $ddReqDetails->CreditCard = $cardDetails;
   $ddReqDetails->PaymentDetails = $paymentDetails;
   $ddReqDetails->IPAddress = $_SERVER["REMOTE_ADDR"];
   //$ddReqDetails->NotifyURL = $ipn_url;
   
   $doDirectPaymentReq = new DoDirectPaymentReq();
   $doDirectPaymentReq->DoDirectPaymentRequest = new DoDirectPaymentRequestType($ddReqDetails);
   
   /*
    * 		 ## Creating service wrapper object
   Creating service wrapper object to make API call and loading
   configuration file for your credentials and endpoint
   */
   $paypalService = new PayPalAPIInterfaceServiceService();
   try {
   	   /* wrap API method calls on the service object with a try catch */
   	   $doDirectPaymentResponse = $paypalService->DoDirectPayment($doDirectPaymentReq);
   } catch (Exception $ex) {
   	// include_once("../Error.php");
	include_once(drupal_get_path('module', 'webform_ext'). '/vpgh_pp/Error.php');
   	exit;
   }
   if (isset($doDirectPaymentResponse)) {
   	   echo "<table>";
   	   echo "<tr><td>Ack :</td><td><div id='Ack'>$doDirectPaymentResponse->Ack</div> </td></tr>";
   	   echo "<tr><td>TransactionID :</td><td><div id='TransactionID'>$doDirectPaymentResponse->TransactionID</div> </td></tr>";
   	   echo "</table>";
   	   echo "<pre>";
   	   print_r($doDirectPaymentResponse);
   	   echo "</pre>";
   }
   //require_once '../Response.php';
   require(drupal_get_path('module', 'webform_ext'). '/vpgh_pp/Response.php');
}

function PaypalDirectPaymentAuthLive()
{
	
   if (!isset($_SESSION['reservation'])){
      drupal_goto('<front>'); // send the user to the home page if nothing was submitted
   }
	
   $reservation = $_SESSION['reservation'];
   $cc_data = $_SESSION['cc_data'];
   $cc_num = $_SESSION['cc_num'];
	
   $firstName = $reservation->data[2][0];
   $lastName = $reservation->data[1][0];
   
   /*
    * shipping address
   */
   $address = new AddressType();
   $address->Name = "$firstName $lastName";
   $address->Street1 = $reservation->data[6][0];
   $address->Street2 = $reservation->data[7][0];
   $address->CityName = $reservation->data[8][0];
   $address->StateOrProvince = $reservation->data[9][0];
   $address->PostalCode = $reservation->data[10][0];
   $address->Country = $reservation->data[11][0];
   $address->Phone = $reservation->data[4][0];
   
   $paymentDetails = new PaymentDetailsType();
   $paymentDetails->ShipToAddress = $address;
   $paymentDetails->PaymentAction = 'Sale';
   // $paymentDetails->IPAddress = $_SERVER["REMOTE_ADDR"];
   // $paymentDetails->NotifyURL = 'url for IPN return';
   
   //$paymentDetails->OrderTotal = new BasicAmountType('USD', $_POST['amount']);
   $sub_total = _calculate_sub_total($reservation->data[13][0], $reservation->data[14][0], $reservation->data[5][0], $reservation->data[18][0], $reservation->data[21][0]);
   $paymentDetails->OrderTotal = new BasicAmountType('USD', "$sub_total");
   /*
    * Your URL for receiving Instant Payment Notification (IPN) about this transaction. If you do not specify this value in the request, the notification URL from your Merchant Profile is used, if one exists.
   
   */
   if (isset($_REQUEST['notifyURL']))
   {
   	   $paymentDetails->NotifyURL = $_REQUEST['notifyURL'];
   }

   $personName = new PersonNameType();
   $personName->FirstName = $firstName;
   $personName->LastName = $lastName;
    
   //information about the payer
   $payer = new PayerInfoType();
   $payer->PayerName = $personName;
   $payer->Address = $address;
   $payer->Payer = $reservation->data[3][0];
   $payer->PayerCountry = $address->Country;
    
   $cardDetails = new CreditCardDetailsType();
   /* $cardDetails->CreditCardNumber = $cc_data->data[1][0]; */
   $cardDetails->CreditCardNumber = $cc_num;
   $cardDetails->CreditCardType = $cc_data->data[2][0]; 
   $cardDetails->ExpMonth = $cc_data->data[4][0];
   $cardDetails->ExpYear = $cc_data->data[5][0];
   $cardDetails->CVV2 = $cc_data->data[3][0];
   $cardDetails->CardOwner = $payer;
    
   $ddReqDetails = new DoDirectPaymentRequestDetailsType();
   $ddReqDetails->CreditCard = $cardDetails;
   $ddReqDetails->PaymentDetails = $paymentDetails;
   
   $doDirectPaymentReq = new DoDirectPaymentReq();
   $doDirectPaymentReq->DoDirectPaymentRequest = new DoDirectPaymentRequestType($ddReqDetails);
   
   $paypalService = new PayPalAPIInterfaceServiceService();	
   $doDirectPaymentResponse = NULL;
   try {
      $doDirectPaymentResponse = $paypalService->DoDirectPayment($doDirectPaymentReq);		
   }catch (Exception $ex){	 
       if(isset($ex)){
		   $ex_type = get_class($ex);
		   if($ex instanceof PPConnectionException) {
		   	$ex_detailed_message = "Error connecting to " . $ex->getUrl();
		   } else if($ex instanceof PPMissingCredentialException || $ex instanceof 			PPInvalidCredentialException) {
		   	$ex_detailed_message = $ex->errorMessage();
		   } else if($ex instanceof PPConfigurationException) {
		   	$ex_detailed_message = "Invalid configuration. Please check your configuration file.";
		   }
		   watchdog(WEBFORM_EXT_MODULE_NAME, $ex_detailed_message .'\n'. $ex->getMessage(), array(), WATCHDOG_ERROR);
	   }
   }		
   
   $_SESSION['pmt'] = $doDirectPaymentResponse;
   if ($doDirectPaymentResponse){	        		    
	   if (strtoupper($doDirectPaymentResponse->Ack) == WEBFORM_EXT_SUCCESS){					  
	      _insert_pp_response_data_success($_SESSION['reservation_node']->nid, $reservation->sid, $ddReqDetails, $doDirectPaymentResponse); 
	      _set_reservation_status();
	      _finalize_booking_process();
	      drupal_goto(_webform_ext_destination_url(WEBFORM_EXT_NOTIFICATION_SUCCESS_PAGE));		
       }else{
	      _insert_pp_response_data_error($_SESSION['reservation_node']->nid, $reservation->sid, $ddReqDetails, $doDirectPaymentResponse);
		  _set_reservation_status(FALSE);
		  _finalize_booking_process(WEBFORM_EXT_FAILURE);		  
	      drupal_goto(_webform_ext_destination_url(WEBFORM_EXT_NOTIFICATION_FAILURE_PAGE));
        } 		  
   }else{	    
	   watchdog(WEBFORM_EXT_MODULE_NAME, 'doDirectPaymentResponse object was returned as null', array(), WATCHDOG_NOTICE);
	   _finalize_booking_process(WEBFORM_EXT_FAILURE);
	   drupal_goto(_webform_ext_destination_url(WEBFORM_EXT_NOTIFICATION_FAILURE_PAGE));
   }    
}