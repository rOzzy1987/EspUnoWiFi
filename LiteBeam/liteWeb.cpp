#include <ESP8266WiFi.h>
#include "common.h"
#include "settings.h"
#include "resources.h"
#include "favicon.h"
#include "liteWeb.h"
#include "stk500impl.h"

#define CLIENT_ARR_SIZE 8

// somehow null is added at the end so we decrease size by 1
#define  fileResponse(wc, ctype, resource) wc.printf(head_frm, 200, ctype, sizeof(resource) - 1); \
                                           wc.write_P(resource, sizeof(resource) - 1);

#define  binFileResponse(wc, ctype, resource) wc.printf(head_frm, 200, ctype, sizeof(resource)); \
                                              wc.write_P((const char*)(void*)resource, sizeof(resource));

#define stringResponse(wc, status, str) wc.printf(head_frm, status, ctPlain, str.length()); \
                                        wc.print(str);

#define Inc(_idx) _idx = (_idx + 1)  % CLIENT_ARR_SIZE 

static const char *head_frm =
    "HTTP/1.1 %d OK\r\n"
    "Content-Type: %s\r\n"
    "Access-Control-Allow-Origin:*\r\n"
    "Content-Length: %d\r\n"
    "Connection : close\r\n\r\n";

static const char *ctHtml = "text/html";
static const char *ctJs = "text/javascript"; 
static const char *ctJson = "application/json"; 
static const char *ctCss = "text/css"; 
static const char *ctPlain = "text/plain"; 
static const char *ctIco = "image/x-icon";

static WiFiServer webServer(SRV_WEB_PORT);
static WiFiClient clients[CLIENT_ARR_SIZE];
static unsigned int lastIdx = 0;


static void (*reset_slave)();
static void (*change_ap)();
static void (*change_st)();
static void (*update_mdns)();

void init_web(
    void (*reset_slave_fn)(),
    void (*change_ap_fn)(),
    void (*change_st_fn)(),
    void (*update_mdns_fn)()) {
    reset_slave = reset_slave_fn;
    change_ap = change_ap_fn;
    change_st = change_st_fn;
    update_mdns = update_mdns_fn;
    webServer.begin();
}

/// @brief Gets a substring until a given character and sets the index to the position after the found character
/// @param str source string
/// @param c character to find
/// @param n index to start  from
/// @return substring to given character or the whole string if not found
static String get_until_char(String *str, char c, unsigned int* n) {
    unsigned int i = n[0];
    unsigned int j = str->indexOf(c, n[0]);

    if (j < n[0]) {
        j = str->length();
    }
    n[0] = j + 1;
    return str->substring(i, j);
}

/// @brief Finds the next slot in the queue of clients where the client is null or already disconnected
/// and fills it in with a new pending client
static void setup_next_client() {
    if (webServer.hasClient()) {
        for (int i = 0; i < CLIENT_ARR_SIZE; i++, Inc(lastIdx)) {
            if (!clients[lastIdx]) break;
            if (!clients[lastIdx].connected()) break;
        }

        if (clients[lastIdx])
            clients[lastIdx].stop();
        clients[lastIdx] = webServer.accept();
        clients[lastIdx].setNoDelay(true);
    }
} 

/// @brief Returns the method part of the HTTP header
/// @param reqStr the request query
/// @param n will be set to the index to continue parsing from  
/// @return the HTTP request method or "get" if not found (shouldn't be possible when using a browser)
static String get_method(String *reqStr, unsigned int* n) {
    String s = get_until_char(reqStr, ' ', n);
    if (s.length() > 6)
        return "get";
    return s;
}

/// @brief Returns the relative path of the HTTP header
/// @param reqStr the rquest query
/// @param n the index to search from; will be set  to the end index + 1 
/// @return the relative path of the HTTP query or "" if not found (shouldn't be possible when using a browser)
static String get_path(String *reqStr, unsigned int* n) {

    // finding the third slash in the URL (http://domain/path)
    unsigned int i = n[0];
    i = reqStr->indexOf('/', i) + 1;
    i = i <= 0 ? i : reqStr->indexOf('/', i) + 1;
    i = i <= 0 ? i : reqStr->indexOf('/', i) + 1;

    if(i == 0) 
        return  "";

    unsigned int j = reqStr->lastIndexOf('?');
    if (j < i) {
        j = reqStr->lastIndexOf(' ');
        if (j < i)
            return "";
    }
    n[0] = j + 1;
    return reqStr->substring(i, j);
} 

/// @brief Returns the query string from the HTTP header
/// @param reqStr the rquest query
/// @param n the index to search from; will be set  to the end index + 1 
/// @return 
static String get_query(String *reqStr, unsigned int* n) {
    unsigned int i = reqStr->lastIndexOf(' ');
    if (i < n[0])
        return "";

    return reqStr->substring(n[0], i);
}

/// @brief Extracts a parameter name from a query string (or post body)
/// @param q the query string
/// @param n the index to start from
/// @return the parameter name
static String get_var_name(String *q, unsigned int* n) {
    return get_until_char(q, '=', n);
}

/// @brief Extracts a parameter value from the query string (or post body)
/// @param q the query string
/// @param n the index to start from
/// @return the parameter value
static String get_var_value(String *q, unsigned int* n) {
    String raw = get_until_char(q, '&', n);
    int l = raw.length();
    char res[l + 1];
    int j = 0;
    for(int i  = 0; i < l; i++, j++ ) {
        if (raw[i] == '%')  {
            res[j] = ((HexToByte(raw[++i]) << 4) | HexToByte(raw[++i]));
        } else {
            res[j] = raw[i];
        }
    }
    res[j] = 0;
    return String(res);
}

/// @brief Handles queries that are not bound to specific urls
/// @param query the query string
/// @param client the client to respond to
static void handle_universal_queries(String *query, WiFiClient *client) {
    SettingsStruct *s = get_settings();
    unsigned int n = 0;
    while (n < query->length()){
        String name = get_var_name(query, &n);
        String val =  get_var_value(query, &n);
        if (name == "baudrate") {
            uint32_t baud = val.toInt();
            if (baud == 0 || baud > 2000000) 
                continue;
            s->baudrate = baud;
            save_eeprom();
            Serial.begin(s->baudrate);
        } else if (name == "baudrateisp") {
            uint32_t baud = val.toInt();
            if (baud == 0 || baud > 2000000) 
                continue;
            s->baudrate_isp = val.toInt();
            save_eeprom();
        }
    }
}

/// @brief Handle HEX uploads
/// @param client the HTTP client to respond to
static void handle_upload(WiFiClient *client)  {
    String line;
    int hexLen = 0;
    do {
        if (!client->available()) break;
        line = client->readStringUntil('\n');
        line.toLowerCase();
        if (line.indexOf("content-length") >= 0) {
            hexLen = line.substring(line.indexOf(':') + 1).toInt();
        }
    } while (line != String("\r"));
    int total_write;
    
    int handLen = handle_hex(&webServer, client, hexLen, &total_write);
    Serial.begin(get_settings()->baudrate);
    if (handLen != hexLen) {
        String response("Upload Error!");
        if (handLen < 0) {
            response += " can't enter bootloader mode!";
        } else {
            response += " at Hex File(byte):";
            response += handLen;
            response += " total:";
            response += hexLen;
            response += ",";
            response += total_write;
            response += " bytes have been written";
        }
        stringResponse(client[0], 500, response);
    } else {
        String response("Upload done! Flash used:");
        response += total_write;
        response += " bytes";
        stringResponse(client[0], 200, response);
    }
}


static void netstat_json(WiFiClient* wc) {
    SettingsStruct *s = get_settings();
    wc->setTimeout(10000);
    int dlen = 
        (strlen(s->apssid) -2) +
        (strlen(s->ssid) -2) +
        (strlen(s->appswd) -2);

    const char* json = R"=====({"ap": {"ssid": "%s", "password": "%s"}, "client": {"ssid": "%s", "password":""}, "status": {"available": [%s], "ip": "%s", "subnet": "%s", "gateway": "%s", "dns": "%s"}})=====";

    int n = WiFi.scanNetworks();
        
    String available = "";
    for (int i = 0; i < n; ++i) {
      String ssid = WiFi.SSID(i);
      ssid.replace('"','.');

        available += "\"" + ssid + "\"";
        if (i < n-1)
            available += ",";
    }
    dlen += available.length() - 2;

    String localip = WiFi.localIP().toString();
    dlen += localip.length() - 2;

    String subnet = WiFi.subnetMask().toString();
    dlen += subnet.length() - 2;

    String gateway = WiFi.gatewayIP().toString();
    dlen += gateway.length() - 2;

    String dns = WiFi.dnsIP().toString();
    dlen += dns.length() - 2;

    wc->printf(head_frm, 200, ctJson, strlen(json) + dlen);
    wc->printf(json, s->apssid, s->appswd, s->ssid, available.c_str(),
                localip.c_str(), subnet.c_str(), gateway.c_str(), dns.c_str());
}


static void setssid(byte isAP, WiFiClient* wc) {
    SettingsStruct *s = get_settings();
    String line;
    do {
        line = wc->readStringUntil('\n');
    } while (line != String("\r"));


    char tssid[32];
    char tpswd[32];
    memset(tssid, 0, sizeof(tssid));
    memset(tpswd, 0, sizeof(tpswd));

    line = wc->readString();

    unsigned int n = 0;
    while (n < line.length()){
        String name = get_var_name(&line, &n);
        String val =  get_var_value(&line, &n);
        if (name == "ssid") {
            strcpy(tssid, val.c_str());
        }
        if (name == "pswd") {
            strcpy(tpswd, val.c_str());
        }
    }
    
    fileResponse((*wc), ctHtml, res_redir_html);

    if (isAP) {
        strcpy(s->apssid, tssid);
        strcpy(s->appswd, tpswd);
        change_ap();
    } else {
        strcpy(s->ssid, tssid);
        strcpy(s->pswd, tpswd);
        change_st();
    }
    update_mdns();
    save_eeprom();

    while (wc->availableForWrite()) wc->flush();
    ESP.reset();
}


void web_loop() {
    setup_next_client();
    
    for (int i = 1; i <= CLIENT_ARR_SIZE; i++) {
        WiFiClient client = clients[(lastIdx + i) % CLIENT_ARR_SIZE];
        if (!client || !client.connected() || !client.available()) 
            break;
        
        String s = client.readStringUntil('\n');
        s.toLowerCase();

        unsigned int tmp = 0;
        String method = get_method(&s, &tmp);
        String path = get_path(&s, &tmp);
        String query = get_query(&s, &tmp);

        // Technical actions
        handle_universal_queries(&query, &client);

        if (path == "pgm/sync") {
            String response("Esp8266 for UnoOTA, SYNC OK!");
            response += millis();
            stringResponse(client, (method == "post" ? 204 : 200), response);
        } else if (path == "pgm/reset" || path == "reset") {
            reset_slave();
            String response("Reset OK!");
            response += millis();
            stringResponse(client, 200, response);
        } else if (path == "pgm/upload") {
            handle_upload(&client);

        // HTML content related
        } else if (path == "favicon.ico") {
            binFileResponse(client, ctIco, res_favicon_ico);

        } else if (path == "common.js") {
            fileResponse(client, ctJs, res_common_js);
        } else if (path == "netstat.js") {
            fileResponse(client, ctJs, res_netstat_js);
        } else if (path == "pendant.js") {
            fileResponse(client, ctJs, res_pendant_js);
        } else if (path == "serial.js") {
            fileResponse(client, ctJs, res_serial_js);
        } else if (path == "serialmon.js") {
            fileResponse(client, ctJs, res_serialmon_js);
        } else if (path == "ui.js") {
            fileResponse(client, ctJs, res_ui_js);

        } else if (path == "ui.css") {
            fileResponse(client, ctCss, res_ui_css);

        } else if (path == "netstat.html") {
            fileResponse(client, ctHtml, res_netstat_html);
        } else if (path == "pendant.html") {
            fileResponse(client, ctHtml, res_pendant_html);
        } else if (path == "redir.html") {
            fileResponse(client, ctHtml, res_redir_html);
        } else if ((path == "index") || (path == "") || (path == "serial.html")) {
            fileResponse(client, ctHtml, res_serial_html);

        // Configuration actions
        } else if ((path == "setap") && (method == "post")) {
            setssid(1, &client);
        } else if ((path == "setwf") && (method == "post")) {
            setssid(0, &client);
        } else if (path == "netstat") {
            netstat_json(&client);
        } else if (path == "getbaudrate") {
            String response(get_settings()->baudrate);
            client.printf(head_frm, 200, ctJson, response.length());
            client.print(response);

        // 404
        } else {
            client.printf(head_frm, 404, ctPlain, 0);
        }
        client.flush();
    }
}
