/*******************************************************************************************
 * rrdtool example
 * (c)2020 Larry Bernstone
 * This example builds a couple sample databases (or uses existing files if uploaded).
 * It then sets up a webserver, which provides a sample webpage using javascriptRRD + flot
 * to display the data.  You can also access /status, which provides the latest data in
 * json format.  Internet access is necessary in order to sync network time.
 * Updates are entered into the database every minute through a ticker.
 ******************************************************************************************/ 
#include <FFat.h> // You must use FAT for rrdtool
#include <WiFi.h>
#include <rrd.h>
#include <Ticker.h>
#include <WebServer.h>

#define BUILD_TEST_DATA

const char* myssid      = "larryb";
const char* mypasswd    = "clownfish";
const char ntpSrv[]     = "pool.ntp.org";

const char* rrd_files[] = {"/ffat/rrd_0.rrd", "/ffat/rrd_1.rrd"}; // Must use full vfs path
const char* rrd_0 = "-s60 -b1600000000 /ffat/rrd_0.rrd DS:bytesin:COUNTER:180:0:100000 RRA:AVERAGE:0.5:1:60 RRA:AVERAGE:0.5:10:2016 RRA:AVERAGE:0.5:360:2924"; 
const int argc = 10;
const char* rrd_1[argc] =  {"rrd_create", //Argv[0] will be ignored
                      rrd_files[1],
                      "--step", "60", //seconds
                      "--start", "1600000000", // Sep 13, 2020
                      "DS:wave:GAUGE:120:0:100", // range is 0-100, expected every 2 mins
                      "RRA:AVERAGE:0.5:1:60",  // 1 hour @ 1 minute
                      "RRA:AVERAGE:0.5:10:2016", // 2 weeks @ 10 minutes
                      "RRA:AVERAGE:0.5:360:2924" }; // 2 years @ 6 hours

const char* indexHtml   = R"rrdJ(
<html>
  <script type="text/javascript" src="http://javascriptrrd.sourceforge.net/docs/javascriptrrd_v1.1.1/src/lib/javascriptrrd.wlibs.js"></script>
  <head><title>RRD Example on ESP32</title></head>
  <body>
    <h1 id="title">RRD Example on ESP32</h1>
    <table id="infotable" border=1>
        <tr><td colspan="21"><b>Javascript needed for this page to work</b></td></tr>
    </table>
    <div id="rrd_graph0"></div>
    <br>
    <div id="rrd_graph1"></div>
    <script type="text/javascript">
      document.getElementById("infotable").deleteRow(0);
      var graph_opts = {legend: { noColumns:4}, tooltipOpts:{content:"Value: %y.3"}};
      var ds_graph_opts0 = {'rx_data':{ label: 'Received Bytes', color: "#ff0000", 
                                  lines: { show: true, fill: false}}};
      var ds_graph_opts1 = {'wave':{ label: 'Wave', color: "#0000ff", 
                                  lines: { show: true, fill: true}}};
      flot_obj1=new rrdFlotAsync('rrd_graph0','files/rrd_0.rrd', null, graph_opts,ds_graph_opts0);
      flot_obj2=new rrdFlotAsync('rrd_graph1','files/rrd_1.rrd', null, graph_opts,ds_graph_opts1);
    </script>
  </body>
</html>
)rrdJ";

Ticker tkFillData;
WebServer server(80);

size_t lastStr(const char* rrdFile, String &returnStr, uint8_t ds=0) {
  char* argv[2] = {(char*)"a", (char*)rrdFile};
  time_t last_update;
  unsigned long ds_cnt;
  char **ds_namv, **last_ds;
  if (rrd_lastupdate(2, (char**)&argv, &last_update, &ds_cnt, &ds_namv, &last_ds) == 0) {
    returnStr = "{\"data_store\":\"" + String(ds_namv[ds]);
    returnStr += "\",\"last_update\":" + String(last_update);
    returnStr += ",\"value\":" + String(last_ds[ds]) + "}";
    for (int x=0; x<ds_cnt; x++) {
      free(ds_namv[x]);
      free(last_ds[x]);
    }
    free(ds_namv);
    free(last_ds);
    return returnStr.length();
  }
  return 0;
}

void handleStatus() {
  String rs0, rs1;
  lastStr(rrd_files[0], rs0);
  lastStr(rrd_files[1], rs1);
  String json = "{\"data stores\":[" + String(rs0) + "," + String(rs1) + "]}";
  server.sendHeader("cache-control", "max-age=60");
  server.send(200, "application/json", json);
}

void fakeData(uint8_t rrd_mask) {
  if (!rrd_mask) return;
  Serial.println("Generating fake data.  This will take ~10 minutes");
  uint32_t first_update = time(NULL) - 120000;
  uint16_t rrd_step = 60;
  uint32_t counter0 = 0;
  size_t counter1 = 0;
  for (uint32_t x=first_update; x<time(NULL); x+=rrd_step) {
    char rrd0_pt[24];
    counter0 += random(100000);
    snprintf(rrd0_pt, 23, "%10u:%lu", x, counter0);
    char* filler0[1] = {(char*)rrd0_pt};
    if (rrd_mask & 1) rrd_update_r(rrd_files[0], NULL, 1, (const char**)&filler0);
    char rrd1_pt[18];
    snprintf(rrd1_pt, 17, "%10u:%5.2f", x, sin(x/600.0*2*M_PI)*50+50);
    char* filler1[1] = {(char*)rrd1_pt};
    if (rrd_mask & 2) rrd_update_r(rrd_files[1], NULL, 1, (const char**)&filler1);
    if (++counter1 % 50 == 0) log_v("count: %d",counter1);
  }
}

bool createRrds() {
  char junk_str[9] = "rrd_info";
  char* checker0[2] = {junk_str, (char*)rrd_files[0]};
#ifdef BUILD_TEST_DATA
  uint8_t build_data = 0;
#endif
  if (access(rrd_files[0], F_OK) == -1 || 
        rrd_info(2,(char**)checker0) == NULL) { // did not return any info
    unlink(rrd_files[0]);
    build_data += 1;
    if(rrd_create_str(rrd_0) != 0) {
      log_e("Unable to create rrd 0");
      return 1;
    }
  }
  char* checker1[2] = {junk_str, (char*)rrd_files[1]};
  if (access(rrd_files[1], F_OK) == -1 || 
        rrd_info(2,(char**)checker1) == NULL) { // did not return any info
    unlink(rrd_files[1]);
    build_data += 2;
    if(rrd_create(argc, (char**)rrd_1) != 0) {
      log_e("Unable to create rrd 1");
      return 1;
    }
  }
#ifdef BUILD_TEST_DATA
  fakeData(build_data);  
#endif
  return 0;
}  

void fillData() {
  static uint32_t DS0 = 0;
  DS0 += random(100000);
  char upd[15];
  snprintf(upd,15,"N:%d",DS0);
  char *rrd_updater0[1] = {upd};
  if (rrd_update_r(rrd_files[0], NULL, 1, (const char**)&rrd_updater0)) {
    log_e("Unable to add data to %s", rrd_files[0]);
    return;
  }
  snprintf(upd,11,"N:%5.2f",sin(time(NULL)/600.0*2*M_PI)*50+50);
  char *rrd_updater1[1] = {upd};
  if (rrd_update_r(rrd_files[1], NULL, 1, (const char**)&rrd_updater1)) {
    log_e("Unable to add data to %s", rrd_files[1]);
    return;
  }
  log_i("Added data");
}

void setup() {
  Serial.begin(115200);

  if (!FFat.begin(true)) {
    Serial.println("Unable to mount FFat");
    return;
  }

  WiFi.begin(myssid, mypasswd);
  WiFi.waitForConnectResult();
  delay(250);

  configTime(0, 0, ntpSrv); // sync time to UTC
  struct tm now;
  if (!getLocalTime(&now, 120000)) { //attempt for 120 seconds
    Serial.println("Unable to sync with ntp server. Time must be synchronized for this example");
    return;
  }
  
  if (createRrds()) {
    Serial.println("Unable to create rrds");
    return;
  }
  
  fillData();
  tkFillData.attach(60, fillData);
  
  server.on("/", []() {server.send(200, "text/html", indexHtml);});
  server.on("/status", []() {handleStatus();});
  server.serveStatic("/files/rrd_0.rrd", FFat, "/rrd_0.rrd", "max-age=60");
  server.serveStatic("/files/rrd_1.rrd", FFat, "/rrd_1.rrd", "max-age=60");
  server.begin();
  Serial.print("Server ready at http://");
  Serial.println(WiFi.localIP());
}

void loop() {
  server.handleClient();
}
