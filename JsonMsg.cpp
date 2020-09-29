/*
Copyright (c) 2020 Adam Kaniewski

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

#include "Logger.h"
#include "JsonMsg.h"


const std::string EMPTY_JSON_STR = "{}";

JsonMsg::JsonMsg()
    : _is_valid (false)
    , _type(Type::UNKNOWN) {
}

bool JsonMsg::Parse(const std::string& str) {
  try{
    _json = nlohmann::json::parse(str);
    _is_valid = true;
    TryDetectType();
  }
  catch (nlohmann::json::parse_error& e) {
    log()->info("parse error : {}", e.what());
    _is_valid = false;
  }
  return _is_valid;
}

std::string JsonMsg::ToString() {
  if(!_is_valid)
    return {};

  return _json.dump();
}

void JsonMsg::TryDetectType() {
  if(!_is_valid)
    return;

  std::string type = ValueToString(_json, "type");

  if(!type.compare("module_added"))
    _type = Type::MODULE_ADD;
  else if(!type.compare("module_removed"))
    _type = Type::MODULE_DEL;
  else if(!type.compare("property_update"))
    _type = Type::PROPERTY_UPDATE;
  else if(!type.compare("load"))
    _type = Type::REQ_LOAD;
  else if(!type.compare("apply"))
    _type = Type::REQ_APPLY;
}

JsonMsg::Type JsonMsg::GetType() {
  return _type;
}

std::string JsonMsg::ValueToString(const nlohmann::json& json, const std::string& key) {
  std::string result;
  auto it_value = json.find(key);
  if(it_value == json.end())
    return result;

  auto value = *it_value;
  result = value.get<std::string>();
  return result;
}

int JsonMsg::ValueToInt(const nlohmann::json& json, const std::string& key) {
  int result = -1;
  auto it_value = json.find(key);
  if(it_value == json.end())
    return result;

  auto value = *it_value;
  result = value.get<int>();
  return result;
}

std::shared_ptr<Module> JsonMsg::ToModule() {
  std::shared_ptr<Module> result;

  auto it_module = _json.find("module");
  if(it_module == _json.end())
    return result;

  auto jmodule = *it_module;

  int id = ValueToInt(jmodule,"id");
  std::string name = ValueToString(jmodule,"name");
  std::vector<Module::Property> props_vec;

  auto& properties = jmodule["properties"];
  for(size_t i = 0; i < properties.size(); ++i) {
    int prop_type = ValueToInt(properties.at(i),"type");
    std::string prop_name = ValueToString(properties.at(i), "name");
    std::string prop_value = ValueToString(properties.at(i),"value");
    bool prop_read_only = (bool)ValueToInt(properties.at(i),"read_only");
    props_vec.push_back({prop_type, prop_name, prop_value, prop_read_only});
  }

  return std::make_shared<Module>(id, name, props_vec, nullptr);
}

int JsonMsg::FindId() {
  int result = -1;
  switch (_type) {
    case Type::MODULE_ADD:
      {
        auto it_module = _json.find("module");
        if(it_module != _json.end()) {
          auto jmodule = *it_module;
          result= ValueToInt(jmodule,"id");
        }
      }
      break;
    case Type::MODULE_DEL:
    case Type::PROPERTY_UPDATE:
    case Type::REQ_APPLY:
      result = ValueToInt(_json, "id");
      break;
    default:
      break;
  }
  return result;
}

void JsonMsg::SetId(int id) {
  _json["id"] = id;
}

nlohmann::json JsonMsg::ModuleToJson(std::shared_ptr<Module> module) {
  auto jmodule = nlohmann::json::object();
  jmodule["id"] = module->_id;
  jmodule["name"] = module->_name;
  auto prop_list = nlohmann::json::array();
  for(auto& property : module->_properties) {
    auto jprop = nlohmann::json::object();
    jprop["type"] = property._type;
    jprop["name"] = property._name;
    jprop["value"] = property._value;
    jprop["read_only"] = (int)property._read_only;
    prop_list.push_back(jprop);
  }
  jmodule["properties"] = prop_list;
  return jmodule;
}

void JsonMsg::GetPropertiesList(std::vector<std::pair<int, std::string> >& out_properties) {
  if((_type != REQ_APPLY) && (_type != PROPERTY_UPDATE))
    return;

  auto& properties = _json["properties"];
  if(!properties.size())
    return;

  for(size_t i = 0; i < properties.size(); ++i) {
    auto id = ValueToInt(properties.at(i), "id");
    auto value = ValueToString(properties.at(i), "value");
    out_properties.emplace_back(id, value);
  }
}

std::string JsonMsg::CreateModuleAdded(std::shared_ptr<Module> module) {
  auto json = nlohmann::json::object();
  json["type"] = "module_added";
  json["module"] = ModuleToJson(module);
  return json.dump();
}


std::string JsonMsg::CreateModuleRemoved(int module_id) {
  auto json = nlohmann::json::object();
  json["type"] = "module_removed";
  json["id"] = module_id;
  return json.dump();
}

std::string JsonMsg::CreatePropertyUpdated(int module_id, const std::vector<std::pair<int, std::string> >& properties) {
  auto json = nlohmann::json::object();
  json["type"] = "property_update";
  json["id"] = module_id;
  auto list = nlohmann::json::array();
  for(auto& prop : properties) {
    auto jprop = nlohmann::json::object();
    jprop["id"] = prop.first;
    jprop["value"] = prop.second;
    list.push_back(jprop);
  }
  json["properties"] = list;
  return json.dump();
}

std::string JsonMsg::CreateModuleList(std::map<int, std::shared_ptr<Module> >& modules) {
  auto json = nlohmann::json::object();
  json["type"] = "module_list";
  auto module_list = nlohmann::json::array();

  for(auto& it : modules) {
    auto jmodule = ModuleToJson(it.second);
    module_list.push_back(jmodule);
  }

  json["modules"] = module_list;
  return json.dump();
}

std::string JsonMsg::Empty() {
  return EMPTY_JSON_STR;
}
