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

#include <map>
#include <memory>


class ModuleManager;
class Server;
class Connection;

class ProxyServer : public ClientManager
                  , public std::enable_shared_from_this<ProxyServer> {
public:
  void Init(std::shared_ptr<Connection> connection, std::shared_ptr<ModuleManager> mgr, int port);
  void HandlePropertyUpdate(JsonMsg& json);
  bool OnClientConnecting(std::shared_ptr<Client> client, NetError err) override;
  void OnClientRead(std::shared_ptr<Client> client, std::shared_ptr<Message> msg) override;
  void OnClientClosed(std::shared_ptr<Client> client) override;
private:
  void AddModule(int remote_id, int local_id, uint32_t client_id);
  void RemoveModule(int local_id);
  void RemoveClientsModules(uint32_t client_id);
  int LocalIdFromRemote(int remote_id, uint32_t client_id);
  std::pair<int, uint32_t> RemoteIdFromLocal(int local_id);
  std::shared_ptr<ModuleManager> _module_mgr;
  std::shared_ptr<Server> _server;
  std::map<int, std::pair<int, uint32_t> > _remote_modules; // local | remote | client
};
