struct egistec_data {
	dev_t devt;
	spinlock_t spi_lock;
	struct spi_device  *spi;
	struct platform_device *pd;
	struct list_head device_entry;
	struct notifier_block notifier;

	/* buffer is NULL unless this device is open (users > 0) */
	struct mutex buf_lock;
	unsigned users;
	u8 *buffer;

	unsigned int irqPin;	    /* interrupt GPIO pin number */
	unsigned int rstPin; 	    /* Reset GPIO pin number */

	unsigned int vdd_18v_Pin;	/* Reset GPIO pin number */
	unsigned int vcc_33v_Pin;	/* Reset GPIO pin number */

    struct input_dev	*input_dev;
	bool property_navigation_enable;

#ifdef CONFIG_OF
	struct pinctrl *pinctrl_gpios;
	struct pinctrl_state *pins_irq;
	struct pinctrl_state *pins_miso_spi, *pins_miso_pullhigh, *pins_miso_pulllow;
	struct pinctrl_state *pins_reset_high, *pins_reset_low;
#endif	
	
	
};
extern void uinput_egis_init(struct etspi_data *egistec);
extern void uinput_egis_destroy(struct etspi_data *egistec);
extern void sysfs_egis_init(struct etspi_data *egistec);
extern void sysfs_egis_destroy(struct etspi_data *egistec);