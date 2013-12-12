/* Handles error message display */
(function($, Drupal){
	Drupal.behaviors.we_errors_list = { 
		attach: function (context, settings) {
		   if (Drupal.settings.we_errors_list === undefined){
			  return; 
		   }
		   var err = Drupal.settings.we_errors_list.error_msgs;	
		   var ctr = 0;
		   var ul_element = "#block-system-main ul#errors";
		   for (var prop in err){
		      ctr++;
		      if (window.console && console.log){
				 console.log(ctr);
			  }		      
// 		      if (prop == ""){
// 		         return;
// 		      }
		   }	
		   if ($(ul_element).length > 0){
		      $(ul_element).remove(); 
		   }		   
		   // There are no errors to process
		   if (ctr == 0){
		      return;
		   }
		   var ul = $('<ul id="errors"></ul>');
		   var items = [];
		   $("#block-system-main").prepend(ul);
		   $.each(err, function(idx, value){		        
				if (window.console && console.log){
				   console.log(err[idx]);
				}
				items.push('<li>'+ err[idx] + '</li>');
		     }
		   );		   
		   $(ul_element).append(items.join(''));
		   $(ul_element).delay(12000).fadeOut(3000);
		} 
	}	
	Drupal.behaviors.we_date_desc = {
		attach: function (context) {
		   var descDiv = $(".container-inline-date .description", context);
		   if (descDiv !== undefined){
		      descDiv.text("e.g. YYYY-MM-DD");
		   }
		}
	}
	
})(jQuery, Drupal);