<?php

require_once('PPSignatureAuthHandler.php');

class PPAuthenticationHandler implements IPPHandler {	
	
	public function handle($httpConfig, $request, $options) {
		$credential = $request->getCredential();
		if(isset($credential)) {
			$thirdPartyAuth = $credential->getThirdPartyAuthorization();
			if($thirdPartyAuth && $thirdPartyAuth instanceof PPTokenAuthorization) {
				$authSignature = AuthSignature::generateFullAuthString($credential->getUsername(), $credential->getPassword(), $thirdPartyAuth->getAccessToken(), $thirdPartyAuth->getTokenSecret(), $httpConfig->getMethod(), $httpConfig->getUrl());
				if($options['port'] == 'PayPalAPI' || $options['port'] == 'PayPalAPIAA') {
					$httpConfig->addHeader('X-PP-AUTHORIZATION', $authSignature);
				}
				else {
					$httpConfig->addHeader('X-PAYPAL-AUTHORIZATION', $authSignature);
				}
			}
			if($credential instanceof PPSignatureCredential) {
				$handler = new PPSignatureAuthHandler($credential);
			} else if($credential instanceof PPCertificateCredential) {
				$handler = new PPCertificateAuthHandler($credential);
			} else {
				throw new PPInvalidCredentialException();
			}
			$handler->handle($httpConfig, $request, $options);
		}
	}
}