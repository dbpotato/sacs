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

#include "Client.h"
#include "JsonMsg.h"

class ModuleManager;
class Server;
class Connection;
class ProxyServer;

 /**
 * Handles communication with main SACS instance using specified port
 */
class Proxy : public ClientManager
            , public std::enable_shared_from_this<Proxy> {
public :
  /**
   * Class constructor
   */
  Proxy();

  /**
   * Starts initialization
   * \param connection connection object used for communication
   * \param mgr owner object
   * \param port port used by proxy server
   */
  void Init(std::shared_ptr<Connection> connection, std::shared_ptr<ModuleManager> mgr, int port);

  /**
   * Implements ClientManager interface
   */
  bool OnClientConnected(std::shared_ptr<Client> client, NetError err) override;

  /**
   * Implements ClientManager interface
   */
  void OnClientRead(std::shared_ptr<Client> client, std::shared_ptr<Message> msg) override;

  /**
   * Implements ClientManager interface
   */
  bool IsRaw() override;

  /**
   * Send message to proxy server in another SACS instance
   * \param msg string to send
   */
  void PushEventSourceMsg(const std::string& msg);

  /**
   * Pass message with propery update to local ProxyServer
   * \param json object with json msg
   */
  void HandlePropertyUpdate(JsonMsg& json);

protected :
  std::shared_ptr<Connection> _connection; ///< connection used for communication
  std::shared_ptr<ModuleManager> _module_mgr; ///< owner instance
  std::shared_ptr<Client> _client; ///< network client - null if _server is used
  std::shared_ptr<ProxyServer> _server; ///< server instance - null if _client is used
  bool _initialized; ///< init flag - false until first OnClientConnected call
  int _port; ///< port used by ProxyServer
};
