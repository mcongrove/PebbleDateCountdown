#Date Countdown for Pebble

A Pebble watch app for counting down the number of days until an event.

![PebbleDateCountdown](https://raw.github.com/mcongrove/PebbleDateCountdown/master/watchface.jpg)

###In-App Options

From inside the application you can click the `SELECT` button to access the settings. This allows you to change the time and date of the event being counted down to, as well as choose from a number of pre-set event titles.

###Event Title Options

* Days Remaining
* Days Until the Wedding
* Days Until Baby Arrives
* Days Until My Birthday
* Days Until Our Anniversary
* Days Until Christmas
* Days Until the New Year

###Acknowledgements

Special thanks to [Peter Hardy](http://github.com/phardy) for helping figure out how to get this watch face working and providing sample code.

###Known Issues

* Currently the Pebble SDK does not allow app settings to be saved. This means that you'll need to re-configure the application should you ever close it.
* Changing the values of a setting does not support click-and-hold
* Daylight savings time is not supported in countdown calculations
