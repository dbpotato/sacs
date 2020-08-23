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

#pragma once

#include "JsonMsg.h"

#include <memory>
#include <vector>
#include <map>
#include <atomic>


class Proxy;
class HttpServer;
class Connection;
class Module;

class ModuleManager : public std::enable_shared_from_this<ModuleManager> {
public:
  static std::shared_ptr<ModuleManager> Create(int port, std::shared_ptr<Connection> connection = {});
  void OnProxyInitialized(bool host_mode);
  std::string ProcessXhrRequest(const std::string& str);

  int RegisterModule(const std::string& name,
                     const std::vector<Module::Property>& properties,
                     void(callback)(int, int, const int*, const char* const*));

  int RegisterModule(std::shared_ptr<Module> module);

  void UnregisterModule(int module_id);

  void UpdateProperties(int module_id, const std::vector<std::pair<int, std::string> >& properties);
  void UpdateProperties(JsonMsg& json);

  void PassPropertyUpdateToModule(JsonMsg& json);

protected:
  ModuleManager(int port, std::shared_ptr<Connection> connection);
  void Init();
private:
  std::string ListModules();
  void HandleMessage(const std::string& msg);

  int _port;
  std::shared_ptr<Connection> _connection;
  std::shared_ptr<HttpServer> _http_server;
  std::shared_ptr<Proxy> _proxy;

  std::map<int, std::shared_ptr<Module> > _modules;
  std::vector<std::string> _msg_qeue;

  int _id_counter;

  std::atomic_bool _initalized;
};
