/* Handles error message display */
(function($, Drupal){
	Drupal.behaviors.we_errors_list = { 
		attach: function (context, settings) {
		   if (settings.we_errors_list === undefined) { return; }
		   var err = settings.we_errors_list.error_msgs;	
		   var items = [];
		   if (!err) { return; }
		   $.each(err, function(idx, value){
		      if (value.indexOf("***") == -1){
		   	  	 err[idx] = "*** " + value;
		   	  	 items.push('<li><span style="font-style: italic;">Error Detected: ' + value + '</span></li>');				 
		   	  }
		   });
		   if (window.console && console.log) { console.log(settings.we_errors_list.error_msgs) };
		   if (window.console && console.log) { console.log(items); }
// 		   Drupal.detachBehaviors($(this)); // must be accompanied by "detach" property declaration
	       if (items.length == 0) { return; }
	       
	       var ul = $("#block-system-main ul#errors");
	       if (ul.length > 0) { 
	          ul.remove();
	       }
	       
	       ul = $('<ul id="errors"></ul>');
	       $("#block-system-main").prepend(ul);	      
	       ul.append(items.join(''));
	       ul.delay(12000).fadeOut(3000);
		}
	}	
	Drupal.behaviors.we_date_desc = {
		attach: function (context) {
// 		   var descDiv = $(".container-inline-date .description", context);
		   var descDiv = $(".container-inline-date .description");	
		   if (descDiv !== undefined){
		      descDiv.text("e.g. YYYY-MM-DD");
		   }
		}
	}
	
})(jQuery, Drupal);