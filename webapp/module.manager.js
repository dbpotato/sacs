class ModuleManager {
  constructor() {
    this.modules = new Map();
  }

  getModuleById(id) {
    return this.modules.get(id);
  }

  crateModule(moduleJson) {
    let module = new Module(moduleJson.id, moduleJson.name);
    moduleJson.properties.forEach(properyJson => {
      module.addProperty(PropertyType.fromId(properyJson.type), properyJson.name, properyJson.value);
    });
    module.maybeAddApplyButton();
    this.modules.set(module.id, module);
    document.webApp.moduleSelector.onModuleAdded(module);
  }

  deleteModule(moduleId) {
    let module = this.getModuleById(moduleId);
    document.webApp.moduleSelector.onModuleRemoved(module);
    this.modules.delete(module.id);
  }

  setModulePoperties(propertiesJson) {
    let module = this.getModuleById(propertiesJson.id);
    if(module == null) {
      return;
    }

    propertiesJson.properties.forEach(propery => {
      module.setPoropertyValue(propery.id, propery.value);
    });
  }

  clear() {
    this.modules.forEach((module, id) => {
      module.deleteNode();
    });
    this.modules.clear();
  }
}

