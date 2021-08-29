#!/bin/bash

cp WebAppDataTemplate.cpp WebAppData.cpp

for filename in ./webapp/*; do
    if [ -f "$filename" ]; then
        base=$(basename $filename);
        echo "_resources[\"$base\"] =  R\"\"\"(" >> WebAppData.cpp;
        cat "$filename" >> WebAppData.cpp;
        echo -e ")\"\"\";\n" >> WebAppData.cpp;
    fi
done

echo -e "\n};\n" >> WebAppData.cpp
