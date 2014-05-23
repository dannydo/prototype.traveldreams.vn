IF EXIST TempResource GOTO WINDIR
mkdir TempResource
:WINDIR
IF EXIST .\TempResource\P1 GOTO P1DIR
cd
mkdir .\TempResource\P1
:P1DIR
IF EXIST .\TempResource\P2 GOTO P2DIR
mkdir .\TempResource\P2
:P2DIR

move .\Resources\SoundEffectPC  .\TempResource\SoundEffectPC  
move .\Resources\GameData\Words\P1\EN_SoundPC .\TempResource\P1\EN_SoundPC
move .\Resources\GameData\Words\P2\EN_SoundPC .\TempResource\P2\EN_SoundPC
pause