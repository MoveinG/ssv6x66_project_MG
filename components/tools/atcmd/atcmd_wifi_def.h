/**
 *
 * @page ATCMD
 *
 * @section ATCMD_wifi WIFI AT Command
 * 
 * @subsection ATCMD_wifi_1 Set DUT Role
 *
 * | command     | AT+DUT_START=<mode> |
 * |---------------|-------------------|
 * | param         | <mode>:0: DUT_NONE<br>1: DUT_STA<br>2: DUT_AP<br>3: DUT_CONCURRENT<br>4: DUT_TWOSTA<br>        |
 * | return value  | +OK               |
 * | example       | AT+DUT_START=1 |
 *
 * @subsection ATCMD_wifi_2 Scan AP 
 *
 * | command     | AT+SCAN=? |
 * |---------------|-------------------|
 * | param         | none         |
 * | return value  | <id> - <ssid>, <rssi>, <channel>, <mac>               |
 * | example       | AT+SCAN=?|
 * 
 *
 * @subsection ATCMD_wifi_3 Set WIFI AP Parameters
 *
 * | command     | AT+SET_WIFICONFIG=<mode>, <ssid>, <key> |
 * |---------------|-------------------|
 * | param         | <mode>: must to set 0: STA-Mode<br>ssid: ap ssid<br>key: ap password        |
 * | return value  | +OK               |
 * | example       | AT+SET_WIFICONFIG=0,P880,12345678 |
 *
 * @subsection ATCMD_wifi_4 Set IP Parameters
 *
 * | command     | AT+SET_IFCONFIG=<dhcp>, <ip>, <submask>, <gateway> |
 * |---------------|-------------------|
 * | param         | <dhcp>: 1:dhcp, 0:static ip<br><ip>: xxx,xxx,xxx,xxx (if dhcp = 0)<br><submask>: xxx,xxx,xxx,xxx (if dhcp = 0)<br><gateway>: xxx,xxx,xxx,xxx (if dhcp = 0)      |
 * | return value  | +OK               |
 * | example       | AT+SET_WIFICONFIG=1<br>AT+SET_WIFICONFIG=0,192.168.1.100,255.255.255.0,192.168.1.1 |
 *
 * @subsection ATCMD_wifi_5 Connect to WIFI AP
 *
 * | command     | AT+WIFICONNECT |
 * |---------------|-------------------|
 * | param         | none         |
 * | return value  | +OK               |
 * | example       | AT+WIFICONNECT |
 *
 * @subsection ATCMD_wifi_6 Disconnect from WIFI AP 
 *
 * | command     | AT+WIFIDISCONNECT |
 * |---------------|-------------------|
 * | param         | none         |
 * | return value  | +OK               |
 * | example       | AT+WIFIDISCONNECT |
 *
 * @subsection ATCMD_wifi_7 Get Networks Configuration
 *
 * | command     | AT+GET_CONFIG=? |
 * |---------------|-------------------|
 * | param         | none         |
 * | return value  | +GET_CONFIG=0,<ssid>,<key>,<key_len>,<ip>,<submask>,<gateway>             |
 * | example       | AT+GET_CONFIG=? |
 *
 * @subsection ATCMD_wifi_8 Get Networks Status
 *
 * | command     | AT+NETSTATUS=? |
 * |---------------|-------------------|
 * | param         | none         |
 * | return value  | +NETSTATUS=<connected>               |
 * | example       | AT+NETSTATUS=? |
 *
 * @subsection ATCMD_wifi_9 Show AP List
 *
 * | command     | AT+SHOWALLAP |
 * |---------------|-------------------|
 * | param         | none         |
 * | return value  | <ssid>, <id>, <rssi>, <mac>, <encryption type> |
 * | example       | AT+SHOWALLAP |
 *
 * @subsection ATCMD_wifi_10 Show Connected AP
 *
 * | command     | AT+SHOWCONNECTAP |
 * |---------------|-------------------|
 * | param         | none         |
 * | return value  | <ssid>, <id>, <rssi>, <mac>, <encryption type> |
 * | example       | AT+SHOWCONNECTAP |
 *
 *
 */
#ifndef ATCMD_WIFI_DEF_H__
#define ATCMD_WIFI_DEF_H__

#define ATCMD_DUT_START             "AT+DUT_START"
#define ATCMD_NETSCAN               "AT+SCAN"
#define ATCMD_SETWIFICONFIG	        "AT+SET_WIFICONFIG"
#define ATCMD_SETIFCONFIG           "AT+SET_IFCONFIG"
#define ATCMD_CONNECT               "AT+WIFICONNECT"
#define ATCMD_DISCONNECT            "AT+WIFIDISCONNECT"
#define ATCMD_CONNECT_ACTIVE            "AT+ACTIVECONNECT"

#define ATCMD_GETCONFIG             "AT+GET_CONFIG"
#define ATCMD_NETSTATUS             "AT+NETSTATUS"
#define ATCMD_SHOWALLAP             "AT+SHOWALLAP"
#define ATCMD_SHOWCONNECTAP         "AT+SHOWCONNECTAP"
#define ATCMD_SET_AUTO_CONNECT      "AT+SET_AUTO_CONNECT"

//STA2
#define ATCMD_SETWIFICONFIG2        "AT+SET_WIFICONFIG2"
#define ATCMD_SETIFCONFIG2          "AT+SET_IFCONFIG2"
#define ATCMD_CONNECT2              "AT+WIFICONNECT2"
#define ATCMD_NETSTATUS2            "AT+NETSTATUS2"
#define ATCMD_DISCONNECT2           "AT+WIFIDISCONNECT2"

//SOFT_AP TEST
#define ATCMD_AP                    ("AT+AP")
#define ATCMD_HKAP                  ("AT+HKAP")
#define ATCMD_SET_APCONFIG          ("AT+SET_APCONFIG")
#define ATCMD_GET_APCONFIG          ("AT+GET_APCONFIG")
#define ATCMD_GET_APMODE            ("AT+GET_APMODE")
#define ATCMD_AP_EXIT               ("AT+AP_EXIT")

#define ATCMD_PING                  "ping"
#define ATCMD_APSTATUS              "ap"
#define ATCMD_SOFTAP_STATUS         "softap"
#define ATCMD_WIFISTATUS            "wifi"

#define ATCMD_SET_COUNTRY_CODE "AT+SET_COUNTRY_CODE"

//Not implement
#define ATCMD_RECONNECT             "AT+WIFIRECONNECT"
#define ATCMD_NETSCAN_CUSTOM        "AT+SCAN_CUSTOM"
#define ATCMD_SEND                  "AT+SEND"
#define ATCMD_CLEAN_LISTS           "AT+CLEAN_LISTS"
#define ATCMD_MAC_HW_QUEUE          "q"
#define ATCMD_MAC_HW_MIB            "mib"
#define ATCMD_FIXRATE               "fixrate"
#define ATCMD_RC_MASK               "rc_mask"
#define ATCMD_MAC_DBG               "AT+MACDBG"
#endif
