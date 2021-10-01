::@ECHO OFF
COLOR 0A
SET BatchName=%~n0
SET BatchDir=%cd%\
CD ..
CD ..
SET hexDir=%cd%\OUT\
SET HexPath=%hexDir%%BatchName%.hex
CD %BatchDir%
ST-LINK_CLI -C SWD UR -ME -P %HexPath% -V -ClrBP -RST
Pause