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

#pragma once

#include "WebAppData.h"
#include "WebsocketServer.h"

#include <memory>
#include <vector>


class ModuleManager;


class ServerImpl : public HttpRequestHandler
                 , public WebsocketClientListener {

public:
  ServerImpl(std::weak_ptr<ModuleManager> module_mgr);

  void PushMsgForJS(const std::string& msg);

  void Handle(HttpRequest& request) override;
  bool OnWsClientConnected(std::shared_ptr<Client> client, const std::string& request_arg) override;
  void OnWsClientMessage(std::shared_ptr<Client> client, std::shared_ptr<WebsocketMessage> message) override;
  void OnWsClientClosed(std::shared_ptr<Client> client) override;

private:
  void PerpareHTTPGetResponse(HttpRequest& request);
  void AddEventListener(std::weak_ptr<Client> client);

  std::weak_ptr<ModuleManager> _module_mgr;
  std::vector<std::weak_ptr<Client>> _event_listeners;
  std::mutex _event_listeners_mutex;
  WebAppData _web_data;
};
