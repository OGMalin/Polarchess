del /s /q C:\Prosjekt\cpp\PolarChess\Installer\packages\com.polarchess.polarbook\data\*.*
copy C:\Prosjekt\cpp\Polarchess\x64\Release\Book.exe C:\Prosjekt\cpp\PolarChess\Installer\packages\com.polarchess.polarbook\data
call C:\qt\5.14.1\msvc2017_64\bin\qtenv2.bat
cd \prosjekt\cpp\PolarChess\Installer
call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvars64.bat"
windeployqt --compiler-runtime --force C:\Prosjekt\cpp\PolarChess\Installer\packages\com.polarchess.polarbook\data\Book.exe
C:\Qt\Tools\QtInstallerFramework\3.2\bin\binarycreator.exe --offline-only -c config\config.xml -p packages PolarChessInstaller
pause