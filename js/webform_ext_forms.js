/* Handles error message display */
(function($, Drupal){
	Drupal.behaviors.we_errors_list = { 
		attach: function (context, settings) {
		   if (Drupal.settings.we_errors_list === undefined){
			  return; 
		   }
		   var err = Drupal.settings.we_errors_list.error_msgs;		   
		   var ul = $('<ul id="errors"></ul>');
		   var items = [];
		   $("#block-system-main ul#errors").remove();
		   $("#block-system-main").prepend(ul);
		   $.each(err, function(idx, value){
		        items.push('<li>'+ err[idx] + '</li>');
				if(window.console && console.log){
				   console.log(err[idx]);
				}
		     }
		   );
		   $("#block-system-main ul#errors").append(items.join(''));
		   $("#block-system-main ul#errors").delay(12000).fadeOut(3000);
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