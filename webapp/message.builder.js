class MessageBuilder {
  static createLoadModules() {
    var req = {type: "load"};
    return JSON.stringify(req);
  }

  static createPropertyUpdated(moduleId, propertyId, propertyValue) {
    var req = {type: "apply", id: moduleId, properties: [{ id : propertyId, value : propertyValue}]};
    return JSON.stringify(req);
  }

  static createPropertiesUpdated(module) {
    let propertiesInfo = [];
    module.properties.forEach(property => {
      if(property.isIndependent == false) {
        propertiesInfo.push({id : property.id, value : property.getValue()});
      }
    });
    var req = {type: "apply", id: module.id, properties: propertiesInfo};
    return JSON.stringify(req);
  }
};

