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

#include <unistd.h>
#include "SacsWrapper.h"


int g_tmp_mod_id = -1;

void callback(int module_id, int count, const int* property_no, const char* const* property_val) {
  if(count == 1 ) { // sanity check

    if(!(*property_no)) {
      if(g_tmp_mod_id != -1) {
        return;
      }

      std::vector<ModuleProperty> properties;
      properties.emplace_back(ModuleProperty::Type::TEXT_AREA_RO, "Just a simple", "Hello World");
      g_tmp_mod_id = SacsWrapper::Instance().RegisterModule("Temporary Module", properties, nullptr);
    } else {
      if(g_tmp_mod_id == -1) {
        return;
      }

      SacsWrapper::Instance().UnregisterModule(g_tmp_mod_id);
      g_tmp_mod_id = -1;
    }
  }
}

int main() {

 /* Register Module with two text fileds and callback
  */
  std::vector<ModuleProperty> properties;
  properties.emplace_back(ModuleProperty::Type::BUTTON, "Create");
  properties.emplace_back(ModuleProperty::Type::BUTTON, "Close");
  SacsWrapper::Instance().RegisterModule("Create / Close New Module", properties, callback);

  while(true)
    sleep(1);
}

