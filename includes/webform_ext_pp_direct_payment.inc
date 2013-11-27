<?php

require(drupal_get_path('module', 'webform_ext'). '/vpgh_pp/PPBootStrap.php');

/*
 * Digital goods payments combine JavaScript with the Express Checkout API to streamline the checkout process for buyers of digital goods.
* Digital goods are items such as e-books, music files, and digital images distributed in electronic format. The buyer can conveniently purchase digital goods during checkout with a minimum of clicks without leaving your website or interrupting their online activities
*
*/

function PaypalDirectPaymentAuthLive2(&$res)
{

   $retVal = WEBFORM_EXT_FAILURE;
	
   if (!isset($res) || !$res){
      drupal_goto('<front>'); // send the user to the home page if nothing was submitted
   }   
	
   $firstName = $res->FirstName;
   $lastName = $res->LastName;
   
   /*
    * shipping address
   */
   $address = new AddressType();
   $address->Name = "$firstName $lastName";
   $address->Street1 = $res->StreetAddress1;
   $address->Street2 = $res->StreetAddress2;
   $address->CityName = $res->CityTown;
   $address->StateOrProvince = $res->ProvState;
   $address->PostalCode = $res->PostalZip;
   $address->Country = $res->Country;
   $address->Phone = $res->ContactNum;
   
   $paymentDetails = new PaymentDetailsType();
   $paymentDetails->ShipToAddress = $address;
   $paymentDetails->PaymentAction = WEBFORM_EXT_PP_TRANSACTION_TYPE;
   
   /* $sub_total = $res->TotalCharge; */
   $paymentDetails->OrderTotal = new BasicAmountType(WEBFORM_EXT_PP_TRANSACTION_CURRENCY, $res->TotalCharge);
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
   $payer->Payer = $res->Email;
   $payer->PayerCountry = $address->Country;
    
   $cardDetails = new CreditCardDetailsType();
   $cardDetails->CreditCardNumber = $res->creditcard[0]->CreditCardNumber;
   $cardDetails->CreditCardType = $res->creditcard[0]->CreditCardType; 
   $cardDetails->ExpMonth = $res->creditcard[0]->ExpMonth;
   $cardDetails->ExpYear = $res->creditcard[0]->ExpYear;
   $cardDetails->CVV2 = $res->creditcard[0]->CVV2;
   $cardDetails->CardOwner = $payer;
    
   $ddReqDetails = new DoDirectPaymentRequestDetailsType();
   $ddReqDetails->CreditCard = $cardDetails;
   $ddReqDetails->PaymentDetails = $paymentDetails;
   $ddReqDetails->IPAddress = $_SERVER["REMOTE_ADDR"];
   $ddReqDetails->ReturnFMFDetails = TRUE;
   
   $doDirectPaymentReq = new DoDirectPaymentReq();
   $doDirectPaymentReq->DoDirectPaymentRequest = new DoDirectPaymentRequestType($ddReqDetails);
   
   $paypalService = new PayPalAPIInterfaceServiceService();	
   $doDirectPaymentResponse = NULL;
   try {
      $doDirectPaymentResponse = $paypalService->DoDirectPayment($doDirectPaymentReq);		
   }catch (Exception $ex){	 
      $ex_type = get_class($ex);
	  if($ex instanceof PPConnectionException) {
	     $ex_detailed_message = "Error connecting to " . $ex->getUrl();
	  }else if($ex instanceof PPMissingCredentialException || $ex instanceof 			   PPInvalidCredentialException) {
	     $ex_detailed_message = $ex->errorMessage();
	  }else if($ex instanceof PPConfigurationException) {
	     $ex_detailed_message = "Invalid configuration. Please check your configuration file.";
	  }
	  watchdog(WEBFORM_EXT_MODULE_NAME, $ex_detailed_message .'\n'. $ex->getMessage(), array(), WATCHDOG_ERROR);	   
   }		
   
   /* $_SESSION['pmt'] = $doDirectPaymentResponse; */
   $res->PaymentProcessorResponse = $doDirectPaymentResponse;
   if ($doDirectPaymentResponse){	        		    
       $res->RequestStatus = strtoupper($doDirectPaymentResponse->Ack);
	   $retVal = $res->RequestStatus;
	   if (strtoupper($doDirectPaymentResponse->Ack) == WEBFORM_EXT_SUCCESS){	  
		  $res->TransactionNumber = $doDirectPaymentResponse->TransactionID; 
	      //drupal_goto(_webform_ext_destination_url(WEBFORM_EXT_NOTIFICATION_SUCCESS_PAGE));		
       }else{
		  $res->Errors = $doDirectPaymentResponse->Errors;
		  /* if (count($res->Errors) > 0){
		     foreach($res->Errors as $key => $error){
		        $msg = (isset($error->LongMessage) ? $error->LongMessage : WEBFORM_EXT_PP_ERROR_GENERIC_MSG);
		        drupal_set_message(t($msg), 'error');
			 }
		  } */
	      //drupal_goto(_webform_ext_destination_url(WEBFORM_EXT_NOTIFICATION_FAILURE_PAGE));
        } 				
    }else{	    
	   watchdog(WEBFORM_EXT_MODULE_NAME, 'doDirectPaymentResponse object was returned as null', array(), WATCHDOG_NOTICE);
	   //drupal_goto(_webform_ext_destination_url(WEBFORM_EXT_NOTIFICATION_FAILURE_PAGE));
   }    
   
   return $retVal;
}