{{
''*******************************************
''*  PWM Object                        V4.0 *
''*  Author: Beau Schwabe                   *
''*  Copyright (c) 2009 Parallax, Inc.      *               
''*  See end of file for terms of use.      *               
''*******************************************

Revision History:
  Version 1.0   - (05/01/2009) initial release
  Version 2.0   - (08/06/2009) bug/error fix:
                                   1)Time on/off mode swapped in PWM function
                                   2)locked I/O states when returning from 0% or 100% Duty Cycle modes
                                     to 'normal' 1% to 99% Duty Cycle modes.
  Version 3.0   - (??/??/20??) 3rd party changes were made.  Use v3.0 at your own discretion  
  Version 4.0   - (05/11/2011) bug/error fix:
                                   1) Resolution was erroniously previously set to 8.15us

}}
CON
  _CLKMODE = XTAL1 + PLL16X
  _XINFREQ = 5_000_000

''Resolution
Resolution      = 8200 '<-- 8.2us             

''Commands
UpdateTonToff   = 1
IO_State        = 2
DutyOverRide    = 3
SyncPhase       = 4

''DutyModes
Duty_default    = 0 '<-- Can be anything other than 1 or 2             
Duty_100        = 1
Duty_0          = 2

''StateModes
DisablePin      = 0
EnablePin       = 1


VAR
long    cog,command, ARG0, ARG1, ARG2

PUB Stop
    cogstop(cog)

PUB Start                                              '' Initialize PWM cog
    cog := cognew(@PWM_Asm, @command)

PUB PhaseSync(Pin1,Pin2,Phase)                         '' Pin1 = 0 to 31                          
                                                       '' Pin2 = 0 to 31
    Phase   := (Phase * 1000)/ Resolution              '' Phase = amount of signal offset
                                                       '' in microseconds
    ARG0    := Pin1 
    ARG1    := Pin2                                    '' The current Phase of Pin1 is copied,
    ARG2    := Phase                                   '' the Phase offset is added to that
    command := SyncPhase                               '' and the Phase of Pin2 is updated with
    repeat until command == 0                          '' the new value.

PUB Duty(Pin,DutyCycle,Period)|BasePeriod,Ton,Toff     '' Pin = 0 to 31
    If Period <> 0
       If DutyCycle == 0                               '' DutyCycle = 0 to 100
          DutyMode(Pin,2)
          StateMode(Pin,1)
       If DutyCycle == 100
          DutyMode(Pin,1)
          StateMode(Pin,1)       
       If DutyCycle <> 0 and DutyCycle <> 100
          BasePeriod := (Period*1000)/Resolution       '' Period is in micro seconds and
          Ton := (DutyCycle * BasePeriod) / 100        '' represents the repetition rate 
          Toff := BasePeriod - Ton                     '' of the PWM signal. 
          if Ton == 0
             DutyMode(Pin,1)
          if Toff == 0
             DutyMode(Pin,2)
          if Ton ==0 and Toff == 0
             StateMode(Pin,0)
          if Ton<>0 or Toff<>0
             PWM(Pin,Ton ,Toff)                '' If Period = 0 then the pin is disabled
             DutyMode(Pin,0)       
    else
       StateMode(Pin,0)                                   
                   
    
PUB Servo(Pin,PulseWidth)|Ton,Toff             '' Pin = 0 to 31
    If PulseWidth <> 0                         '' PulseWidth ... 1000 = 1us / 2000 = 2us
       Ton  := (PulseWidth * 1000)/Resolution  '' If Pulsewidth = 0 then the pin is disabled
       Toff := ((20000-PulseWidth)*1000)/Resolution
       PWM(Pin,Ton ,Toff)
    else
       StateMode(Pin,0)    

PUB PWM(Pin,OnTime,OffTime)                    '' Pin = 0 to 31

    If OnTime ==0 and OffTime == 0
        StateMode(Pin,0)

    If OnTime == 0                             '' An OnTime value of 0 forces a 0% Duty Cycle
       DutyMode(Pin,2)     'error fix; changed to 2                         
       StateMode(Pin,1)    'bug fix; added to prevent small error when OnTime starts out as Zero    
    If OffTime == 0                            '' An OffTime value of 0 forces a 100% Duty Cycle                                     
       DutyMode(Pin,1)     'error fix; changed to 1                    
       StateMode(Pin,1)    'bug fix; added to prevent small error when OnTime starts out as Zero   
    If OnTime ==0 and OffTime == 0             '' If OnTime & OffTime are both 0 then the pin is         
       StateMode(Pin,0)                        '' disabled.
    If OnTime <>0 and OffTime <>0              '' ONLY allow OnTime and OffTime to update if they are NOT Zero         
       ARG0    := Pin
       ARG1    := OnTime - 1
       ARG2    := OffTime - 1
       command := UpdateTonToff
       repeat until command == 0
       StateMode(Pin,1)
       DutyMode(Pin,0)     'bug fix; added to make sure Default Duty cycle is selected            

PUB StateMode(Pin,State)                       '' Pin = 0 to 31
    ARG0    := Pin                             '' If State is 0 then pin is disabled
    ARG1    := State                           '' If State is 1 then pin is enabled
    command := IO_State
    repeat until command == 0

PUB DutyMode(Pin,Mode)                         '' Pin = 0 to 31
    ARG0    := Pin                             '' If Duty = 0 then the pin is in it's default state
    ARG1    := Mode                            '' If Duty = 1 then the pin is forced HIGH ; 100%
    command := DutyOverRide                    '' If Duty = 2 then the pin is forced LOW  ; 0%
    repeat until command == 0

DAT
PWM_Asm       org

              mov      temp, par
              mov      cmd_address, temp        'Acquire command variable address
              add      temp,    #4  
              mov      ARG_0, temp              'Acquire ARG_0 variable address
              add      temp,    #4  
              mov      ARG_1, temp              'Acquire ARG_1 variable address
              add      temp,    #4
              mov      ARG_2, temp              'Acquire ARG_2 variable address

Main_PWM_Loop
'-------------------------------------------------------------
              sub      t1 + 00, #1              wc
Ch01    if_c  xor      Ch + 00, Mask     + 00   wz
  if_c_and_z  mov      t1 + 00, T_Off_Ch + 00
  if_c_and_nz mov      t1 + 00, T_On__Ch + 00
'-------------------------------------------------------------   
              sub      t1 + 01, #1              wc
Ch02    if_c  xor      Ch + 01, Mask     + 01   wz
  if_c_and_z  mov      t1 + 01, T_Off_Ch + 01
  if_c_and_nz mov      t1 + 01, T_On__Ch + 01
'-------------------------------------------------------------
              sub      t1 + 02, #1              wc
Ch03    if_c  xor      Ch + 02, Mask     + 02   wz
  if_c_and_z  mov      t1 + 02, T_Off_Ch + 02
  if_c_and_nz mov      t1 + 02, T_On__Ch + 02
'-------------------------------------------------------------   
              sub      t1 + 03, #1              wc
Ch04    if_c  xor      Ch + 03, Mask     + 03   wz
  if_c_and_z  mov      t1 + 03, T_Off_Ch + 03
  if_c_and_nz mov      t1 + 03, T_On__Ch + 03
'-------------------------------------------------------------
              sub      t1 + 04, #1              wc
Ch05    if_c  xor      Ch + 04, Mask     + 04   wz
  if_c_and_z  mov      t1 + 04, T_Off_Ch + 04
  if_c_and_nz mov      t1 + 04, T_On__Ch + 04
'-------------------------------------------------------------   
              sub      t1 + 05, #1              wc
Ch06    if_c  xor      Ch + 05, Mask     + 05   wz
  if_c_and_z  mov      t1 + 05, T_Off_Ch + 05
  if_c_and_nz mov      t1 + 05, T_On__Ch + 05
'-------------------------------------------------------------
              sub      t1 + 06, #1              wc
Ch07    if_c  xor      Ch + 06, Mask     + 06   wz
  if_c_and_z  mov      t1 + 06, T_Off_Ch + 06
  if_c_and_nz mov      t1 + 06, T_On__Ch + 06
'-------------------------------------------------------------   
              sub      t1 + 07, #1              wc
Ch08    if_c  xor      Ch + 07, Mask     + 07   wz
  if_c_and_z  mov      t1 + 07, T_Off_Ch + 07
  if_c_and_nz mov      t1 + 07, T_On__Ch + 07
'-------------------------------------------------------------
              sub      t1 + 08, #1              wc
Ch09    if_c  xor      Ch + 08, Mask     + 08   wz
  if_c_and_z  mov      t1 + 08, T_Off_Ch + 08
  if_c_and_nz mov      t1 + 08, T_On__Ch + 08
'-------------------------------------------------------------   
              sub      t1 + 09, #1              wc
Ch10    if_c  xor      Ch + 09, Mask     + 09   wz
  if_c_and_z  mov      t1 + 09, T_Off_Ch + 09
  if_c_and_nz mov      t1 + 09, T_On__Ch + 09
'-------------------------------------------------------------
              sub      t1 + 10, #1              wc
Ch11    if_c  xor      Ch + 10, Mask     + 10   wz
  if_c_and_z  mov      t1 + 10, T_Off_Ch + 10
  if_c_and_nz mov      t1 + 10, T_On__Ch + 10
'-------------------------------------------------------------   
              sub      t1 + 11, #1              wc
Ch12    if_c  xor      Ch + 11, Mask     + 11   wz
  if_c_and_z  mov      t1 + 11, T_Off_Ch + 11
  if_c_and_nz mov      t1 + 11, T_On__Ch + 11
'-------------------------------------------------------------
              sub      t1 + 12, #1              wc
Ch13    if_c  xor      Ch + 12, Mask     + 12   wz
  if_c_and_z  mov      t1 + 12, T_Off_Ch + 12
  if_c_and_nz mov      t1 + 12, T_On__Ch + 12
'-------------------------------------------------------------   
              sub      t1 + 13, #1              wc
Ch14    if_c  xor      Ch + 13, Mask     + 13   wz
  if_c_and_z  mov      t1 + 13, T_Off_Ch + 13
  if_c_and_nz mov      t1 + 13, T_On__Ch + 13
'-------------------------------------------------------------
              sub      t1 + 14, #1              wc
Ch15    if_c  xor      Ch + 14, Mask     + 14   wz
  if_c_and_z  mov      t1 + 14, T_Off_Ch + 14
  if_c_and_nz mov      t1 + 14, T_On__Ch + 14
'-------------------------------------------------------------   
              sub      t1 + 15, #1              wc
Ch16    if_c  xor      Ch + 15, Mask     + 15   wz
  if_c_and_z  mov      t1 + 15, T_Off_Ch + 15
  if_c_and_nz mov      t1 + 15, T_On__Ch + 15
'-------------------------------------------------------------
              sub      t1 + 16, #1              wc
Ch17    if_c  xor      Ch + 16, Mask     + 16   wz
  if_c_and_z  mov      t1 + 16, T_Off_Ch + 16
  if_c_and_nz mov      t1 + 16, T_On__Ch + 16
'-------------------------------------------------------------   
              sub      t1 + 17, #1              wc
Ch18    if_c  xor      Ch + 17, Mask     + 17   wz
  if_c_and_z  mov      t1 + 17, T_Off_Ch + 17
  if_c_and_nz mov      t1 + 17, T_On__Ch + 17
'-------------------------------------------------------------
              sub      t1 + 18, #1              wc
Ch19    if_c  xor      Ch + 18, Mask     + 18   wz
  if_c_and_z  mov      t1 + 18, T_Off_Ch + 18
  if_c_and_nz mov      t1 + 18, T_On__Ch + 18
'-------------------------------------------------------------   
              sub      t1 + 19, #1              wc
Ch20    if_c  xor      Ch + 19, Mask     + 19   wz
  if_c_and_z  mov      t1 + 19, T_Off_Ch + 19
  if_c_and_nz mov      t1 + 19, T_On__Ch + 19
'-------------------------------------------------------------
              sub      t1 + 20, #1              wc
Ch21    if_c  xor      Ch + 20, Mask     + 20   wz
  if_c_and_z  mov      t1 + 20, T_Off_Ch + 20
  if_c_and_nz mov      t1 + 20, T_On__Ch + 20
'-------------------------------------------------------------   
              sub      t1 + 21, #1              wc
Ch22    if_c  xor      Ch + 21, Mask     + 21   wz
  if_c_and_z  mov      t1 + 21, T_Off_Ch + 21
  if_c_and_nz mov      t1 + 21, T_On__Ch + 21
'-------------------------------------------------------------
              sub      t1 + 22, #1              wc
Ch23    if_c  xor      Ch + 22, Mask     + 22   wz
  if_c_and_z  mov      t1 + 22, T_Off_Ch + 22
  if_c_and_nz mov      t1 + 22, T_On__Ch + 22
'-------------------------------------------------------------   
              sub      t1 + 23, #1              wc
Ch24    if_c  xor      Ch + 23, Mask     + 23   wz
  if_c_and_z  mov      t1 + 23, T_Off_Ch + 23
  if_c_and_nz mov      t1 + 23, T_On__Ch + 23
'-------------------------------------------------------------
              sub      t1 + 24, #1              wc
Ch25    if_c  xor      Ch + 24, Mask     + 24   wz
  if_c_and_z  mov      t1 + 24, T_Off_Ch + 24
  if_c_and_nz mov      t1 + 24, T_On__Ch + 24
'-------------------------------------------------------------   
              sub      t1 + 25, #1              wc
Ch26    if_c  xor      Ch + 25, Mask     + 25   wz
  if_c_and_z  mov      t1 + 25, T_Off_Ch + 25
  if_c_and_nz mov      t1 + 25, T_On__Ch + 25
'-------------------------------------------------------------
              sub      t1 + 26, #1              wc
Ch27    if_c  xor      Ch + 26, Mask     + 26   wz
  if_c_and_z  mov      t1 + 26, T_Off_Ch + 26
  if_c_and_nz mov      t1 + 26, T_On__Ch + 26
'-------------------------------------------------------------   
              sub      t1 + 27, #1              wc
Ch28    if_c  xor      Ch + 27, Mask     + 27   wz
  if_c_and_z  mov      t1 + 27, T_Off_Ch + 27
  if_c_and_nz mov      t1 + 27, T_On__Ch + 27
'-------------------------------------------------------------
              sub      t1 + 28, #1              wc
Ch29    if_c  xor      Ch + 28, Mask     + 28   wz
  if_c_and_z  mov      t1 + 28, T_Off_Ch + 28
  if_c_and_nz mov      t1 + 28, T_On__Ch + 28
'-------------------------------------------------------------   
              sub      t1 + 29, #1              wc
Ch30    if_c  xor      Ch + 29, Mask     + 29   wz
  if_c_and_z  mov      t1 + 29, T_Off_Ch + 29
  if_c_and_nz mov      t1 + 29, T_On__Ch + 29
'-------------------------------------------------------------
              sub      t1 + 30, #1              wc
Ch31    if_c  xor      Ch + 30, Mask     + 30   wz
  if_c_and_z  mov      t1 + 30, T_Off_Ch + 30
  if_c_and_nz mov      t1 + 30, T_On__Ch + 30
'-------------------------------------------------------------   
              sub      t1 + 31, #1              wc
Ch32    if_c  xor      Ch + 31, Mask     + 31   wz
  if_c_and_z  mov      t1 + 31, T_Off_Ch + 31
  if_c_and_nz mov      t1 + 31, T_On__Ch + 31
'-------------------------------------------------------------
'' Update IO Pins
              mov      temp,    Ch + 00                 ' Clear and initialize temp
              or       temp,    Ch + 01                 '   OR the remaining 
              or       temp,    Ch + 02                 '   channels into temp
              or       temp,    Ch + 03
              or       temp,    Ch + 04
              or       temp,    Ch + 05
              or       temp,    Ch + 06  
              or       temp,    Ch + 07                      
              or       temp,    Ch + 08
              or       temp,    Ch + 09
              or       temp,    Ch + 10 
              or       temp,    Ch + 11
              or       temp,    Ch + 12
              or       temp,    Ch + 13
              or       temp,    Ch + 14
              or       temp,    Ch + 15                      
              or       temp,    Ch + 16
              or       temp,    Ch + 17                        
              or       temp,    Ch + 18
              or       temp,    Ch + 19
              or       temp,    Ch + 20
              or       temp,    Ch + 21
              or       temp,    Ch + 22
              or       temp,    Ch + 23
              or       temp,    Ch + 24
              or       temp,    Ch + 25                        
              or       temp,    Ch + 26
              or       temp,    Ch + 27                      
              or       temp,    Ch + 28
              or       temp,    Ch + 29
              or       temp,    Ch + 30
              or       temp,    Ch + 31
              mov      outa,    temp                    ' Move temp to the IOs
'-------------------------------------------------------------
''Check if Command is waiting
              rdlong   temp,    cmd_address     wz      ' Read command
         if_z jmp      #Main_PWM_Loop                   ' Return to main PWM loop

'-------------------------------------------------------------                
''Command Detected ; Check for valid Commands
              xor      temp,    #UpdateTonToff  wz,nr   ' Check Ton/Toff              
         if_z jmp      #_Update_TonToff                 
        
              xor      temp,    #IO_State       wz,nr   ' Check IO state        
         if_z jmp      #_Update_IO_State

              xor      temp,    #DutyOverRide   wz,nr   ' Check DutyOverRide        
         if_z jmp      #_DutyOverRide

              xor      temp,    #SyncPhase      wz,nr   ' Check PhaseSync        
         if_z jmp      #_SyncPhase

              wrlong    Zero,    cmd_address            ' Tell SPIN command has been processed
              jmp      #Main_PWM_Loop                   ' Return to main PWM loop

'-------------------------------------------------------------
_Update_TonToff
              rdlong    _Index,  ARG_0                  ' Read the selected pin number
              rdlong    _Ton,    ARG_1                  ' Read the TimeOn 
              add       _Index,  #T_Off_Ch              ' Calculate database offset
              movd      Tf,      _Index                 ' Self-modify TimeOn address
              rdlong    _Toff,   ARG_2                  ' Read the TimeOff
              add       _Index,  #32                    ' Calculate database offset
              movd      Tn,      _Index                 ' Self-modify TimeOff address
              wrlong    Zero,    cmd_address            ' Tell SPIN command has been processed
Tn            mov       00_00,   _Ton                   ' Move TimeOn into modified address 
Tf            mov       00_00,   _Toff                  ' Move TimeOff into modified address
              jmp      #Main_PWM_Loop                   ' Return to main PWM loop
'-------------------------------------------------------------        
_Update_IO_State
              rdlong    _Index,  ARG_0                  ' Read the selected pin number
              rdlong    _PinValue, ARG_1                ' Read the desired pin State
              mov       temp,    #1                     ' Create mask for selected pin
              shl       temp,    _Index
              wrlong    Zero,    cmd_address            ' Tell SPIN command has been processed
              test      _PinValue,#1            wz      ' Check Pin State
              nop
        if_nz or        dira,    temp                   '   If State is '1' then set dira bit
        if_z  andn      dira,    temp                   '   If State is '0' then clear dira bit
              jmp      #Main_PWM_Loop                   ' Return to main PWM loop
'-------------------------------------------------------------
_DutyOverRide
              rdlong    _Index,    ARG_0                ' Read the selected pin number
              rdlong    _OverRide, ARG_1                ' Read the desired OverRide State
              
              mov       temp,      _Index               ' Calculate Index 
              shl       temp,      #2                   '   Multiply by 4 (<-this is the number of
                                                        '   instructions between each Channel) 
              add       temp,      #Ch01                ' Add Calculated Index to Ch01 reference
              movd      ChModify,  temp                 ' Self-modify Channel destination address
              nop
 
              mov       temp,      D_Default            ' Force Default
              xor       _OverRide,  #1           wz,nr  ' Force 100% Duty              
         if_z mov       temp,      D_100  
              xor       _OverRide,  #2           wz,nr  ' Force 0% Duty              
         if_z mov       temp,      D_0

ChModify      movi      00_00 , temp                    ' Self-modify Duty Changes to Channel
              wrlong    Zero,    cmd_address            ' Tell SPIN command has been processed
              jmp      #Main_PWM_Loop                   ' Return to main PWM loop

'-------------------------------------------------------------
_SyncPhase                                              
              rdlong    temp1,       ARG_0              'Read Channel position 1
              add       temp1,       #t1                'add database offset
              movs      Ch1_Read,   temp1               'Self modify code to read proper position            
              nop                                       'delay for pipelining
Ch1_Read      mov       temp1,      00_00               'Read current value

              rdlong    temp2,       ARG_2              'Read Phase Offset
              add       temp1,       temp2              'Add Phase Offset

              rdlong    temp2,       ARG_1              'Read Channel position 2
              add       temp2,       #t1                'add database offset
              movd      Ch2_Write,   temp2              'Self modify code to write proper position           
              nop                                       'delay for pipelining
Ch2_Write     mov       00_00,       temp1              'Write new value                           

              wrlong    Zero,    cmd_address            ' Tell SPIN command has been processed
              jmp      #Main_PWM_Loop                   ' Return to main PWM loop

''-------------------------Defined variables---------------------------------
D_100         long      %011010_001                     ' or   - OpCode with Z flag cleared
D_0           long      %011001_001                     ' andn - Opcode with Z flag cleared
D_default     long      %011011_101                     ' xor  - Opcode with Z flag set

Zero          long      0
temp2         long      0

cmd_address   long      0
ARG_0         long      0
ARG_1         long      0
ARG_2         long      0

''---------------------Defined Aliased variables-----------------------------
temp                            ' Alias variable
temp1                           ' Alias variable
              long      0
_Index
_Ch1                            ' Alias variable
              long      0

_OverRide                       ' Alias variable
_PinValue                       ' Alias variable
_Ton                            ' Alias variable
_Ch2                            ' Alias variable    
              long      0

_Toff                           ' Alias variable
_Phase                          ' Alias variable
              long      0              

''------------------Defined PWM specific variables---------------------------
Mask          long      |<0,|<1,|<2,|<3,|<4,|<5,|<6,|<7 
              long      |<8,|<9,|<10,|<11,|<12,|<13
              long      |<14,|<15,|<16,|<17,|<18,|<19
              long      |<20,|<21,|<22,|<23,|<24,|<25
              long      |<26,|<27,|<28,|<29,|<30,|<31                            

t1            long      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
              long      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0

''----------------This Block of memory needs to stay intact-------------------
T_Off_Ch      long      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
              long      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
              
T_On__Ch      long      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
              long      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
''----------------------------------------------------------------------------              

Ch            long      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
              long      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0

CON
{{
┌──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────┐
│                                                   TERMS OF USE: MIT License                                                  │                                                            
├──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────┤
│Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation    │ 
│files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy,    │
│modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software│
│is furnished to do so, subject to the following conditions:                                                                   │
│                                                                                                                              │
│The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.│
│                                                                                                                              │
│THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE          │
│WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR         │
│COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,   │
│ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                         │
└──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────┘
}}                      
