# Simple application control system

SACS is **C++ Linux library** which gives you a fast solution if you need to call parts of your code during application execution. Once integrated you will have two-way communication between your project and interface provided through an HTTP web page.
It's easiest to compare this to a remote debugger in a Chromium-based browser.

## Getting started
- checkout submodules : **git submodule update --init --recursive**
- build with **cmake**
- put compiled **libsacs&period;so** with reset of your project's libraries
- include **SacsWrapper.h** in your source code

## Structure

### Web Interface
Once initialized *Sacs* starts HTTP server on 8079 port. All needed web app resources are located in **libsacs&period;so**
Connection to open internet is not needed.
To begin just create *SacsWrapper* Instance:
 ```c++
  #include <unistd.h>
  #include "SacsWrapper.h"

  int main() {
    SacsWrapper::Instance();
    while(true) {
      sleep(1);
    }
  }
```
... and open http://localhost:8079 in web browser.

### Module
*Module* is a list of *properties* under a common name. It is visible in the *web interface* as a single *tab*.
Changes in *Module* properties execute notification callback in your application.
Once registered a module is identified by a unique id.

 ```c++
  #include <unistd.h>
  #include <vector>
  #include "SacsWrapper.h"

  void callback(int module_id, int count, const int* property_no, const char* const* property_val) {
  }

  int main() {
    std::vector<ModuleProperty> properties;
    int module_id = SacsWrapper::Instance().RegisterModule("Empty Module", properties, callback);
    while(true)
      sleep(1);
  }
```

When a module is no longer needed (for example an object which was using it was destroyed) it can be removed with :

 ```c++
 SacsWrapper::Instance().UnregisterModule(module_id);
```

*Module* can have its own "Apply" button if it has at least one *Property* which is **not independent**

### Property

It's your base element that will share a state between your application and the *web interface*.
Changes made in the web interface will be reported through *Module* callback.
Updates made in your application will be visible in the web interface.
Property is defined by its type, name and default value :
```c++
...
  ModuleProperty property(ModuleProperty::Type::TEXT, "Label", "Value");
...
```

#### *Property* id:
It is always an index of property in the vector passed during *Module* registration.

#### *Property* types :
|              | HTML Representation       | Is Read Only | Is Independent
|--------------|---------------------------|--------------|---------------|
| BUTTON       | div button                | Yes          | Yes
| BUTTON_SW    | input / checkbox          | No           | No
| BUTTON_SW_IN | input / checkbox          | No           | Yes
| TEXT         | input / text              | No           | No
| TEXT_BT      | input / text + div button | No           | Yes
| TEXT_RO      | input / text              | Yes          | Yes
| TEXT_AREA    | textarea                  | No           | No
| TEXT_AREA_BT | textarea + div button     | No           | Yes
| TEXT_AREA_RO | textarea                  | Yes          | Yes
| LOG          | textarea                  | Yes          | Yes
| SCRIPT       | ?                         | ?            | Yes


*Script* type allows defining your own property which will be injected into the *web interface* -
 see **examples/script.cpp** for more info.

#### *Read Only* Properties:

Those will not allow change of their value in *web interface*. Good for fields that have only a purpose for
displaying information. For example - current status, logs, etc.
Their value is modified only on application side.

#### *Independent* vs *Not Independent* Properties:

There will be cases when you will want to report all values in one single callback,
for example - let's say you want to add a new person to a database :

```c++
...
  void callback(int module_id, int count, const int* property_no, const char* const* property_val) {
    std::string name = property_val[0];
    std::string surname = property_val[1];
    int age = (int)strtol(property_val[2], nullptr, 0);
    add_person(name, surname, age);
  }
...
  std::vector<ModuleProperty> properties;
  properties.emplace_back(ModuleProperty::Type::TEXT, "Name", "");
  properties.emplace_back(ModuleProperty::Type::TEXT, "Surname", "");
  properties.emplace_back(ModuleProperty::Type::TEXT, "Age", "");
  int module_id = SacsWrapper::Instance().RegisterModule("Add Person", properties, callback);
...
```
Here your callback will always have 3 values. Order is the same as declared when registering a module.

In other cases you might just want to know that some button was pressed :
```c++
...
  void callback(int module_id, int count, const int* property_no, const char* const* property_val) {
    if(property_no[0] == 0) {
      clear_db();
    } else {
      reset_db();
    }
  }
...
  std::vector<ModuleProperty> properties;
  properties.emplace_back(ModuleProperty::Type::BUTTON, "Clear Database", "");
  properties.emplace_back(ModuleProperty::Type::BUTTON, "Reset Database", "");
  int module_id = SacsWrapper::Instance().RegisterModule("Database Ctrl", properties, callback);
...
```
You can mix *independent* and *not independent* in single module, just remember to handle them correctly in module callback.
### Updating properties
You can change properties values in web interface using *SacsWrapper::UpdateProperties*
```c++
  std::vector<std::pair<int, std::string> > properties;
  properties.emplace_back(0, str_stream.str());
  SacsWrapper::Instance().UpdateProperties(module_id, properties);
```
Pairs in properties vector represents **property id** and **new value**

### Internal Proxy System
Its purpose is to avoid a situation when different system processes with *SACS* implementation try to start HTTP Server on the same default port. When the first *SACS* instance is initialized, it listens on **default port - 1** for connections from other instances. All *Modules* registered in all *SACS* instances will be visible on the same *web interface* page as long as those use the same **default port**.
There is no *web server host* migration so make sure that the first started application/process with *SACS* implementation is the one with the longest lifetime.


## Additional Information

### Default port change
Modify SACS_PORT in ***wrappers/SacsWrapper.h***
Notice that chosen port and port with the previous value must be usable on your device.

### Thread Safety
**SacsWrapper** methods are thread-safe, but keep in mind that *Module* callback will be executed on a separate thread.

### Integration for Android
Please check ***wrappers/android/README.md*** for more information.


# License

MIT

