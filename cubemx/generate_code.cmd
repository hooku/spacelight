set generate_dir=Z:\generated

rmdir /q /s %generate_dir%
mkdir %generate_dir%
copy * %generate_dir%\

set cubemx_script=%generate_dir%\spacelight.txt

echo config load %generate_dir%\spacelight.ioc> %cubemx_script%
echo project generate>> %cubemx_script%
echo exit>> %cubemx_script%

"C:\Program Files\STMicroelectronics\STM32Cube\STM32CubeMX\STM32CubeMX.exe" -q %cubemx_script%
timeout 50

Z:
cd %generate_dir%
"C:\Program Files\Git\usr\bin\patch.exe" -p1 < spacelight.patch
