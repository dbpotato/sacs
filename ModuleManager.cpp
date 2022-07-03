/*
Copyright (c) 2020 - 2022 Adam Kaniewski

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "ModuleManager.h"
#include "Connection.h"
#include "ServerImpl.h"
#include "Proxy.h"
#include "Logger.h"
#include "WebsocketServer.h"

#include <sstream>

std::shared_ptr<ModuleManager> ModuleManager::Create(int port, std::shared_ptr<Connection> connection) {
  std::shared_ptr<ModuleManager> sacs;
  sacs.reset(new ModuleManager(port, connection));
  sacs->Init();
  return sacs;
}

ModuleManager::ModuleManager(int port, std::shared_ptr<Connection> connection)
    : _port(port)
    , _connection(connection)
    , _id_counter(0)
    , _initialized(false) {
  if(!_connection)
    _connection = Connection::CreateBasic();
}

void ModuleManager::Init() {
  _proxy = std::make_shared<Proxy>();
  _proxy->Init(_connection, shared_from_this(), _port - 1);
}

void ModuleManager::OnProxyInitialized(bool host_mode) {
  log()->info("Running in Host Mode : {}", host_mode); 
  if(host_mode) {
    _ws_server = std::make_shared<WebsocketServer>();
    _server_impl = std::make_shared<ServerImpl>(shared_from_this());

    bool res = _ws_server->Init(_connection, _server_impl, _server_impl, _port); //TODO error handle
    log()->info("Created Http Server at port : {} , success : {}", _port, res);
  }

  _initialized = true;

  for(auto& msg: _msg_queue) {
    if(_server_impl) {
      _server_impl->PushMsgForJS(msg);
    }
    else {
      _proxy->PushMsgForJS(msg);
    }
  }
  _msg_queue.clear();
}

void ModuleManager::HandleMessage(const std::string& msg) {
  if(!_initialized) {
    _msg_queue.push_back(msg);
    return;
  }
  else if(_server_impl) {
    _server_impl->PushMsgForJS(msg);
  }
  else {
    _proxy->PushMsgForJS(msg);
  }
}

int ModuleManager::RegisterModule(const std::string& name,
                                  const std::vector<Module::Property>& properties,
                                  void(callback)(int, int, const int*, const char* const*)) {
  auto module = std::make_shared<Module>(name, properties, callback);
  return RegisterModule(module);
}

int ModuleManager::RegisterModule(std::shared_ptr<Module> module) {
  AddModule(module);
  HandleMessage(JsonMsg::CreateModuleAdded(module));
  return module->_id;
}

void ModuleManager::UnregisterModule(int module_id) {
  RemoveModule(module_id);
  HandleMessage(JsonMsg::CreateModuleRemoved(module_id));
}

void ModuleManager::UpdateProperties(int module_id, const std::vector<std::pair<int, std::string> >& properties) {
  SaveProperties(module_id, properties);
  HandleMessage(JsonMsg::CreatePropertyUpdated(module_id, properties));
}

void ModuleManager::UpdateProperties(int module_id, JsonMsg& json) {
  SaveProperties(module_id, json);
  HandleMessage(json.ToString());
}

void ModuleManager::SaveProperties(int module_id, JsonMsg& json) {
  std::vector<std::pair<int, std::string>> properties;
  json.GetPropertiesList(properties);
  SaveProperties(module_id, properties);
}

void ModuleManager::SaveProperties(int module_id, const std::vector<std::pair<int, std::string> >& properties) {
  auto module = GetModuleById(module_id);

  for(size_t i = 0; i < properties.size(); ++i) {
    int id = properties.at(i).first;
    const std::string& value = properties.at(i).second;
    module->SetPropertyValue(id, value);
  }
}

std::string ModuleManager::ProcessJSRequest(const std::string& str) { 
  JsonMsg json;
  if(!json.Parse(str))
    return JsonMsg::Empty();

  switch(json.GetType()) {
    case JsonMsg::Type::REQ_LOAD:
      return JsonMsg::CreateModuleList(_modules);
    case JsonMsg::Type::REQ_APPLY:
      PassPropertyUpdateToModule(json);
      break;
    default:
      break;
  }
  return JsonMsg::Empty();
}

void ModuleManager::PassPropertyUpdateToModule(JsonMsg& json) {
  int module_id = json.FindId();
  auto module = GetModuleById(module_id);

  std::vector<std::pair<int, std::string>> properties;
  json.GetPropertiesList(properties);

  if(!properties.size())
    return;

  SaveProperties(module_id,  properties);

  if(!module->_callback) {
    _proxy->HandlePropertyUpdate(json);
    return;
  }

  int* prop_ids = new int[properties.size()];
  char** prop_vals = new char*[properties.size()];

  for(size_t i = 0; i < properties.size(); ++i) {
    int id = properties.at(i).first;
    const std::string& value = properties.at(i).second;
    prop_ids[i] = id;

    char* str = nullptr;
    if(value.length()) {
      str = new char[value.length()+1];
      str[value.length()] = 0;
      std::memcpy(str, value.c_str(), value.length());
    }
    else{
      str = new char[1];
      str[0] = 0;
    }
    prop_vals[i] = str;
  }

  module->_callback(module_id, properties.size(), prop_ids, prop_vals);

  delete[] prop_ids;
  for(size_t i = 0; i < properties.size(); ++i) {
    delete[] prop_vals[i];
  }
  delete[] prop_vals;
}


void ModuleManager::RemoveModule(int module_id) {
  std::lock_guard<std::mutex> lock(_mutex);
  _modules.erase(module_id);
}

void ModuleManager::AddModule(std::shared_ptr<Module> module) {
  std::lock_guard<std::mutex> lock(_mutex);
  int mod_id = _id_counter++;
  module->SetId(mod_id);
  _modules.insert(std::make_pair(mod_id, module));
}

std::shared_ptr<Module> ModuleManager::GetModuleById(int module_id) {
  std::lock_guard<std::mutex> lock(_mutex);
  std::shared_ptr<Module> module;
  auto it = _modules.find(module_id);
  if(it != _modules.end()) {
    module = it->second;
  }
  return module;
}
