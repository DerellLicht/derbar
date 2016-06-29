//**********************************************************************
//  Copyright (c) 2009-2014  Daniel D Miller
//  derbar.exe - Another WinBar application
//  lintdefs.cpp: PcLint rules.
//  This file is not linked into the project, it is only used for
//  running Lint on the sources
//  
//  DerBar, its source code and executables, are Copyrighted in their
//  unmodified form by Daniel D Miller, and are distributed as free
//  software, with only one restriction:
//  
//  Any modified version of the program cannot be distributed with
//  the name DerBar.
//  
//  Other than this, the source code, executables, help files, and any
//  other related files are provided with absolutely no restriction 
//  on use, distribution, modification, commercial adaptation, 
//  or any other conditions.
//  
//  Written by:   Daniel D. Miller
//  
//  Last Update:  10/05/12 09:43
//**********************************************************************

//lint -e10  Expecting '}' even though nothing is wrong with the code

//lint -e840  Use of nul character in a string literal (is quite common)
//lint -e845  The right argument to operator '|' is certain to be 0 

//lint -e1712  default constructor not defined for class 'registry_iface'

//lint -e1762  Member function could be made const
//lint -e1066  Symbol declared as "C" conflicts with something else that's identical

//lint -e650  Constant '-12' out of range for operator 'case' (used alot by Win32)
//lint -esym(537, \mingw\include\prsht.h)

//lint -esym(601, monitor_object_s::FILE)
//lint -esym(768, monitor_object_s::FILE)
//lint -esym(114, monitor_object_s)

//lint -esym(714, termout)
//lint -esym(759, termout)
//lint -esym(765, termout)

//lint -esym(534, syslog, wsprintfA, SetDlgItemTextA, lstrcatA, SendMessageA, ShowWindow, SleepEx)
//lint -esym(534, SetTextAlign, EnableWindow, GetWindowText, PostMessageA, DestroyWindow)
//lint -esym(534, SetWindowTextA, GetWindowTextA, lstrcpyA, SetWindowLongA, lstrcpynA)
//lint -esym(534, SelectObject, DeleteObject, ReleaseDC, MoveToEx)

//lint -esym(515, syslog)
//lint -esym(516, syslog)

//lint -esym(534, termout, errout, txout, rxout, hex_dump, rand_chars, LineTo)

//lint -esym(119, strtod)

//lint -esym(756, reg_value_p, reg_cmd_t, reg_cmd_p)

//lint -e786   String concatenation within initializer
//lint -e525   Negative indentation from line ...
//lint -e539   Did not expect positive indentation from line ...
//lint -e725   Expected positive indentation from line 1470
//lint -e716   while(1) ...
//lint -e731   Boolean argument to equal/not equal
//lint -e801   Use of goto is deprecated
//lint -e818   Pointer parameter could be declared as pointing to const
//lint -e830   Location cited in prior message
//lint -e831   Reference cited in prior message
//lint -e834   Operator '-' followed by operator '-' is confusing.  Use parentheses.
//lint -e1776  Converting a string literal to char * is not const safe (arg. no. 2)
//lint -e1773  Attempt to cast away const (or volatile)
//lint -e526   Symbol 'socket(int, int, int)' not defined (yes, they are)
//lint -e730   Boolean argument to function that takes boolean argument (duh)

//lint -e768   global struct member not referenced
//lint -e1714  Member function not referenced

//lint -e755   global macro not referenced
//lint -e534   Ignoring return value of function

//  rules that should be put back later, when I'm feeling really anal.
//  Generally, these are warnings about things that the compiler knows how to handle.
//lint -e713   Loss of precision (assignment) (unsigned int to int)  (123 warnings left)
//lint -e732   Loss of sign (arg. no. 5) (int to unsigned int) (94 warnings left)

