class PropertyType {
  // Create new instances of the same class as static attributes
  static BUTTON = new PropertyType("BUTTON");
  static BUTTON_SW = new PropertyType("BUTTON_SW");
  static BUTTON_SW_IN = new PropertyType("BUTTON_SW_IN");
  static TEXT = new PropertyType("TEXT");
  static TEXT_BT = new PropertyType("TEXT_BT");
  static TEXT_RO = new PropertyType("TEXT_RO");
  static TEXT_AREA = new PropertyType("TEXT_AREA");
  static TEXT_AREA_BT = new PropertyType("TEXT_AREA_BT");
  static TEXT_AREA_RO = new PropertyType("TEXT_AREA_RO");
  static LOG = new PropertyType("LOG");
  static SCRIPT = new PropertyType("SCRIPT");

  static PropertyTypeArray = [
    PropertyType.BUTTON,
    PropertyType.BUTTON_SW,
    PropertyType.BUTTON_SW_IN,
    PropertyType.TEXT,
    PropertyType.TEXT_BT,
    PropertyType.TEXT_RO,
    PropertyType.TEXT_AREA,
    PropertyType.TEXT_AREA_BT,
    PropertyType.TEXT_AREA_RO,
    PropertyType.LOG,
    PropertyType.SCRIPT
  ];

  constructor(id, name) {
    this.id = id;
    this.name = name;
  }

  static fromId(id) {
    if(id >= 0 && id < PropertyType.PropertyTypeArray.length) {
      return PropertyType.PropertyTypeArray[id];
    }
    return null;
  }
}

class Property extends View {
  constructor(module, id, propertyType) {
    super();
    this.label = null;
    this.labelContainer = null;
    this.valueContainer = null;
    this.sendButton = null;
    this.module = module;
    this.id = id;
    this.type = propertyType;
    this.isIndependent = false;
  }

  createNode() {
    super.createNode();
    this.node.setAttribute("class", "property");

    this.valueContainer = document.createElement("div");
    this.valueContainer.setAttribute("class", "property_value vertical");
    this.node.appendChild(this.valueContainer);
  }

  addObj(obj) {
    this.valueContainer.appendChild(obj);
  }

  createLabel(labelText) {
    this.labelContainer = document.createElement("div");
    this.labelContainer.setAttribute("class", "property_name_container vertical");
    this.label = document.createElement("div");
    this.label.setAttribute("class", "property_name");
    this.label.innerHTML = labelText;

    this.labelContainer.appendChild(this.label);
    this.node.prepend(this.labelContainer);
  }

  createSendButton() {
    if(this.labelContainer == null) {
      return;
    }
    let thisObj = this;
    this.sendButton = document.createElement("div");
    this.sendButton.innerHTML = "Send";
    this.sendButton.addEventListener("click", function(event){thisObj.onActivated();});
    this.sendButton.setAttribute("class", "base_bt send_bt");
    this.labelContainer.appendChild(this.sendButton);
  }

  getValue() {
    return "";
  }

  setValue(value) {
  }

  onActivated() {
    this.module.onPropertyActivated(this);
  }
};

class ButtonProperty extends Property {
  constructor(module, id, label) {
    super(module, id, PropertyType.BUTTON);
    this.label = label;
    this.isIndependent = true;
    this.createNode();
  }

  createNode() {
    super.createNode();
    let thisObj = this;
    let button = document.createElement("div");
    button.innerHTML = this.label;
    button.addEventListener("click", function(event){thisObj.onClicked();});
    button.setAttribute("class", "base_bt property_bt");
    this.addObj(button);
  }

  getValue() {
    return "1";
  }

  onClicked() {
    this.onActivated();
  }
}

class ButtonSwitchProperty extends Property {
  constructor(module, id, label, value) {
    super(module, id, PropertyType.BUTTON_SW);
    this.label = label;
    this.value = value;
    this.checkbox = null;
    this.isIndependent = false;
    this.createNode();
  }

  createNode() {
    super.createNode();
    this.createLabel(this.label);

    this.checkbox = document.createElement("input");
    this.checkbox.setAttribute("type", "checkbox");
    this.checkbox.setAttribute("class", "toggle");
    this.checkbox.checked = this.value == '1' ? true : false;
    this.addObj(this.checkbox);
  }

  getValue() {
    return this.checkbox.checked ? "1" : "0";
  }

  setValue(value) {
    this.checkbox.checked = (value == "1");
  }
}

class ButtonSwitchIndependentProperty extends ButtonSwitchProperty {
  constructor(module, id, label, value) {
    super(module, id, label, value);
    this.type = PropertyType.BUTTON_SW_IN;
    this.isIndependent = true;
  }

  createNode() {
    super.createNode();
    let thisObj = this;
    this.checkbox.setAttribute("class", "toggle tg_green");
    this.checkbox.addEventListener("click", function(event){thisObj.onClicked();});
  }

  onClicked() {
    this.onActivated();
  }
}

class TextProperty extends Property {
  constructor(module, id, label, value) {
    super(module, id, PropertyType.TEXT);
    this.label = label;
    this.value = value;
    this.inputText = null;
    this.createNode();
  }

  createNode() {
    super.createNode();
    this.createLabel(this.label);
    this.inputText = document.createElement("input");
    this.inputText.setAttribute("type", "text");
    this.inputText.setAttribute("class", "property_input");
    this.inputText.value = this.value;
    this.addObj(this.inputText);
  }

  getValue() {
    return this.inputText.value;
  }

  setValue(value) {
    this.inputText.value = value;
  }
}

class TextReadOnlyProperty extends TextProperty {
  constructor(module, id, label, value) {
    super(module, id, label, value);
    this.type = PropertyType.TEXT_RO;
    this.isIndependent = true;
  }

  createNode() {
    super.createNode();
    this.inputText.classList.add("property_ro");
  }
}

class TextWithSendProperty extends TextProperty {
  constructor(module, id, label, value) {
    super(module, id, label, value);
    this.type = PropertyType.TEXT_BT;
    this.isIndependent = true;
  }

  createNode() {
    super.createNode();
    this.createSendButton()
  }
}

class TexAreaProperty extends Property {
  constructor(module, id, label, value) {
    super(module, id, PropertyType.TEXT_AREA);
    this.label = label;
    this.value = value;
    this.textArea = null;
    this.createNode();
  }

  createNode() {
    super.createNode();
    this.createLabel(this.label);
    this.textArea = document.createElement("textarea");
    this.textArea.setAttribute("class", "property_textarea");
    this.textArea.value = this.value;
    this.addObj(this.textArea);
  }

  getValue() {
    return this.textArea.value;
  }

  setValue(value) {
    this.textArea.value = value;
  }
}

class TexAreaWithSendProperty extends TexAreaProperty {
  constructor(module, id, label, value) {
    super(module, id, label, value);
    this.type = PropertyType.TEXT_AREA_RO;
    this.isIndependent = true;
  }

  createNode() {
    super.createNode();
    this.createSendButton();
  }

  getValue() {
    return this.textArea.value;
  }

  setValue(value) {
    this.textArea.value = value;
  }
}

class TextAreaReadOnlyProperty extends TexAreaProperty {
  constructor(module, id, label, value) {
    super(module, id, label, value);
    this.type = PropertyType.TEXT_AREA_RO;
    this.isIndependent = true;
  }

  createNode() {
    super.createNode();
    this.textArea.classList.add("property_ro");
  }
}

class LogProperty extends TextAreaReadOnlyProperty {
  constructor(module, id, label, value) {
    super(module, id, label, value);
    this.type = PropertyType.LOG;
  }

  setValue(value) {
    if(value.length > 0) {
      this.textArea.value = (this.textArea.value.length > 0) ? value + "\n" + this.textArea.value : value;
    }
    else {
      this.textArea.value = "";
    }
  }
}

class ScriptProperty extends Property {
  constructor(module, id, value) {
    super(module, id, PropertyType.SCRIPT);
    this.isIndependent = true;
    this.value = value;
    this.customClassObj = null;
    this.createNode();
  }

  createNode() {
    super.createNode();
    let customConstructor = new Function("propertyObj", this.value);
    this.customClassObj = customConstructor(this);
  }

  getValue() {
    return this.customClassObj.getValue();
  }

  setValue(value) {
    this.customClassObj.setValue(value);
  }
}