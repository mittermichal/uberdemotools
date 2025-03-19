echo Visual Studio express? y for yes, anything else for no
set /p choice=
echo %choice%
if %choice%==y (
	set vs_exe=VCExpress.exe
) else (
	set vs_exe=devenv.exe
)