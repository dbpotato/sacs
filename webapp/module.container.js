class ModuleContainer extends View {
  constructor() {
    super();
    this.createNode();
  }

  createNode() {
    super.createNode();
    this.setId("main_content");
  }

  setModlule(module) {
    this.clearNode();
    if (module != null) {
      this.addObj(module.node);
    }
  }
};
