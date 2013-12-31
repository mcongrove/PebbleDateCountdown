Pebble.addEventListener("ready", function(e) {

});

Pebble.addEventListener("showConfiguration", function(e) {
	var options = JSON.parse(window.localStorage.getItem("options"));
	
	var url = "http://wearewearable.com/date-countdown/?" +
			  "theme=" + encodeURIComponent(options["0"]) +
			  "&day=" + encodeURIComponent(options["1"][0]) +
			  "&month=" + encodeURIComponent(options["1"][1]) +
			  "&year=" + encodeURIComponent(options["1"][2]) +
			  "&hour=" + encodeURIComponent(options["1"][3]) +
			  "&minute=" + encodeURIComponent(options["1"][4]) +
			  "&label=" + encodeURIComponent(options["2"]);
	
	Pebble.openURL(url);
});

Pebble.addEventListener("webviewclosed", function(e) {
	if(e.response) {
		var options = JSON.parse(decodeURIComponent(e.response));
		
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