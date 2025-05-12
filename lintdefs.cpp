//**********************************************************************
//  Copyright (c) 2009-2023  Daniel D Miller
//  lintdefs.cpp: PcLint rules for DerBar utility
//  This file is not linked into the project, 
//  it is only used for running Lint on the sources
//**********************************************************************

//lint -e10  Expecting '}' even though nothing is wrong with the code

//lint -e840  Use of nul character in a string literal (is quite common)
//lint -e845  The right argument to operator '|' is certain to be 0 

//lint -e1762  Member function could be made const
//lint -e1066  Symbol declared as "C" conflicts with something else that's identical

//lint -esym(119, strtod)

//  case NM_CLICK:
//lint -e650  Constant '-2' out of range for operator 'case'

//  later C++ constructs that PcLint V9.00L does not support
//lint -esym(628, std::__is_trivially_copyable, std::__is_base_of)
//lint -e740   Unusual pointer cast (incompatible indirect types)
//lint -e1055  Symbol undeclared, assumed to return int (except it *is* declared)
//lint -e78    Symbol typedef'ed at location unknown used in expression
//lint -e18    Symbol redeclared conflicts with ... (no, it doesn't)
//lint -e61    Bad type (no, it isn't)
//lint -e78

//lint -e731   Boolean argument to equal/not equal
//lint -e801   Use of goto is deprecated
//lint -e818   Pointer parameter could be declared as pointing to const
//lint -e1776  Converting a string literal to char * is not const safe (arg. no. 2)
//lint -e1773  Attempt to cast away const (or volatile)
//lint -e730   Boolean argument to function that takes boolean argument (duh)

//lint -e534   Ignoring return value of function
//lint -e768   global struct member not referenced
//lint -e755   global macro not referenced
//lint -e1714  Member function not referenced

//  rules that should be put back later, when I'm feeling really anal.
//  Generally, these are warnings about things that the compiler knows how to handle.
//lint -e713   Loss of precision (assignment) (unsigned int to int)  (123 warnings left)
//lint -e732   Loss of sign (arg. no. 5) (int to unsigned int) (94 warnings left)

