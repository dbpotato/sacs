class WebApp {
  constructor() {
    this.messenger = null;
    this.moduleSelector = null;
    this.moduleContainer = null;
    this.reconnectInfo = null;
    this.moduleManager = null;
  }
  init() {
    this.messenger = new Messenger();
    this.moduleContainer = new ModuleContainer();
    this.moduleSelector = new ModuleSelector();
    this.reconnectInfo = new ReconnectInfo();
    this.moduleManager = new ModuleManager();

    document.body.appendChild(this.moduleSelector.node);
    document.body.appendChild(this.moduleContainer.node);
    document.body.appendChild(this.reconnectInfo.node);

    this.messenger.createWs();
  }

  clear() {
    this.moduleManager.clear();
    this.moduleSelector.clear();
  }

  onConnected() {
    this.reconnectInfo.disable();
    this.messenger.send(MessageBuilder.createLoadModules());
  }

  onDisconnected() {
    this.clear();
    this.reconnectInfo.enable();
  }

  onMessage(msg) {
    let moduleManager = this.moduleManager;
    var json = JSON.parse(msg);
    if(json.type == "property_update") {
      moduleManager.setModulePoperties(json);
    }
    else if(json.type == "module_added") {
      this.moduleManager.crateModule(json.module);
    }
    else if(json.type == "module_removed") {
      this.moduleManager.deleteModule(json.id);
    }
    else if(json.type == "module_list") {
      json.modules.forEach(function(moduleJson) {
        moduleManager.crateModule(moduleJson);
      });
    }
  }

  reconnect() {
    this.messenger.createWs();
  }
};
