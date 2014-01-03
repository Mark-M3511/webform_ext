<?php
  
// require_once(drupal_get_path('module', 'webform_ext'). '/includes/webform_ext_consts.inc'); 
// require_once(drupal_get_path('module', 'webform_ext'). '/includes/webform_ext_funcs.inc');

module_load_include('inc', 'webform_ext', 'includes/webform_ext_consts');
module_load_include('inc', 'webform_ext', 'includes/webform_ext_funcs');

function _webform_ext_payment_form_1($form, &$form_state)
{   

   $_SESSION['reservation'] = array();
   $res = new_reservation_object_default();
   $_SESSION['reservation'][] = $res;
   
   if (isset($_SESSION['billing_info'])){
     $res->BillingInfo = $_SESSION['billing_info']; 
   }   
   
   if (isset($_SESSION['reservation_room_picks'])){
      $res->rooms = $_SESSION['reservation_room_picks'];
   }

   $form = array();   

   $form['fs_reservation_info'] = array(
     '#title' => t("Reservation Information:"),
     '#prefix' => '<div id="reservation-info">',
     '#suffix' => '</div>',
     '#type' => 'fieldset',
     /* '#description' => t('Enter all required information indicated with an [*]'), */
   );
   
   $form['fs_reservation_info']['accept_res_msg'] = array(
      '#markup' => '<p>'. WEBFORM_EXT_RESERVATION_ACCEPTED_MSG. '</p>',
      '#type' => 'markup', 
	  '#prefix' => '<div id="accept-reservation-msg">',
      '#suffix' => '</div>',
   );
   
   $form['fs_reservation_info']['res_detail_msg'] = array(
      '#markup' => "<p>{$_SESSION['billing_info']->ItemName}</p>",
      '#type' => 'markup', 
	  '#prefix' => '<div id="reservation-detail-msg">',
      '#suffix' => '</div>',
   );
   
   $form['fs_contact_info'] = array(
     '#title' => t("Contact information of purchaser:"),
     '#prefix' => '<div id="contact-info">',
     '#suffix' => '</div>',
     '#type' => 'fieldset',
     '#description' => t('Enter all required information indicated with an [*]'),
   );
   
   $form['fs_contact_info']['salutation'] = array(
      '#title' => t('Salutation:'),
	  '#type' => 'select',
      '#options' => drupal_map_assoc(array(t('Mr.'),t('Mrs.'), t('Miss'), t('Dr.'), t('Ms.'),)),
	  '#default_value' => $res->Salutation, 
      '#required' => TRUE,
	  '#empty_value' => t('Salutation'),
   );   
   
   $form['fs_contact_info']['firstname'] = array(
      '#title' => t('First Name:'),
	  '#type' => 'textfield',
	  '#default_value' => check_plain($res->FirstName), 
	  '#size' => 20, 
      '#maxlength' => 30, 
      '#required' => TRUE,
   );
   
   $form['fs_contact_info']['lastname'] = array(
      '#title' => t('Last Name:'),
	  '#type' => 'textfield',
	  '#default_value' => check_plain($res->LastName), 
	  '#size' => 20, 
      '#maxlength' => 30, 
      '#required' => TRUE,
   );
   
   $form['fs_contact_info']['email'] = array(
      '#title' => t('Email:'),
	  '#type' => 'textfield',
	  '#default_value' => $res->Email, 
	  '#size' => 40, 
      '#maxlength' => 50, 
      '#required' => TRUE,
   );
   
   $form['fs_contact_info']['country_code'] = array(
      '#title' => t('Country Code:'),
	  '#type' => 'select',
	  '#options' => _webform_ext_load_country_codes(),
	  '#default_value' => check_plain($res->ContactNumCountryCode), 
      '#required' => TRUE,
   );
   
   $form['fs_contact_info']['area_code'] = array(
      '#title' => t('Area Code:'),
	  '#type' => 'textfield',
	  '#default_value' => check_plain($res->ContactNumAreaCode), 
	  '#size' => 6, 
      '#maxlength' => 4, 
      '#required' => TRUE,
   ); 
   
   /* $form['fs_contact_info']['prefix_code'] = array(
     '#title' => t('Prefix Code:'),
     '#type' => 'textfield',
     '#default_value' =>$res->ContactNumPrefixCode, 
     '#size' => 10, 
     '#maxlength' => 4, 
     '#required' => TRUE,
   ); */
   
   $form['fs_contact_info']['line_code'] = array(
     '#title' => t('Telephone Number:'),
     '#type' => 'textfield',
     '#default_value' => check_plain($res->ContactNumLineNum), 
     '#size' => 15, 
     '#maxlength' => 15, 
     '#required' => TRUE,
   );
   
   $form['fs_contact_info']['street_address_1'] = array(
      '#title' => t('Street Address:'),
	  '#type' => 'textfield',
	  '#default_value' => check_plain($res->StreetAddress1), 
	  '#size' => 40, 
      '#maxlength' => 50, 
      '#required' => TRUE,
   );
   
   $form['fs_contact_info']['street_address_2'] = array(
      '#title' => t('Street Address (Optional):'),
	  '#type' => 'textfield',
	  '#default_value' => check_plain($res->StreetAddress2), 
	  '#size' => 40, 
      '#maxlength' => 50, 
   );
   
   $form['fs_contact_info']['city_town'] = array(
      '#title' => t('City:'),
	  '#type' => 'textfield',
	  '#default_value' => check_plain($res->CityTown), 
	  '#size' => 20, 
      '#maxlength' => 30, 
      '#required' => TRUE,
   );
   
   $form['fs_contact_info']['country'] = array(
      '#title' => t('Country:'),
	  '#type' => 'select',
	  '#options' => _webform_ext_load_country_names(),
	  '#default_value' => $res->Country, 
      '#required' => TRUE,
	  '#description' => t('Attention: If American or Canadian, please select the Canada/United States option followed by your province or state.'),
   );
   
   $form['fs_contact_info']['prov_state_NA'] = array(
      '#title' => t('Prov./State:'),
	  '#type' => 'select',
      '#options' => _webform_ext_load_states_prov_NA(),
	  '#states' => array(
          'visible' => array(			 			
			   ':input[name="country"]' => array("value" => "US-CAN"),
            )
         ),
   );
   
   $form['fs_contact_info']['prov_state'] = array(
      '#title' => t('Prov./State:'),
	  '#type' => 'textfield',
	  '#default_value' => $res->ProvState, 
	  '#size' => 30, 
      '#maxlength' => 30, 
      '#states' => array(
         'invisible' => array(
			':input[name="prov_state_NA"]' => array('visible' => TRUE),
         ),
       ),
   );  
      
   $form['fs_contact_info']['postal_zip'] = array(
      '#title' => t('Postal/Zip:'),
	  '#type' => 'textfield',
	  '#default_value' => check_plain($res->PostalZip), 
	  '#size' => 15, 
      '#maxlength' => 20, 
      '#required' => TRUE,
   );
         
   $form['fs_notes_info'] = array(
     '#title' => t("Additional information for VPGH:"),
     '#prefix' => '<div id="notes">',
     '#suffix' => '</div>',
     '#type' => 'fieldset',
     '#description' => t('500 characters maximum including spaces'),
   );
   
   $form['fs_notes_info']['notes'] = array(
      '#title' => t(''),
	  '#type' => 'textarea',
	  '#default_value' => check_plain($res->Note), 
      '#maxlength' => 500, 
	  '#resizable' => FALSE,
	  '#cols' => 15,
	  '#wows' => 10,
	   
   );
   
   $payment_btn_caption = 'Proceed to Pay';
   
   if (_webform_ext_user_is_content_admin()){
      $form['fs_admin_only'] = array(
     	'#title' => t("Administrative Use:"),
     	'#prefix' => '<div id="admin-use">',
     	'#suffix' => '</div>',
     	'#type' => 'fieldset',
   	  ); 
   	  $form['fs_admin_only']['offline_booking'] = array(
      	'#title' => t("This booking was not made through {$_SERVER['SERVER_NAME']}:"),
	  	'#type' => 'checkbox',
	  	'#default_value' => 1, 	   
      );
      $payment_btn_caption = 'Submit Transaction';		
   }
   
   $form['submit'] = array(
     '#type' => 'submit',
     '#value' => t($payment_btn_caption),
   );
   
   return $form;
}

/*
*  Ajax call back
*/
function _webform_ext_payment_form_1_validate($form, &$form_state) {
   $email = filter_var($form_state['values']['email'], FILTER_VALIDATE_EMAIL);
   $salutation = ($form_state['values']['salutation'] != $form['fs_contact_info']['salutation']['#empty_value']);   
   $order_total = $_SESSION['order_total'] > 0;
   if (!$email){
      form_set_error('email', t('Email address provided does not appear to be valid.'));
   }
   if (!$salutation){
      form_set_error('salutation', t('Salutation: field is required.'));
   }
   if (!$order_total){
      form_set_error('order_total', t('Order amount must be greater than 0.')); 
   }
   $error_msgs = form_get_errors();
   drupal_add_js(array('we_errors_list' => array('error_msgs' => $error_msgs)), 'setting');
}

function _webform_ext_payment_form_1_submit($form, &$form_state) {
    _webform_ext_get_reservation_values($form_state, $_SESSION['reservation'][0]);
	$res = $_SESSION['reservation'];
	_webform_ext_save_reservation_to_db($res);
	_webform_ext_save_room_bookings_to_db($res);
	foreach($res as $r){
	   foreach($r->rooms as $key => $room){
		  $params = array('rid' => $room->RoomNumber, 'rbid' => 0, 'nid' => 0, 'booked_date' => $res[0]->ArrivalDate, 'reservation_uid' => $res[0]->ReservationUid,);
		  _create_room_booking_dates_detail($params, strtotime($res[0]->ArrivalDate), strtotime($res[0]->DepartureDate)); 
	   }	
	}
	
	//  send the guest off to pay
	if (_webform_ext_user_is_content_admin() && !empty($form_state['values']['offline_booking'])){
		_webform_ext_admin_transaction($res[0]);
// 		drupal_set_message(WEBFORM_EXT_RES_SUCCESS_MSG_3);
		drupal_goto(WEBFORM_EXT_NOTIFICATION_ADMIN_TXN_NOTIFICATION_PAGE);	
	}else{
	    drupal_goto(WEBFORM_EXT_PP_REVIEW_PAY);	
	}
}