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

#include <memory>
#include <map>


class ModuleManager;
class Server;
class Message;
class Connection;

class Request {
public:
  Request();
  enum Type { UNKNOWN, FILE, EVENTS, XHR };
  Type _type;
  std::string _value;
  size_t _size;
  std::shared_ptr<unsigned char> _data;
};

class HttpServer : public ClientManager
                 , public std::enable_shared_from_this<HttpServer>  {

public:
  HttpServer();
  bool Init(std::shared_ptr<Connection> connection,
            std::shared_ptr<ModuleManager> module_mgr,
            int port);

  bool OnClientConnected(std::shared_ptr<Client> client, NetError err) override;
  void OnClientRead(std::shared_ptr<Client> client, std::shared_ptr<Message> msg) override;
  bool IsRaw() override;

  void PushEventSourceMsg(const std::string& msg);

private:
  void ProcessRequest(std::shared_ptr<Client> client, std::shared_ptr<Message> msg);
  void ParseRequest(Request& req, const std::string& str);

  void PerpareFileResponse(Request& req);
  void PrepareEventResponse(std::shared_ptr<Client> client, Request& req);
  void PrepareXhrResponse(Request& req);
  void PrepareErrorResponse(Request& req);

  void AddEventListener(std::shared_ptr<Client> client);
  void NotifyEventListeners(const std::string& msg);

  std::shared_ptr<Server> _server;
  std::shared_ptr<ModuleManager> _module_mgr;
  std::map<int, std::weak_ptr<Client>> _event_listeners;
  std::mutex _event_listeners_mutex;
};
