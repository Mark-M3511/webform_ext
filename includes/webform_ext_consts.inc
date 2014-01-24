<?php
 /* Module identifier */
 define('WEBFORM_EXT_MODULE_NAME', 'webform_ext');

 
 /* Node Types and Parameters */ 
 define('NODE_TYPE_AVAILABILITY_BLOCKER', 'availability_blocker');
 define('NODE_TYPE_BOOKING', 'booking');
 define('NODE_TYPE_EMAIL_MSG', 'email_message');
 define('NODE_TYPE_RATES', 'rates'); 

 define('NODE_TYPE_NEW_MSG', 'This entry automatically created.');

 define('NODE_TYPE_CHANGED_MSG', 'This entry automatically updated.');

 
 define('SQL_FETCH_LIMIT', 30); 
 define('DAY_IN_SECS', 60*60*24);
 define('MIN_OCCUPANCY_COUNT', 2);

 define('DATE_BLOCK_THRESHOLD', 1);
 define('WEBFORM_EXT_DIRECT_PAY_PAGE', 'webform_ext/doDirectPayment');

 define('WEBFORM_EXT_EMAIL_TITLE_SUCCESS', 'VuPoint Guest House');
 define('WEBFORM_EXT_NEG_CODE_ADMIN', 'A');
 define('WEBFORM_EXT_NEG_CODE_REQUEST', 'R');
 
 /* Success / Failure */
 define('WEBFORM_EXT_SUCCESS', 'SUCCESS');
 define('WEBFORM_EXT_FAILURE', 'FAILURE');
 define('WEBFORM_EXT_COMPLETED', 'COMPLETED');
 
 /*  Time zone */
 define('WEBFORM_EXT_DEFAULT_TIMEZONE', 'America/Toronto');
 
 /* Secure pages */
 define('IS_SSL', 0);
 define('HTTPS', 'https://'); 
 define('HTTP', 'http://'); 
 define('WEBFORM_EXT_HOST', $_SERVER['SERVER_NAME']);
 define('WEBFORM_EXT_NOTIFICATION_SUCCESS_PAGE', 'payment-success-notification');
 define('WEBFORM_EXT_NOTIFICATION_FAILURE_PAGE', 'payment-failure-notification');
 define('WEBFORM_EXT_NOTIFICATION_ADMIN_TXN_NOTIFICATION_PAGE', 'admin-transaction-notification');
 define('WEBFORM_EXT_RESERVATIONS_PATH_ALIAS', 'reservations');
 define('WEBFORM_EXT_PAYMENT_PATH_ALIAS', 'vpgh/payment');
 
 /* webform node ids */
 define('WEBFORM_EXT_RESERVATION_FORM_NID', 60);
 define('WEBFORM_EXT_PAYMENT_FORM_NID', 100);
 
 /* Rates */
 define('VPGH_DAILY_RATE', 85.00);
 define('VPGH_DAILY_RATE_EXTRA', 50.00);
 
 /* Defaults */
 // define('WEBFORM_EXT_COUNTRY_DEFAULT', 'CA');
 define('WEBFORM_EXT_COUNTRY_DEFAULT', 'US-CAN');
 define('WEBFORM_EXT_COUNTRY_CODE_DEFAULT', 1);
 define('WEBFORM_EXT_SELECT_DEFAULT', '-Select-');
 
 /* Test Flags */
 define('WEBFORM_EXT_TEST_PP_USE_SANDBOX', 1);
 define('WEBFORM_EXT_USE_TEST_EMAIL', 1);
 define('WEBFORM_EXT_TEST_ORDER_AMT_1_00', 0);
 define('WEBFORM_EXT_TEST_ORDER_AMT_EXCESS', 0);

 define('WEBFORM_EXT_TEST_PP_LOG_IPN_DATA', 1);
 
  /* Module Flags */
 define('WEBFORM_EXT_QUEUE_MAIL_SEND', 1);
 
 /* Messages */
 define('WEBFORM_EXT_PP_ERROR_GENERIC_MSG', 'Our payment processor could not approve the transaction at this time.');
 define('WEBFORM_EXT_PP_SUCCESS_GENERIC_MSG_1', 'Your payment card was successfully authorized. Thank you!');
 define('WEBFORM_EXT_PP_SUCCESS_GENERIC_MSG_2', 'You should receive an email with your billing details and other instructions for your stay, shortly.');
 define('WEBFORM_EXT_RES_SUCCESS_MSG_1', 'Your reservation has been accepted.');
 define('WEBFORM_EXT_RES_SUCCESS_MSG_2', 'Your order total in U.S. dollars: $');
 define('WEBFORM_EXT_RES_SUCCESS_MSG_3', 'Reservation information processed successfully.');
 define('WEBFORM_EXT_PP_ITEM_NAME', 'Room Stay: @num_guests guest(s) | @num_rooms room(s) | @from - @to | US $@amount');
 define('WEBFORM_EXT_NOTIFICATION_EMAIL_SUBJECT', 'Vupoint Guest House Reservation & Payment Confirmation');
 define('WEBFORM_EXT_GUEST_NOTIFICATION_EMAIL_SUBJECT', 'Vupoint Guest House Reservation & Payment Confirmation');
 define('WEBFORM_EXT_ADMIN_NOTIFICATION_EMAIL_SUBJECT', 'Vupoint Guest House Reservation');
 define('WEBFORM_EXT_NOTIFICATION_EMAIL_ERROR', 'Unable to send confirmation email.');
 define('WEBFORM_EXT_RESERVATION_ACCEPTED_MSG', 'Your reservation is accepted. Please enter your personal information for payment and confirmation of your stay.');
 define('WEBFORM_EXT_CHECKED_ROOMS_MSG', 'Please Note: Checked rooms will be included in your booking. Uncheck rooms you wish to exclude.');
 define('ERROR_AVAILABILITY', 'Unable to complete reservation. Please check your dates.');
 define('WEBFORM_EXT_NOTIFICATION_EMAIL_INFO', 'Email successfully sent to \'@email_to\' with subject \'@subject\'');
 define('WEBFORM_EXT_ORDER_VALUE_LIMIT_EXCEEDED_MSG', 'Order total must not exceed $!amount (!currency). Your order total is: $!order_total (!currency).');
 
 /* Paypal Flags */
 define('WEBFORM_EXT_PP_TRANSACTION_TYPE', 'Sale');
 define('WEBFORM_EXT_PP_TRANSACTION_CURRENCY', 'USD');
 define('WEBFORM_EXT_PP_CHARGE_LIMIT', 10000);
 
 /* Paypal Button Type Selection */
 //define('WEBFORM_EXT_PP_STANDARD_BTN_CODE', 'TOKEN');
 //define('WEBFORM_EXT_PP_STANDARD_BTN_TYPE', 'PAYMENT');
 define('WEBFORM_EXT_PP_STANDARD_BTN_CODE', 'ENCRYPTED');
 define('WEBFORM_EXT_PP_STANDARD_BTN_TYPE', 'BUYNOW');
 

 /* URLs */
 define('WEBFORM_EXT_PP_RETURN_URL', WEBFORM_EXT_HOST. '/admin/config/content/webform_ext');
 define('WEBFORM_EXT_PP_NOTIFY_URL', WEBFORM_EXT_HOST. '/webform_ext/paypalapi/testipncallback');
 define('WEBFORM_EXT_PP_NOTIFY_URL_FRAGMENT', '/vpgh/pp_ipn_listener');
 define('WEBFORM_EXT_PP_REVIEW_PAY', 'vpgh/review_pay');
 define('WEBFORM_EXT_TEST_PP_SANDBOX_URL', 'https://www.sandbox.paypal.com/cgi-bin/webscr'); 
 define('WEBFORM_EXT_TEST_LIVE_URL', 'https://www.paypal.com/cgi-bin/webscr');
 define('WEBFORM_EXT_PAYMENT_FORM', 'payment-form');
 define('ROOMS_AND_FEATURES', 'rooms_and_features');
 define('CALENDAR_VIEW_NAME', 'booking_calendar');
 define('CALENDAR_VIEW_TARGET_DISPLAY', 'page_4'); 
 
 /* Mail Keys */  
 define('WEBFORM_EXT_EMAIL_KEY', 'booking_notification');
 define('WEBFORM_EXT_GUEST_RESERVATION_NOTIFICATION_KEY', 'booking_notification');
 define('WEBFORM_EXT_ADMIN_RESERVATION_NOTIFICATION_KEY', 'admin_booking_notification');

 /* Email Addresses */

 define('WEBFORM_EXT_PP_BUSINESS', 'star.light@rogers.com');
 define('WEBFORM_EXT_PP_BUSINESS_TEST', 'ebilling-facilitator@holbrookgoodman.com');
 // star.light@rogers.com|mark.mcfarlane@gmail.com|farid.tahira@gmail.com
 define('WEBFORM_EXT_TEST_EMAIL', 'mark.mcfarlane@gmail.com');
 
 /* Miscellaneous */
 define('WEBFORM_EXT_MANDRILL_TEMPLATE_NAME', 'vgph-reservation-confirmation');
 define('WEBFORM_EXT_FORM_SUBMIT_BTN_CAPTION', 'Reserve Now');
 define('WEBFORM_EXT_MIN_DATE_FOR_RESERVATION', '+1 days');
 define('WEBFORM_EXT_MIN_DATE_FOR_RESERVATION_2', '+7 days');
 define('WEBFORM_EXT_QUEUE_CREATE_BLOCKING_NODES', 'create_blocking_nodes');
 define('WEBFORM_EXT_QUEUE_CONFIRMATION_EMAILS', 'send_booking_confirmation_mail');
 
 /* Cron */
 define('WEBFORM_EXT_CRON_5MINS', '*/5 * * * *');
 
 /* Cron API task identifiers */
 define('WEBFORM_EXT_CRONAPI_BLOCKED_DATES', 'cron_queue_blocked_dates');
 
 /* Roles and Permissions */
 define('WEBFORM_EXT_ROLE_PRINCIPAL', 'principal');
 