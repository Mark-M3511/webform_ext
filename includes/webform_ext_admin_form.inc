<?php

function _webform_ext_build_config_form($form, &$form_state)
{
    $sdk_path = '/'. drupal_get_path('module', 'webform_ext'). '/paypal_merchant_sdk';
   
    $form['webform_ext_config_form1'] = array(
		'#type' => 'fieldset',
		'#title' => t('Paypal SDK setup'),
	);
	// change 'vupoint.holbrookgoodman.com' to 'all' in production
	$form['webform_ext_config_form1']['webform_ext_config_marker1'] = array(
		'#markup' => '<a href="'. $sdk_path. '/samples/install.php">Run Paypal SDK for PHP installation</a>',	
	);
	
	$form['webform_ext_config_form2'] = array(
		'#type' => 'fieldset',
		'#title' => t('Paypal SDK Test'),
	);
	// change 'vupoint.holbrookgoodman.com' to 'all' in production
	$form['webform_ext_config_form2']['webform_ext_config_marker2'] = array(
	    '#prefix' => '<div>',
		'#markup' => '<a href="/webform_ext/paypalapi/testPPEC">Run Paypal API integration test for Express Checkout</a>',	
		'#suffix' => '</div>',
	);
	$form['webform_ext_config_form2']['webform_ext_config_marker3'] = array(
	    '#prefix' => '<div>',
		'#markup' => '<a href="/webform_ext/paypalapi/testPPDP">Run Paypal API integration test for Direct Payment</a>',
        '#suffix' => '</div>',		
	);
    
	$form['webform_ext_config_form2']['webform_ext_config_marker4'] = array(
	    '#prefix' => '<div>',
		'#markup' => '<a href="/webform_ext/paypalapi/testPPButtonCreate">Run Paypal API integration test for automatic Button Creation</a>',
        '#suffix' => '</div>',		
	);
	
	return $form;
}

