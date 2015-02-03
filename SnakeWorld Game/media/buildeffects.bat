@echo off

set OUT=Release
set PRM=

if {%1} == {debug} set OUT=Debug
if {%1} == {debug} set PRM=/Zi /Do

if {%1} == {release} set OUT=Release
if {%1} == {release} set PRM=


fxc %PRM% /nologo /Tfx_4_0 /Fo..\%OUT%\fx\effects10.fxo media\effects10.fx
fxc %PRM% /nologo /Tfx_4_0 /Fo..\%OUT%\fx\menu10.fxo media\menu10.fx
fxc %PRM% /nologo /Tfx_4_0 /Fo..\%OUT%\fx\sky10.fxo media\sky10.fx

fxc %PRM% /nologo /Tfx_2_0 /Fo..\%OUT%\fx\effects9.fxo media\effects9.fx
fxc %PRM% /nologo /Tfx_2_0 /Fo..\%OUT%\fx\menu9.fxo media\menu9.fx
fxc %PRM% /nologo /Tfx_2_0 /Fo..\%OUT%\fx\sky9.fxo media\sky9.fx

set OUT=
set PRM=