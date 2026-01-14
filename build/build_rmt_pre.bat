rem This is called in the "src" folder.
rem The first parameter is the output directory.

rem Prepare template folder with the most recent contents.
xcopy ..\doc\*.html  ..\rmt\docs
xcopy ..\doc\*.gif   ..\rmt\docs

rem Clean the output folder completely.
del /Q /S %1

