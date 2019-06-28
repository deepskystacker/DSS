!define PathRemoveArgsAndQuotes "!insertmacro PathRemoveArgsAndQuotes "
!macro PathRemoveArgsAndQuotes out path
Push `${path}`
Call PathRemoveArgsAndQuotes
Pop ${out}
!macroend
Function PathRemoveArgsAndQuotes
Exch $0
Push $1 ; Pos
Push $2
Push $3 ; Quote or space
StrCpy $3 " "
StrCpy $1 ""
StrCpy $2 $0 1
StrCmp $2 '"' 0 +3
IntOp $1 $1 + 1
StrCpy $3 $2
loop:
	StrCpy $2 $0 1 $1
	StrCmp $2 "" stop
	StrCmp $2 $3 stop
	IntOp $1 $1 + 1
	Goto loop
stop:
	StrCmp $3 '"' 0 +3
	StrCpy $2 1
	IntOp $1 $1 - 1
	StrCpy $0 $0 $1 $2
Pop $3
Pop $2
Pop $1
Exch $0
FunctionEnd

