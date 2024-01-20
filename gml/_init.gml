{
    // run this on startup

    // initialize global.AP_* and global.ext_* for apclient_* calls to work
    ap_init();

    // init apclient lib
    apclient_init(1);
}
