document.write('<style type="text/css">');

var name='AKTUALITY';

var cookies = document.cookie;

var show ='none';

var a = cookies.indexOf(escape(name) + '=');

if (a != -1)
{
	a = a + (escape(name) + '=').length;
	b = cookies.indexOf(';', a);
	if (b == -1) 
		b = cookies.length;
	
	show = cookies.substring(a, b);
}

document.write('#events_hid { display: '+show+"; }");

document.write('</style>');

$(document).ready(function() {
  $( "#events" ).click(function() {
    if($("#events_hid").css("display") != 'none')
		{
      $("#events_hid").animate({height: '0'}, function() {$("#events_hid").hide();});
      document.cookie='AKTUALITY='+escape('none');
		}
		else
		{
			$("#events_hid").animate({height: '300'});
      $("#events_hid").show();
      document.cookie='AKTUALITY='+escape('block'); 
		}
  });
});
