#!/bin/bash

cd ../..
rm bin/Release-linux-x86_64/SandboxApp/SandboxApp

if [ ! -f bin/Release-linux-x86_64/SandboxApp/run.sh ]; then
    touch bin/Release-linux-x86_64/SandboxApp/run.sh > "#!/bin/bash
./SandboxApp"
fi

#bulding app
make config=release

if [ $? -eq 0 ]; then
    #check if compilation succeed or not
    #xdg-open bin/Release-linux-x86_64/SandboxApp
    echo "Succeed!"
else
    echo "Error was occured!"
fi

#bin/Release-linux-x86_64/SandboxApp/run.sh
