Pebble.addEventListener("ready", function(e) {

});

Pebble.addEventListener("showConfiguration", function(e) {
	var options = JSON.parse(window.localStorage.getItem("options"));
	
	var url = "http://sens.li/date-countdown/?" +
			  "theme=" + encodeURIComponent(options["0"]) +
			  "&day=" + encodeURIComponent(options["1"]) +
			  "&month=" + encodeURIComponent(options["2"]) +
			  "&year=" + encodeURIComponent(options["3"]) +
			  "&hour=" + encodeURIComponent(options["4"]) +
			  "&minute=" + encodeURIComponent(options["5"]);
	
	Pebble.openURL(url);
});

Pebble.addEventListener("webviewclosed", function(e) {
	if(e.response) {
		var options = JSON.parse(decodeURIComponent(e.response));
		
		console.log(JSON.stringify(options));
		
		window.localStorage.setItem("options", JSON.stringify(options));
		
		Pebble.sendAppMessage(options,
			function(e) {
				console.log("Successfully sent options to Pebble");
			},
			function(e) {
				console.log("Failed to send options to Pebble.\nError: " + e.error.message);
			}
		);
	}
});