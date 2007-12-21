echo Tausche Dummy-Icon gegen Vista-Icon
contrib\ReplaceVistaIcon.exe bin\surfer.exe resource\surfer.ico
echo Komprimiere EXE
contrib\upx.exe --best bin\surfer.exe