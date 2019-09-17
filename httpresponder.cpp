#include "httpresponder.h"

HTTPResponder::HTTPResponder(String &BridgeID, unsigned int HTTPPort) {
    this->HTTPPort = HTTPPort;
    this->BridgeID = BridgeID;
    this->BridgeUID = "2f402f80-da50-11e1-9b23-" + this->BridgeID;
}

String HTTPResponder::handleDescription(ESP8266WebServer &HTTP) {
  String result = 
    "<?xml version=\"1.0\" ?>"
    "<root xmlns=\"urn:schemas-upnp-org:device-1-0\">"
        "<specVersion><major>1</major><minor>0</minor></specVersion>"
        "<URLBase>http://" + WiFi.localIP().toString() + ":" + String(this->HTTPPort, DEC) + "</URLBase>"
        "<device>"
            "<deviceType>urn:schemas-upnp-org:device:Basic:1</deviceType>"
            "<friendlyName>Philips hue (" + this->BridgeID + ")</friendlyName>"
            "<manufacturer>Royal Philips Electronics</manufacturer>"
            "<manufacturerURL>http://www.philips.com</manufacturerURL>"
            "<modelDescription>Philips hue Personal Wireless Lighting</modelDescription>"
            "<modelName>Philips hue bridge 2012</modelName>"
            "<modelNumber>929000226503</modelNumber>"
            "<modelURL>http://www.meethue.com</modelURL>"
            "<serialNumber>" + this->BridgeID + "</serialNumber>"
            "<UDN>uuid:" + this->BridgeUID + "</UDN>"
            "<presentationURL>index.html</presentationURL>"
        "</device>"
    "</root>";
    #ifdef HTTP_DEBUG
    Serial.printf("%s requesting device description... answering %s\n", HTTP.client().remoteIP().toString().c_str(), result.c_str());
    #endif
    return result;
}

String HTTPResponder::handleApi(ESP8266WebServer &HTTP, bool canpair) {
  String result;
	if (HTTP.arg(0).indexOf("devicetype") > 0 && canpair) {
    result = "[ { \"success\" : { \"username\" : \"" + this->BridgeUID + "\" } } ]";
  } else {
    result = "[ { \"error\" : { \"type\" : 101, \"address\" : \"\", \"description\" : \"link button not pressed\" } } ]";
  }
  #ifdef HTTP_DEBUG
    Serial.printf("%s requesting client registration... answering %s\n", HTTP.client().remoteIP().toString().c_str(), result.c_str());
  #endif
  return result;
}

String HTTPResponder::handleLights(ESP8266WebServer &HTTP, std::vector<HueLight> &Lights) {
  String result = "{ ";
  for (unsigned i = 0; i < Lights.size(); i++) {
    result += "\"" + String(i, DEC) + "\" : " + this->lightJson(Lights[i]);
    if (i!=Lights.size() - 1) result += ", "; else result += " }";
  }
  #ifdef HTTP_DEBUG
    Serial.printf("%s requesting light list... answering %s\n", HTTP.client().remoteIP().toString().c_str(), result.c_str());
  #endif
  return result;
}

String HTTPResponder::handleLight(unsigned char id, ESP8266WebServer &HTTP, std::vector<HueLight> &Lights) {
  String result = "[ ]";
  if (HTTP.method() == HTTP_GET) {
    result = this->lightJson( Lights[id]);
    #ifdef HTTP_DEBUG
      Serial.printf("%s requesting light %u info... answering %s\n", HTTP.client().remoteIP().toString().c_str(), id, result.c_str());
    #endif
  } else if (HTTP.method() == HTTP_PUT) {
    String values = HTTP.arg(0);
    if (values.indexOf("\"on\"") != -1) {
      String value = values.substring(values.indexOf(":", values.indexOf("\"on\"")) + 1, values.indexOf("}", values.indexOf("\"on\"")));
      value.trim();
      Lights[id].setState(value == "true");
      result = " [ { \"success\" : { \"/lights/" + String(id, DEC) + "/state/on\" : " + value + " } } ]";
    } else if (values.indexOf("bri") != -1) {
      String value = values.substring(values.indexOf(":", values.indexOf("\"bri\"")) + 1, values.indexOf("}", values.indexOf("\"bri\"")));
      Lights[id].setBrightness(value.toInt());
      result = "[ { \"success\" : { \"/lights/" + String(id, DEC) + "/state/bri\" : " + value + " } } ]";
    } 
    #ifdef HTTP_DEBUG
      Serial.printf("%s setting values of light %u ... answering %s\n", HTTP.client().remoteIP().toString().c_str(), id, result.c_str());
    #endif
  }
  return result;
}

String HTTPResponder::handleControl(ESP8266WebServer &HTTP, std::vector<HueLight> &Lights) {
  String result = 
  "<html>"
  "<head>"
    "<style>"
      "html,body{"
        "height:100%;"
        "padding:0px;"
        "margin:0px;"
      "}"
      "div{"
        "height:100%;"
        "position:relative;"
      "}"
      "div p{"
        "margin:0px;"
        "text-align:center;"
        "position:absolute;"
        "top:50%;"
        "left:50%;"
        "transform:translate(-50%,-50%);"
      "}"
      "input {"
        "font-size: 48px;"
        "padding:20px;"
        "margin:15px;"
        "border-radius:15px;"
        "border:0px;"
        "color:#ffffff;"
      "}"
      ".on {"
        "background-color: #009900;"
      "}"
      ".off {"
        "background-color: #990000;"
      "}"
      ".on:hover {"
        "background-color: #00AA00;"
      "}"
      ".off:hover {"
        "background-color: #AA0000;"
      "}"
    "</style>"
  "</head>"
  "<body>"
    "<div>"
      "<p id=\"items\">"
      "</p>"
    "</div>"
    "<script>"
      "var xhttp = new XMLHttpRequest();"
      "items = [ITEMS];"
      "SCRIPTS"
      "function update(id) {"
          "xhttp.open('GET', 'APIURL' + id, false);"
          "xhttp.send();"
          "currentbutton = document.getElementById('button' + id);"
          "json = JSON.parse(xhttp.responseText);"
          "tag = json['state']['on'] ? 'off' : 'on';"
          "if (!currentbutton) {"
            "body = document.getElementById('items');"
            "body.innerHTML = body.innerHTML + '<input id=\"button' + id + '\" type=\"button\" class=\"' + tag + '\" value=\"Turn ' + items[id] + ' ' + tag + '\" onclick=\"buttonClick(' + id + ')\"><br/>';"
          "} else {"
            "currentbutton.className = tag;"
            "currentbutton.value = 'Turn ' + items[id] + ' ' + tag;"
          "}"
        "}"

      "function buttonClick(id) {"
        "currentbutton = document.getElementById('button' + id);"
        "xhttp.open('PUT', 'APIURL' + id + '/state', false);"
        "xhttp.send('{ \"on\" : ' + (currentbutton.className == 'on' ? 'true' : 'false') + ' }');"
        "json = JSON.parse(xhttp.responseText);"
        "tag = json[0]['success']['/lights/' + id + '/state/on'] ? 'off' : 'on';"
        "currentbutton.className = tag;"
        "currentbutton.value = 'Turn ' + items[id] + ' ' + tag;"
      "}"
    "</script>"
  "</body>"
"</html>";
  String lights, scripts, api_uri;
  for (unsigned i = 0; i < Lights.size(); i++) {
    scripts += "function update" + String(i, DEC) + "() { update(" +  String(i, DEC) +"); setTimeout(update" + String(i, DEC) + ", 1000); } update" + String(i, DEC) + "();";
    lights += "\"" + Lights[i].getName() + "\"";
    if (i!=Lights.size() - 1) lights += ", ";
  }
  result.replace("ITEMS", lights);
  result.replace("SCRIPTS", scripts);
  result.replace("APIURL", "/api/" + this->BridgeUID + "/lights/");
  return result;
}

bool HTTPResponder::handle(ESP8266WebServer &HTTP, bool canpair, std::vector<HueLight> &Lights) {
  if (HTTP.uri() == "/control" && HTTP.method() == HTTP_GET) {
    HTTP.send(200, "text/html", this->handleControl(HTTP, Lights));
    return true;
  } else  if (HTTP.uri() == "/description.xml" && HTTP.method() == HTTP_GET) {
    HTTP.send(200, "text/xml", this->handleDescription(HTTP));
    return true;
  }
  if (HTTP.uri() == "/api" && HTTP.method() == HTTP_POST) {
    HTTP.send(200, "application/json", this->handleApi(HTTP, canpair));
    return true;
  }
  if (HTTP.uri().indexOf(this->BridgeUID) != -1) {
    String uri = HTTP.uri();
    uri.replace("/api/" + this->BridgeUID, "");
    if (uri == "/lights" && HTTP.method() == HTTP_GET) {
      HTTP.send(200, "application/json", this->handleLights(HTTP, Lights));
      return true;
    } else {
      if (uri.indexOf("/lights/") != -1) {
        uri.replace("/lights/", "");
        if (uri.indexOf("/state") != -1) uri.replace("/state", "");
        unsigned char id = uri.toInt();
        HTTP.send(200, "application/json", this->handleLight(id, HTTP, Lights));
        return true;
      }
    }
  }
  return false;
}

String HTTPResponder::lightJson(HueLight &Light) {
  return 
     "{ \"type\" : \"Extended color light\", "
       "\"name\" : \"" + Light.getName() + "\", "
       "\"uniqueid\" : \"" + Light.getUID() + "\", "
       "\"modelid\" : \"LCT007\", "
       "\"state\" : { "
         "\"on\" : " + String(Light.getState() ? "true" : "false") + ", " + String((Light.hasBrightness) ? "\"bri\" : " + String(Light.getBrightness(), DEC) + ", " : "") + ""
         "\"reachable\" : true "
       "}, "
       "\"capabilities\" : { "
         "\"certified\" : false, "
         "\"streaming\" : { "
           "\"renderer\" : true, "
           "\"proxy\" : false "
         "} "
      "}, "
      "\"swversion\" : \"5.105.0.21169\" "
    "}";
}
