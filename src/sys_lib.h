#ifndef __SYS_LIB_H__
#define __SYS_LIB_H__

typedef void (*lib_fun_t)(void);

typedef struct{
	void * fun;
	const char * name;
}sys_lib_t;

#define SYSLIB_EXPORT(name,sectionName) \
const sys_lib_t sys_lib_##name __attribute__((section(sectionName)))={ \
	(void*)&name,	\
	#name	\
};

#endif