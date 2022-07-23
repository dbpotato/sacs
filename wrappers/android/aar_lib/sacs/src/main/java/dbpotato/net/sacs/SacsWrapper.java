/*
Copyright (c) 2021 Adam Kaniewski

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

package dbpotato.net.sacs;

import java.util.HashMap;


public class SacsWrapper {
    public interface SacsObject {
        void onPropertiesUpdated(int module_id, ModulePropertyUpdate[] properties);
    };

    public static class ModuleProperty {
        public ModuleProperty(PropType type, String name, String value) {
          this.type = type.ordinal();
          this.name = name;
          this.value = value;
        }
        public ModuleProperty(PropType type, String name) {
          this(type, name, "");
        }
        public enum PropType {
            BUTTON,
            BUTTON_SW,
            BUTTON_SW_IN,
            TEXT,
            TEXT_BT,
            TEXT_RO,
            TEXT_AREA,
            TEXT_AREA_BT,
            TEXT_AREA_RO,
            LOG,
            SCRIPT
        }
        int type;
        String name;
        String value;
    };

    public static class ModulePropertyUpdate {
        public ModulePropertyUpdate(int id, String value) {
            this.id = id;
            this.value = value;
        }
        public int id;
        public String value;
    };

    private static SacsWrapper instance;

    private SacsWrapper(){
    }

    public static SacsWrapper getInstance( ) {
        if(instance == null) {
            instance = new SacsWrapper();
            instance.valid = instance.Init();
        }
        return instance;
    }

    static {
        System.loadLibrary("sacs_jni");
    }

    public boolean isValid() {
        return valid;
    }

    public int registerModule(String name, ModuleProperty[] properties, SacsObject obj) {
        if(!valid) {
            return -2;
        }
        int module_id = RegisterModule(name, properties);
        modules.put(module_id, obj);
        return module_id;
    }

    public void unregisterModule(int module_id) {
        if(valid)
            UnregisterModule(module_id);
    }

    public void updateProperties(int module_id, ModulePropertyUpdate[] properties) {
        if(!valid) {
            return;
        }
        UpdateProperties(module_id, properties);
    }

    public static void onPropertiesUpdated(int module_id, ModulePropertyUpdate[] properties) {
        getInstance().internalOnPropertiesUpdated(module_id, properties);
    }

    private void internalOnPropertiesUpdated(int module_id, ModulePropertyUpdate[] properties) {
        modules.get(module_id).onPropertiesUpdated(module_id, properties);
    }

    private boolean valid = false;
    private HashMap<Integer,SacsObject> modules = new HashMap<Integer, SacsObject>();

    private native boolean Init();
    private native int RegisterModule(String name, ModuleProperty[] properties);
    private native void UnregisterModule(int module_id);
    private native void UpdateProperties(int module_id, ModulePropertyUpdate[] properties);
}
