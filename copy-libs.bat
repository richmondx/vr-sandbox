
echo off

echo "Copying Bins Win64"
robocopy ".\Plugins\Assimp\Win64\bin" ".\Binaries\Win64"
echo "Copying Libs Win64"
robocopy ".\Plugins\Assimp\Win64\lib" ".\Binaries\Win64"