
# SACS Wrapper for Android

The Android wrapper takes base sacs native library, adds JNI communication, Java interface and packs it all into an AAR library.
Functionality is the same as described in **sacs/README.md**, this documentation covers differences in usage.


## Getting started

- checkout submodules : **git submodule update --init --recursive**
- edit **sacs/wrappers/android/build.sh** and set paths for Android's SDK/NDK
- run **sacs/wrappers/android/build.sh** script
- in build.gradle for your project add to dependencies generated aar library:
-- **implementation files('PATH_TO_SACS_AAR')**
- in AndroidManifest.xml for your project add permission for internet access
-- **\<uses-permission android:name="android.permission.INTERNET"/>**  

## Differences between java and c++ wrapper

### Module callback
Instead of function here is *SacsObject* interface with *onPropertiesUpdated* method to override
```java
  public interface SacsObject {
    void onPropertiesUpdated(int module_id, ModulePropertyUpdate[] properties);
  };
```
### Module registration
Mostly the same as with the C++ wrapper

```java
  ModuleProperty[] props = {
    new ModuleProperty(ModuleProperty.PropType.LOG, "Log", "Hello"),
  };
  int modId = SacsWrapper.getInstance().registerModule("Button and Log", props, this);
```
 ### Updating UI state 
 Mostly the same as with the C++ wrapper
```java
  ModulePropertyUpdate[] update = {new ModulePropertyUpdate(0, "New Value")};
  SacsWrapper.getInstance().updateProperties(mModuleId, update);
```

## Buttons and log example in Java
This is what recreation of **sacs/examples/ButtonAndLog.cpp** should look like :
```java

import androidx.appcompat.app.AppCompatActivity;
import android.os.Bundle;

import dbpotato.net.sacs.SacsWrapper;
import dbpotato.net.sacs.SacsWrapper.ModuleProperty;
import dbpotato.net.sacs.SacsWrapper.ModulePropertyUpdate;
import dbpotato.net.sacs.SacsWrapper.SacsObject;

public class MainActivity extends AppCompatActivity {

    private  SacsImpl sacs;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        sacs = new SacsImpl();
    }

    private class SacsImpl implements SacsObject {
        private int mModId;
        private Integer mEnabled = 0;
        private Integer mCounter = 0;
        public SacsImpl() {
            createModule();
        }
        public void createModule() {
            SacsWrapper sacs = SacsWrapper.getInstance();
            ModuleProperty[] props = {
                new ModuleProperty(ModuleProperty.PropType.LOG, "Log"),
                new ModuleProperty(ModuleProperty.PropType.BUTTON, "Add Entry"),
                new ModuleProperty(ModuleProperty.PropType.BUTTON_SW, "Enabled", mEnabled.toString())
            };
            mModId = sacs.registerModule("Button and Log", props, this);
        }
        @Override
        public void onPropertiesUpdated(int moduleId, ModulePropertyUpdate[] properties) {
            if(properties[0].id == 1) {
                String msg = "";
                if ((++mCounter % 6) != 0) {
                    msg = ((mEnabled == 1) ? "Enabled" : "Disabled") + " : " + mEnabled;
                } else {
                    mCounter = 0;
                }
                ModulePropertyUpdate[] update = {new ModulePropertyUpdate(0, msg)};
                SacsWrapper.getInstance().updateProperties(mModId, update);
            }
            else {
                mEnabled = Integer.parseInt(properties[0].value);
            }
        }
    }
}
```

