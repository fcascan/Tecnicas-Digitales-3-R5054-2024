#ifndef __SCHEDULER_LIB_H

#define __SCHEDULER_LIB_H

	#include <stdint.h>
	#include "./mmu_tools.h"
	
	extern const void _T1_READING_AREA;
	extern const void _T2_READING_AREA;
	extern const void _T3_READING_AREA;

	extern const void	_T1_STACK_INIT;
	extern const void	__irq_stack_top__t1__;
	extern const void	__fiq_stack_top__t1__;
	extern const void	__svc_stack_top__t1__;
	extern const void 	__abt_stack_top__t1__;
	extern const void 	__und_stack_top__t1__;
	extern const void	__sys_stack_top__t1__;
	
	extern const void 	_T2_STACK_INIT;
	extern const void	__irq_stack_top__t2__;
	extern const void	__fiq_stack_top__t2__;
	extern const void	__svc_stack_top__t2__;
	extern const void 	__abt_stack_top__t2__;
	extern const void 	__und_stack_top__t2__;
	extern const void 	__sys_stack_top__t2__;

	extern const void 	_PUBLIC_STACK_INIT;
	extern const void 	__irq_stack_top__;
	extern const void 	__fiq_stack_top__;
	extern const void 	__svc_stack_top__;
	extern const void 	__abt_stack_top__;
	extern const void 	__und_stack_top__;
	extern const void 	__sys_stack_top__;

	extern const void 	_SYSTABLES_PHYSICAL_T1;
	extern const void	_SYSTABLES_PHYSICAL_T2;
	extern const void 	_SYSTABLES_PHYSICAL_T3; 

	#define IRQ_MODE    0x00000012
	#define FIQ_MODE	0x00000011
	#define SVC_MODE	0x00000013
	#define ABT_MODE    0x00000017
	#define UND_MODE    0x0000001b
	#define SYS_MODE    0x0000001f
	#define I_BIT       0x00000080 		//Con '1' deshabilito las interrupcciones por IRQ
	#define F_BIT       0x00000060 		//Con '1' deshabilito las interrupcciones por FIQ

	#define RUNING		0
	#define READY 		1
	#define WAITING 	2

	#define NUM_TASKS 	3
	#define NUM_REGS	13
	#define NUM_MODOS	6

	#define TASK1SLICE  1
	#define TASK2SLICE  1
	#define TASK3SLICE  8

	void tasks_init		(void);
	void salvar_contexto(void);
	void cargar_contexto(void);
	void scheduler		(void);

	void task1_f		(void);
	void task2_f		(void);
	void task_idle_f	(void);

	typedef struct{
        uint32_t    stack_pointer_irq;
		uint32_t	stack_pointer_fiq;
		uint32_t	stack_pointer_svc;
		uint32_t	stack_pointer_abt;
		uint32_t	stack_pointer_und;
		uint32_t	stack_pointer_sys;

		uint32_t	ptr_function;

        uint32_t    current_ttbr0;	//4
		uint32_t    time_slice;
        uint32_t    remaining_time_slice;
        uint32_t    state;
		uint32_t 	ident;
	
    } Task;

#endif // __SCHEDULER_LIB_H
