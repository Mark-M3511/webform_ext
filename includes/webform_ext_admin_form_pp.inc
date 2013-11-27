<?php

function _webform_ext_pp_api_config_form($form, &$form_state)
{
   $form['webform_ext_sdk_setup'] = array(
		'#type' => 'fieldset',
		'#title' => t('Paypal Integration Setup'),
		'#collapsible' => FALSE,
		'#collapsed'   => FALSE,
	);
	
	$form['webform_ext_sdk_setup']['webform_ext_pp_IPN'] = array(
		'#type' => 'textfield',
		'#title' => t('Instant Payment Notification (IPN) return url'),
		'#default_value' => variable_get('webform_ext_pp_IPN', ''),
		'#size' => 100,
		'#maxlength' => 250,
		'#description' => t('Enter the return url for IPN notification'),
		'#required' => FALSE,		
	);   
   
   return system_settings_form($form); 
}
