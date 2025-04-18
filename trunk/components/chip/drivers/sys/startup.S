//start from __start, 
//(0)initialize vector table
//(1)initialize all registers
//(2)prepare initial reg values for user process
//(3)initialize supervisor mode stack pointer
//(4)construct ASID Table
//(5)prepare PTE entry for user process start virtual address
//(6)creat a mapping between VPN:0 and PFN:0 for kernel
//(7)set VBR register
//(8)enable EE and MMU
//(9)jump to the main procedure using jsri main

.text
.export vector_table
//.import VecTable
.align  10
vector_table:           //totally 256 entries
//  .long   __start
//  .rept   128
//  .long   __dummy
//  .endr

.long __start
.long MisalignedHandler
.long AccessErrHandler
.long DummyHandler
.long IllegalInstrHandler
.long PriviledgeVioHandler
.long DummyHandler
.long BreakPointHandler
.long UnrecExecpHandler
.long DummyHandler
.long DummyHandler
.long DummyHandler
.long DummyHandler
.long DummyHandler
.long DummyHandler
.long DummyHandler
.long Trap0Handler
.long Trap1Handler
.long Trap2Handler
.long Trap3Handler
.long DummyHandler
.long DummyHandler
.long DummyHandler
.long DummyHandler
.long DummyHandler
.long DummyHandler
.long DummyHandler
.long DummyHandler
.long DummyHandler
.long DummyHandler
.long DummyHandler
.long PendTrapHandler

/*
// External interrupts 
.long do_irq		//CORETHandler
.long do_irq		//SYSCONIntHandler
.long csp_ifc_irq_handler		//IFCIntHandler
.long do_irq		//ADCIntHandler		
.long do_irq		//EPT0IntHandler
.long DummyHandler//EPT0EMIntHandler
.long do_irq		//WWDTHandler
.long do_irq		//EXI0IntHandler
.long do_irq		//EXI1IntHandler
.long do_irq		//GPT0IntHandler
.long DummyHandler//GPT1IntHandler
.long DummyHandler
.long do_irq		//RTCIntHandler
.long do_irq		//UART0IntHandler
.long do_irq		//UART1IntHandler
.long do_irq		//UART2IntHandler//USARTIntHandler
.long DummyHandler
.long do_irq		//I2CIntHandler
.long DummyHandler
.long do_irq		//SPI0IntHandler
.long do_irq		//SIO0IntHandler
.long do_irq		//EXI2to3IntHandler
.long do_irq		//EXI4to9IntHandler
.long do_irq		//EXI10to13IntHandler
.long do_irq		//CNTAIntHandler
.long do_irq		//TKEYIntHandler
.long do_irq		//LPTIntHandler
.long DummyHandler//LEDIntHandler
.long do_irq		//BT0IntHandler
.long do_irq		//BT1IntHandler
.long DummyHandler//BT2IntHandler
.long DummyHandler//BT3IntHandler
*/

// External interrupts 
.long CORETHandler
.long SYSCONIntHandler
.long IFCIntHandler
.long ADCIntHandler
.long EPT0IntHandler
.long DummyHandler		//EPT0EMIntHandler
.long WWDTHandler
.long EXI0IntHandler
.long EXI1IntHandler
.long GPT0IntHandler
.long DummyHandler		//GPT1IntHandler
.long DummyHandler
.long RTCIntHandler
.long UART0IntHandler
.long UART1IntHandler
.long UART2IntHandler	//USARTIntHandler
.long DummyHandler
.long I2CIntHandler
.long DummyHandler
.long SPI0IntHandler
.long SIO0IntHandler
.long EXI2to3IntHandler
.long EXI4to9IntHandler
.long EXI10to15IntHandler
.long CNTAIntHandler
.long TKEYIntHandler
.long LPTIntHandler
.long DummyHandler		//LEDIntHandler
.long BT0IntHandler
.long BT1IntHandler
.long DummyHandler		//BT2IntHandler
.long DummyHandler		//BT3IntHandler



.text
.export __start
.long 0x00000000
.long 0x00000000
//  .long __start
__start:

	
  //initialize all registers
  movi r0, 0
  movi r1, 0
  movi r2, 0
  movi r3, 0
  movi r4, 0
  movi r5, 0
  movi r6, 0
  movi r7, 0
  //movi r8, 0
  //movi r9, 0
  //movi r10, 0
  //movi r11, 0
  //movi r12, 0
  //movi r13, 0
  //movi r14, 0
  //movi r15, 0

//set VBR
  lrw r2, vector_table
  mtcr r2, cr<1,0>

//enable EE bit of psr
  mfcr r2, cr<0,0>
  bseti r2, r2, 8
  mtcr r2, cr<0,0>

////set rom access delay
//  lrw     r1, 0xe00000
//  lrw     r2, 0x7
//  st.w    r2, (r1,0x0)

////enable cache
//  lrw     r1, 0xe000f000
//  movi    r2, 0x2
//  st.w    r2, (r1,0x0)
//  lrw     r2, 0x29
//  st.w    r2, (r1,0x4)
//  movi    r2, 0x1
//  st.w    r2, (r1,0x0)

//disable power peak 
  lrw     r1, 0xe000ef90
  movi    r2, 0x0
  st.w    r2, (r1, 0x0)



//initialize kernel stack
  lrw  r7, __kernel_stack
  mov r14,r7
  subi r6,r7,0x4

  //lrw  r3, 0x40
  lrw  r3, 0x04
	
  subu r4, r7, r3
  lrw  r5, 0x0
INIT_KERLE_STACK:
  addi r4, 0x4
  st.w r5, (r4)
  //cmphs r7, r4
  cmphs r6, r4
  bt  INIT_KERLE_STACK
        
__to_main:
  lrw r0,__main
  jsr r0
  mov r0, r0
  mov r0, r0


  
  lrw r15, __exit
  lrw r0,main
  jmp r0
  mov r0, r0
  mov r0, r0
  mov r0, r0
  mov r0, r0
  mov r0, r0

.export __exit
__exit:

  lrw r4, 0x20003000
  //lrw r5, 0x0
  mov r5, r0
  st.w r5, (r4)

  mfcr r1, cr<0,0>
  lrw  r1, 0xFFFF
  mtcr r1, cr<11,0>
  lrw     r1, 0xFFF
  movi    r0, 0x0
  st      r1, (r0)

.export __fail
__fail:
  lrw  r1, 0xEEEE
  mtcr r1, cr<11,0>
  lrw     r1, 0xEEE
  movi    r0, 0x0
  st      r1, (r0)

__dummy:
  br      __fail

.export DummyHandler
DummyHandler:
  br      __fail


.data
.align  10
.long __start
