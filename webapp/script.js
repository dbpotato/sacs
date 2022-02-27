var event_source;
var websocket_obj;

function Enum() {
  for (var i in arguments) {
    this[arguments[i]] = parseInt(i);
  }
}

var Property = new Enum('BUTTON',
                        'BUTTON_SW',
                        'TEXT',
                        'TEXT_BT',
                        'TEXT_RO',
                        'TEXT_AREA',
                        'TEXT_AREA_BT',
                        'TEXT_AREA_RO',
                        'LOG',
                        'SCRIPT');

function dlog(str) {
  document.getElementById("dlog").innerHTML += "<br>" + str;
}

function clear() {
  document.module_list = [];
  document.selected_module = null;
  document.getElementById("top_bar").innerHTML = "";
  document.getElementById("main_content").innerHTML = "";
}

function applyModuleChanges(module_id) {
  for (var i = 0; i < document.module_list.length; i++) {
    if(document.module_list[i].id == module_id) {
      var req = {type: "apply", id: module_id, properties: []};
      for (var n = 0; n < document.module_list[i].properties.length; n++) {
        var prop = document.module_list[i].properties[n];
        switch(prop.type) {
          case Property.TEXT:
          case Property.TEXT_AREA:
            req.properties.push({id : n, value : prop.html.value});
            break;
          default:
            break;
        }
      }
      send_req(JSON.stringify(req));
      break;
    }
  }
}

function handlePopertyActivated(module_id, property_id, val) {
  var req = {type: "apply", id: module_id, properties: [{ id : property_id, value : val}]};
  send_req(JSON.stringify(req));
}

function handlePopertiesActivated(module_id, props) {
  var req = {type: "apply", id: module_id, properties: props};
  send_req(JSON.stringify(req));
}

function selectModule(module) {
  if(document.selected_module != null) {
    document.selected_module.html_bt.classList.remove("module_bt_selected");
  }
  document.selected_module = module;
  document.getElementById("main_content").innerHTML = "";
  document.getElementById("main_content").appendChild(module.html);
  module.html_bt.classList.add("module_bt_selected");
}

function addModuleList(list) {
  clear();
  for (var i = 0; i < list.length; i++) {
    crateModule(list[i]);
  }
  if(list.length) {
    selectModule(list[0]);
  }
}

function setPoropertyValue(prop, value) {
  switch(prop.type) {
    case Property.TEXT:
    case Property.TEXT_BT:
    case Property.TEXT_RO:
    case Property.TEXT_AREA:
    case Property.TEXT_AREA_BT:
    case Property.TEXT_AREA_RO:
      prop.html.value = value;
      break;
    case Property.LOG:
      if(value.length > 0)
        prop.html.value = value + "\n" + prop.html.value;
      else
        prop.html.value = "";
      break;
    case Property.BUTTON:
      prop.html.innerHTML = value
      break;
    case Property.SCRIPT:
      window[prop.name + "_update"](value);
      break;
    default:
      break;
  }
}

function setModulePoperties(json) {
  var module = null;
  for (var i = 0; i < document.module_list.length; i++) {
    if(document.module_list[i].id == json["id"]) {
      module = document.module_list[i];
      break;
    }
  }

  if(module == null)
    return;

  var prop_list = json["properties"];
  for (var i = 0; i < prop_list.length; i++) {
    setPoropertyValue(module.properties[prop_list[i]["id"]], prop_list[i]["value"]);
  }
}

function handleServerEvent(e) {
  var json = JSON.parse(e.data);
  if(json.type == "property_update") {
    setModulePoperties(json);
  }
  else if(json.type == "module_added") {
    crateModule(json.module);
  }
  else if(json.type == "module_removed") {
    deleteModule(json.id);
  }
  else if(json.type == "module_list") {
    addModuleList(json.modules);
  }
}

function clear_reconnect_timeout() {
  if(document.reconnect_timeout !== undefined) {
     clearTimeout(document.reconnect_timeout);
  }
}

function update_connection_counter(counter, bt){
  if(counter > 0){
    bt.innerHTML = counter;
    counter -= 1;
    document.reconnect_timeout = setTimeout(function() {
      update_connection_counter(counter, bt);
    }, 1000);
  }
  else {
    init();
  }
}

function showConnectionInfoErr() {
  clear();

  var reconnect_info = document.createElement("div");
  reconnect_info.setAttribute("id", "reconnect_info");

  var text = document.createElement("div");
  text.innerHTML = "Server Disconnected<br/>Reconnecting in :";

  var bt = document.createElement("div");
  bt.setAttribute("class", "base_bt");
  bt.setAttribute("id", "reconnect_bt");
  bt.innerHTML = 10;
  bt.addEventListener("click", function(){clear_reconnect_timeout();init();});

  reconnect_info.appendChild(text);
  reconnect_info.appendChild(bt);
  document.getElementById("main_content").appendChild(reconnect_info);

  update_connection_counter(10, bt)
}

function create_se() {
  event_source = new EventSource("events");
  event_source.onmessage = handleServerEvent;
  event_source.onerror = function(e) {
    event_source.close();
    showConnectionInfoErr();
  };
}

function create_ws() {
  var currentUrl = new URL(window.location.href);
  websocket_obj = new WebSocket("ws://" + currentUrl.host);
  websocket_obj.onopen = onWsCreated;
  websocket_obj.onmessage = handleServerEvent;
  websocket_obj.error = function(e) {
    websocket_obj.close();
  }
  websocket_obj.onclose = showConnectionInfoErr;
};

function init() {
  create_ws();
}

function onWsCreated() {
  send_req('{"type": "load"}');
}

function send_req(str) {
  websocket_obj.send(str);
}

