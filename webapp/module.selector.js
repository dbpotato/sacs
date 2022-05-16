class ModuleButton extends View {
  constructor(module) {
    super();
    this.module = module;
    this.button = null;
    this.createNode();
  }

  createNode() {
    super.createNode();
    let thisObj = this;
    this.button = document.createElement("div");
    this.button.setAttribute("class", "base_bt module_bt");
    this.button.innerHTML = this.module.name;
    this.button.addEventListener("click", function(event) {thisObj.onClicked()});
    this.addObj(this.button);
  }

  onClicked() {
    document.webApp.moduleSelector.selectModule(this.module);
  }

  setSelected(isSelected) {
    if(isSelected) {
      this.button.classList.add("module_bt_selected");
    } else {
      this.button.classList.remove("module_bt_selected");
    }
  }
}

class ModuleSelector extends View {
  constructor() {
    super();
    this.createNode();
    this.currentModule =  null;
    this.buttons = new Map();
  }

  createNode() {
    super.createNode();
    this.setId("ModuleSelector");
  }

  onModuleAdded(module) {
    let moduleButton = new ModuleButton(module);
    this.buttons.set(module, moduleButton);
    this.addObj(moduleButton.node);
    if(this.currentModule == null) {
      this.selectModule(module);
    }
  }

  onModuleRemoved(module) {
    if(module == this.currentModule) {
      document.webApp.moduleContainer.setModlule(null);
      this.currentModule = null;
    }
    let button = this.buttons.get(module);
    this.removeObj(button.node);
    this.buttons.delete(button);
  }

  getCurrentButton() {
    if(this.currentModule == null) {
      return null;
    }
    return this.buttons.get(this.currentModule);
  }

  selectCurrentButton() {
    let currentButton = this.getCurrentButton();
    if(currentButton !== null) {
      currentButton.setSelected(true);
    }
  }

  unselectCurrentButton() {
    let currentButton = this.getCurrentButton();
    if(currentButton !== null) {
      currentButton.setSelected(false);
    }
  }

  selectModule(module) {
    this.unselectCurrentButton();
    this.currentModule = module;
    document.webApp.moduleContainer.setModlule(module);
    this.selectCurrentButton();
  }

  clear() {
    this.buttons.forEach((button, module) => {
      button.deleteNode();
    })
    this.buttons.clear();
    this.currentModule = null;
  }
};
