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

/**
 * Magic happens here. This is the main class of the project.
 */
class ModuleManager : public std::enable_shared_from_this<ModuleManager> {
public:
  /**
  * Create and initialize new ModuleManager object
  * \param port listening port for HttpServer
  * \param connection connection instance used for Proxy / HttpServer
  * \return new instance
  */
  static std::shared_ptr<ModuleManager> Create(int port, std::shared_ptr<Connection> connection = {});

  /**
  * Called form Proxy
  * \param host_mode if true - this is an only instance on specified port and http server should be created.
  */
  void OnProxyInitialized(bool host_mode);

  /**
  * Called form Proxy
  * \param host_mode if true - this is an only instance on specified port and http server should be created.
  */
  std::string ProcessXhrRequest(const std::string& str);

  /**
  * Create a new module. Called by SacsLib.cpp
  * \param name module name
  * \param properties list of properties
  * \param callback called when properties are updated
  * \return id of created module
  */
  int RegisterModule(const std::string& name,
                     const std::vector<Module::Property>& properties,
                     void(callback)(int, int, const int*, const char* const*));

  /**
  * Adds a new module received by ProxyServer
  * \param module new module
  * \return id of registered module
  */
  int RegisterModule(std::shared_ptr<Module> module);

  /**
  * Removes module
  * \param module_id id of module which should be removed
  */
  void UnregisterModule(int module_id);

  /**
  * Updates module properties with new values. Called by SacsLib.cpp
  * \param module_id id of module which will be updated
  * \param properties list of properies : property_id | property_value
  */
  void UpdateProperties(int module_id, const std::vector<std::pair<int, std::string> >& properties);

  /**
  * Updates module properties with values received by ProxyServer
  * \param module_id id of module which will be updated
  * \param json object with received message
  */
  void UpdateProperties(int module_id, JsonMsg& json);

  /**
  * Handle propery update request messge recived form HttpServer or Proxy
  * \param module_id id of module which will be updated
  * \param json object with received message
  */
  void PassPropertyUpdateToModule(JsonMsg& json);

protected:
  /**
  * Class constructor
  * \param port listening port for http server
  * \param connection connection instance used for Proxy / HttpServer
  */
  ModuleManager(int port, std::shared_ptr<Connection> connection);

  /**
  * Initialize Proxy
  */
  void Init();

  /**
  * Save properties current values
  * \param module_id related module's id
  * \param json object with properties
  */
  void SaveProperties(int module_id, JsonMsg& json);

  /**
  * Save properties current values
  * \param module_id related module's id
  * \param properties list with properties : property_id | property_value
  */
  void SaveProperties(int module_id, const std::vector<std::pair<int, std::string> >& properties);

private:

  /** Pass msg to HttpSerer or Proxy
   * \param msg string with json message
   */
  void HandleMessage(const std::string& msg);

  int _port; ///< listening port for HttpServer
  std::shared_ptr<Connection> _connection; ///< used by Proxy and HttpServer
  std::shared_ptr<HttpServer> _http_server; ///< null if this works as proxy
  std::shared_ptr<Proxy> _proxy; ///< used for communicating with other Sacs instances

  std::map<int, std::shared_ptr<Module> > _modules; ///< registered modules
  std::vector<std::string> _msg_queue; ///< used for storing request before Proxy is initialized

  int _id_counter; ///< used for creating next module's id
  std::atomic_bool _initialized; ///< false before Proxy is initialized
};
