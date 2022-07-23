 class Module extends View {
  constructor(id, name) {
    super();
    this.id = id;
    this.name = name;
    this.properties = [];
    this.createNode();
  }

  onPropertyActivated(property) {
    document.webApp.messenger.send(MessageBuilder.createPropertyUpdated(this.id, property.id, property.getValue()));
  }

  onPropertiesActivated() {
    document.webApp.messenger.send(MessageBuilder.createPropertiesUpdated(this));
  }

  addProperty(propertyType, name, value) {
    var property = null;
    var id = this.properties.length;
    switch(propertyType) {
      case PropertyType.BUTTON :
        property = new ButtonProperty(this, id, name);
        break;
      case PropertyType.TEXT :
        property = new TextProperty(this, id, name, value);
        break;
      case PropertyType.TEXT_RO :
        property = new TextReadOnlyProperty(this, id, name, value);
        break;
      case PropertyType.TEXT_BT :
        property = new TextWithSendProperty(this, id, name, value);
        break;
      case PropertyType.BUTTON_SW :
        property = new ButtonSwitchProperty(this, id, name, value);
        break;
      case PropertyType.BUTTON_SW_IN :
        property = new ButtonSwitchIndependentProperty(this, id, name, value);
        break;
      case PropertyType.LOG :
        property = new LogProperty(this, id, name, value);
        break;
      case PropertyType.TEXT_AREA :
        property = new TexAreaProperty(this, id, name, value);
        break;
      case PropertyType.TEXT_AREA_RO :
        property = new TextAreaReadOnlyProperty(this, id, name, value);
        break;
      case PropertyType.TEXT_AREA_BT :
        property = new TexAreaWithSendProperty(this, id, name, value);
        break;
      case PropertyType.SCRIPT :
        property = new ScriptProperty(this, id, value);
        break;
      default :
        break;
    }

    if(property !== null) {
      this.properties.push(property);
      this.addObj(property.node);
    }
  }

  setPoropertyValue(id, value) {
    this.properties[id].setValue(value);
  }

  needsApplyButton() {
    for (var i = 0; i < this.properties.length; i++) {
      if(this.properties[i].isIndependent == false) {
        return true;
      }
    }
    return false;
  }

  maybeAddApplyButton() {
    if(this.needsApplyButton() == false) {
      return;
    }
    let thisObj = this;
    let buttonContainer = document.createElement("div");
    buttonContainer.setAttribute("class", "property_apply");

    let button = document.createElement("div");
    button.setAttribute("class", "base_bt apply_bt");
    button.innerHTML = "Apply";
    button.addEventListener("click", function(event) {
        thisObj.onPropertiesActivated();
    });

    buttonContainer.appendChild(button);
    this.addObj(buttonContainer);
  }
}
