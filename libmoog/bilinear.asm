        align 32
; File "bilinear.c"
; Line 117
_szxform:
        push ebx
        push ebp
        sub esp,56
; Line 32
        fld qword ptr 116[esp]
        fld qword ptr [__Gen00]
        mov ebp,dword ptr 132[esp]
        fincstp
        mov ebx,dword ptr 136[esp]
        fcomi st(7)
        mov eax,dword ptr 116[esp]
        mov ecx,dword ptr 120[esp]
        mov dword ptr 8[esp],eax
        mov dword ptr 12[esp],ecx
        ja short __conditionalmove21

        mov eax,dword ptr [__Gen12]
        mov dword ptr 8[esp],eax
        mov eax,dword ptr [__Gen12 + 4]
        mov dword ptr 12[esp],eax
__conditionalmove21:
; Line 34
        fdecstp
        fld qword ptr 124[esp]
        fmul qword ptr [__Gen23]
        fld qword ptr 8[esp]
        fmul qword ptr [__Gen34]
        fdiv qword ptr 124[esp]
; Line 136
        sub esp,8
        fstp qword ptr [esp]
        ffree st(2)
        ffree st(1)
        fstp qword ptr 8[esp]
        call _tan

        add esp,8
        fld qword ptr 116[esp]
        fxch st(1)
; Line 34
        fld qword ptr [esp]
        fmulp st(1),st(0)
; Line 36
        fld st(0)
        fmul st(1)
; Line 32
        fxch st(2)
        fst qword ptr 8[esp]
        fld qword ptr [__Gen00]
        fincstp
        fcomip st(7)
        ja short __conditionalmove22

        mov eax,dword ptr [__Gen12]
        mov dword ptr 8[esp],eax
        mov eax,dword ptr [__Gen12 + 4]
        mov dword ptr 12[esp],eax
__conditionalmove22:
; Line 36
        fincstp
        fdivr qword ptr 84[esp]
        fstp qword ptr 84[esp]
; Line 37
        fdecstp
        fdecstp
        fdivr qword ptr 76[esp]
        fstp qword ptr 76[esp]
; Line 34
        fld qword ptr 8[esp]
        fmul qword ptr [__Gen34]
        fdiv qword ptr 124[esp]
; Line 136
        sub esp,8
        fstp qword ptr [esp]
        ffree st(5)
        call _tan

; Line 86
        fld qword ptr [ebp]
; Line 136
        add esp,8
; Line 34
        fincstp
        fmul qword ptr [esp]
; Line 37
        fdecstp
        fld qword ptr 100[esp]
        fdiv st(2)
; Line 83
        fmul qword ptr [esp]
; Line 81
        fld qword ptr 76[esp]
        fmul qword ptr [esp]
; Line 36
        fxch st(3)
        fmul st(0)
        fdivr qword ptr 108[esp]
; Line 89
        fld qword ptr 124[esp]
        fmul qword ptr [__Gen67]
; Line 83
        fld qword ptr 124[esp]
        fmul qword ptr 124[esp]
        fld st(0)
        fmul qword ptr [__Gen56]
; Line 81
        fld qword ptr 84[esp]
        fmul st(1)
        fstp qword ptr 8[esp]
        fld qword ptr 8[esp]
        fadd st(7)
        fadd qword ptr 68[esp]
        fstp qword ptr 16[esp]
; Line 95
        fld1
        fdiv qword ptr 16[esp]
        fstp qword ptr 24[esp]
        fld qword ptr 68[esp]
        fmul qword ptr [__Gen23]
        fstp qword ptr 32[esp]
        fld qword ptr 124[esp]
        fmul qword ptr 84[esp]
        fmul st(3)
        fsubr qword ptr 32[esp]
        fmul qword ptr 24[esp]
; Line 97
        fdecstp
        fsubr qword ptr 8[esp]
        fadd qword ptr 68[esp]
        fmul qword ptr 24[esp]
        fstp qword ptr 40[esp]
; Line 89
        fld qword ptr 92[esp]
        fmul qword ptr [__Gen23]
        fstp qword ptr 48[esp]
        fld qword ptr 124[esp]
        fmul st(5)
        fmul st(4)
        ffree st(4)
        fsubr qword ptr 48[esp]
; Line 83
        fdecstp
        fdecstp
        fdecstp
        fld st(0)
        fmul st(7)
; Line 91
        ffree st(7)
        fmul qword ptr [__Gen56]
        fsub st(2)
; Line 83
        fxch st(6)
        fmul st(0),st(1)
        ffree st(1)
        fadd st(2)
        ffree st(2)
        fadd qword ptr 92[esp]
; Line 86
        fld st(0)
        fincstp
        fstp st(0)
        fld1
        fdiv st(7)
        ffree st(7)
        fld qword ptr 16[esp]
        fmul st(1)
        fmul st(0),st(4)
        ffree st(4)
        fstp qword ptr [ebp]
; Line 95
        fxch st(5)
        fstp qword ptr 16[ebx]
; Line 97
        mov eax,dword ptr 40[esp]
        mov ecx,dword ptr 44[esp]
        mov dword ptr 24[ebx],eax
        mov dword ptr 28[ebx],ecx
; Line 91
        fld qword ptr 92[esp]
; Line 136
        add esp,56
; Line 91
        fadd st(0),st(6)
        ffree st(6)
        fmul st(5)
        fstp qword ptr 8[ebx]
; Line 89
        fdecstp
        fdecstp
        fdecstp
        fdecstp
        fmul st(0),st(7)
        ffree st(7)
        fstp qword ptr [ebx]
; Line 136
        pop ebp
        pop ebx
        ret

__EndProcedure24:
