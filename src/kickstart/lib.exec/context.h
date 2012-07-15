#ifndef context_h
#define context_h

extern int context_save_arch(StackContext *ctx) __attribute__((returns_twice));
extern void context_restore_arch(StackContext *ctx) __attribute__((noreturn));
#define context_save(ctx)  context_save_arch(ctx)

__attribute__((no_instrument_function)) static inline void context_restore(StackContext *ctx)
{
	context_restore_arch(ctx);
}


#endif