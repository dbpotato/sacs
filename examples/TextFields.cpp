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

#include <unistd.h>
#include "SacsWrapper.h"
#include "Logger.h"

void callback(int module_id, int count, const int* property_no, const char* const* property_val) {
  if(count == 1 && ((*property_no == 0) || (*property_no == 2))) { // sanity check
    std::vector<std::pair<int, std::string> > properties;
    properties.emplace_back(1, property_val[0]);
    SacsWrapper::Instance().UpdateProperties(module_id, properties);
  }
}

int main() {

 /* Register Module with two text fileds and callback
  */
  std::vector<ModuleProperty> properties;
  properties.emplace_back(ModuleProperty::Type::TEXT, "New Value", "");
  properties.emplace_back(ModuleProperty::Type::TEXT_RO, "Current Value", "Default Value");
  properties.emplace_back(ModuleProperty::Type::TEXT_BT, "Independent Value", "Value");
  SacsWrapper::Instance().RegisterModule("Text Input", properties, callback);

  while(true)
    sleep(1);
}
