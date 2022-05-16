class Loader {
  constructor() {
    this.loadCount = 0;
    this.scripts = [
      "view.js",
      "module.js",
      "message.builder.js",
      "messenger.js",
      "module.container.js",
      "module.manager.js",
      "module.selector.js",
      "properties.js",
      "reconnect.info.js",
      "web.app.js"
    ],
    this.includeScript(this.scripts[0]);
  }

  includeScript(url) {
    let thisObj = this;
    let script  = document.createElement('script');
    script.type = 'text/javascript';

    script.addEventListener('load', (event) => {
      thisObj.onScriptLoaded();
    });
    script.src = url;
    document.head.appendChild(script);
  }

  onScriptLoaded() {
    this.loadCount++;
    if(this.loadCount == this.scripts.length) {
      document.webApp = new WebApp();
      document.webApp.init();
    }
    else {
      this.includeScript(this.scripts[this.loadCount]);
    }
  }
};

function init() {
  new Loader();
}

