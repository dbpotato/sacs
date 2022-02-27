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

#include "Proxy.h"
#include "ProxyServer.h"
#include "Connection.h"
#include "ModuleManager.h"
#include "Message.h"
#include "Server.h"
#include "Logger.h"


Proxy::Proxy()
    : _initialized(false)
    , _port(-1) {
}

void Proxy::Init(std::shared_ptr<Connection> connection,
                 std::shared_ptr<ModuleManager> mgr,
                 int port) {
  _connection = connection;
  _module_mgr = mgr;
  _port = port;

  _connection->CreateClient(_port, "127.0.0.1", shared_from_this());
}

bool Proxy::OnClientConnected(std::shared_ptr<Client> client, NetError err) {
  if(_initialized)
    return (err == NetError::OK);

  _initialized = true;

  if(err == NetError::OK) {
    _client = client;
  }
  else {
    log()->info("Create Proxy Server at port : {}", _port);
    _server = std::make_shared<ProxyServer>();
    _server->Init(_connection, _module_mgr, _port);
  }
  _module_mgr->OnProxyInitialized(_server != nullptr);

  return (err == NetError::OK);
}

void Proxy::OnClientRead(std::shared_ptr<Client> client, std::shared_ptr<Message> msg) {
  JsonMsg json;
  json.Parse(msg->ToString());
  _module_mgr->PassPropertyUpdateToModule(json);
}

void Proxy::PushMsgForJS(const std::string& msg) {
  _client->Send(std::make_shared<Message>(0, msg));
}

void Proxy::HandlePropertyUpdate(JsonMsg& json) {
  _server->HandlePropertyUpdate(json);
}

bool Proxy::IsRaw() {
  return false;
}
