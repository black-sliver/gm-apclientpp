# GM-APClientPP GML

This is work in progress.

This contains GML (script) code to put into the game.
Note that you have to run the code in [`_init.gml`](init.gml) on startup, or at latest before you first use apclient.
[`ap_init.gml`](ap_init.gml) sets up the DLL calls and constants and
[`apclient_wrappers.gml`](apclient_wrappers.gml) provides GM functions for each DLL call.

[`events/`](events/) will contain sample GML code to run when receiving events.
You need to implement all of them as described in the [main README file](../README.md).
