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


#include "ModuleManager.h"

#include <memory>
#include <vector>


std::shared_ptr<ModuleManager> g_mgr;

extern "C" {

void init(int port) {
  if(!g_mgr) {
    g_mgr = ModuleManager::Create(port);
  }
}

int register_module(const char* module_name,
                    int properties_count,
                    int* propert_types,
                    char** properties_names,
                    char** properties_defaults,
                    void(callback)(int, int, const int*, const char* const*)) {
  if(!g_mgr)
    return -2;

  std::vector<Module::Property> vec_props;

  for(int i = 0; i < properties_count; ++i) {
    vec_props.emplace_back(*(propert_types + i), *(properties_names + i), *(properties_defaults + i));
  }

  return g_mgr->RegisterModule(module_name, vec_props, callback);
}

bool unregister_module(int module_id) {
  g_mgr->UnregisterModule(module_id);
  return true; //TODO
}

void update_properties(int module_id,
                       int properties_count,
                       int* property_ids,
                       char** properties_values) {
  std::vector<std::pair<int, std::string> > properties;

  for(int i = 0; i < properties_count; i++)
    properties.emplace_back(property_ids[i], properties_values[i]);

  g_mgr->UpdateProperties(module_id, properties);
}


} //extern "C"
