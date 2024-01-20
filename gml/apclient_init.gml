{
    if (!external_call(global.ext_apclient_init, argument0))
    {
        // init failed
        show_message("Error initializing apclient!");
    }
}
