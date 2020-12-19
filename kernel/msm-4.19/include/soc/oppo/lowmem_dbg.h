#ifndef __LOWMEM_DBG_H
#define __LOWMEM_DBG_H

#ifdef CONFIG_OPLUS_FEATURE_LOWMEM_DBG

void oppo_lowmem_dbg(void );
inline int oppo_is_dma_buf_file(struct file *file);

#else

inline void oppo_lowmem_dbg(void )
{
}

inline int oppo_is_dma_buf_file(struct file *file)
{
	return 0;
}

#endif /* VENDOR_EDIT && CONFIG_OPPO_LOWMEM_DBG */

#endif /* __LOWMEM_DBG_H */
