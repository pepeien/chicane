echo Compiling Vertice Shaders

for %%f in (*.vert) do (
	echo Compiling: %%f
	glslc %%f -o vertices\%%~nf.spv
)

echo Compiling Fragment Shaders

for %%f in (*.frag) do (
	echo Compiling: %%f
	glslc %%f -o fragments\%%~nf.spv
)

pause