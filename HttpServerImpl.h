/*
Copyright (c) 2020 - 2021 Adam Kaniewski

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

#include "WebAppData.h"
#include "HttpServer.h"

#include <memory>
#include <vector>


class ModuleManager;


class HttpServerImpl : public HttpRequestHandler
                     , public std::enable_shared_from_this<HttpServerImpl>  {

public:
  bool Init(std::shared_ptr<Connection> connection,
            std::shared_ptr<ModuleManager> module_mgr,
            int port);

  void PushEventSourceMsg(const std::string& msg);

  void GetResource(HttpRequest& request) override;

private:
  void PerpareFileResponse(HttpRequest& request);
  void PrepareEventResponse(HttpRequest& request);
  void PrepareXhrResponse(HttpRequest& request);

  void AddEventListener(std::weak_ptr<Client> client);
  void NotifyEventListeners(const std::string& msg);

  std::shared_ptr<HttpServer> _server;
  std::shared_ptr<ModuleManager> _module_mgr;
  std::vector<std::weak_ptr<Client>> _event_listeners;
  std::mutex _event_listeners_mutex;
  WebAppData _web_data;
};
