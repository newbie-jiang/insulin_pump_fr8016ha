#include <stdint.h>

#include "jump_table.h"
#include "low_power.h"
//#include "os_mem.h"

#include "driver_system.h"
#include "driver_uart.h"
#include "driver_pmu.h"
#include "driver_flash.h"
#include "flash_usage_config.h"

#define UPDATE_CHANEL_MAP_EN              0
#define PATCH_MAP_BASE_ADDR             0x20002000
#define ENABLE_CHECK_CONN_IND_RSSI_PATCH  0
/* Suppress warning messages */
#if defined(__CC_ARM)
// Suppress warning message: extended constant initialiser used
#pragma diag_suppress 1296
#elif defined(__ICCARM__)
#elif defined(__GNUC__)
#endif

struct patch_element_t
{
    uint32_t patch_pc;
    void *replace_function;
};

#ifdef USER_MEM_API_ENABLE
extern void *ke_malloc_user(uint32_t size, uint8_t type);
extern void ke_free_user(void* mem_ptr);
#endif

#if ENABLE_CHECK_CONN_IND_RSSI_PATCH
bool lld_adv_pkt_rx_patch(uint8_t act_id);
#endif
void pmu_calibration_start(uint8_t type, uint16_t counter);
void pmu_calibration_stop(void);
uint16_t pmu_get_isr_state(void);

void enable_cache(uint8_t invalid_ram);
void disable_cache(void);

int llc_patch_1(void);
void platform_reset_patch(uint32_t error);
void con_max_lat_calc_patch(void);
uint8_t lld_con_llcp_tx_patch(uint8_t, void *);
uint8_t lld_con_data_tx_patch(uint8_t, void *);
void vPortSuppressTicksAndSleep_patch( uint32_t xExpectedIdleTime );
void adv_tx_free_imp(uint16_t buf);
void llm_ch_map_update_patch(void);
uint8_t lld_adv_start_patch(uint8_t act_id, void * params);
void lld_adv_frm_isr_patch(uint8_t act_id, uint32_t timestamp, bool abort);
void lld_con_sched_patch(uint8_t link_id, uint32_t clock, bool sync);
/*
 * keil debug breakpoint will take place FPB entry at the beginning of patch table with increasing
 * direction, so use patch entry point start at the end of patch table with decreasing direction.
 *
 * TAKE CARE !!!!!!!!!!!!!!!!!!!!!!!!!! the patch breakpoint register[0~5, 8~13] may be cleared by
 * KEIL unexpectly when debugging this IDE.
 */
struct patch_element_t patch_elements[] =
{
    [15] = {
        .patch_pc = 0x000029e6,
        .replace_function = adv_tx_free_imp,
    },
    [14] = {
        #if ENABLE_CHECK_CONN_IND_RSSI_PATCH
        .patch_pc = 0x0000f9d2,
        .replace_function = lld_adv_pkt_rx_patch,
        #else
        .patch_pc = 0x00000001,
        #endif
    },
    /*
     * (con_par->instant_proc.type == INSTANT_PROC_NO_PROC), this judgement will keep slave latency stopped once
     * receiving connection parameter update, channel update until the new parameters is applied. This processing
     * will make the system working with higher power consumption in deep sleep mode.
     */
    [13] = {
        .patch_pc = 0x00011f06,
        .replace_function = lld_con_sched_patch,
    },
    [12] = {
        //.patch_pc = 0x0000849c, // check adv interval in set adv parameter cmd handler
#if defined(CFG_HCI_CODE) || defined(CFG_FT_CODE)
        .patch_pc = 0x00000001,
#else
        .patch_pc = 0x0001ca6a,
        .replace_function = vPortSuppressTicksAndSleep_patch,
#endif
    },
#ifdef USER_MEM_API_ENABLE
    [11] = {
        .patch_pc = 0x0000a4ce,
        .replace_function = ke_malloc_user,
    },
    [10] = {
        .patch_pc = 0x0000a3a6,
        .replace_function = ke_free_user,
    },
#else
#if 1
    [11] = {
        .patch_pc = 0x0000849c,
    },
    [10] = {
        .patch_pc = 0x00000001,
    },
#else
    [10] = {
        .patch_pc = 0x0001126a, 
        .replace_function = lld_con_data_tx_patch,
    },
    [11] = {
        .patch_pc = 0x000118f6,
        .replace_function = lld_con_llcp_tx_patch,
    },
#endif
#endif
#if  UPDATE_CHANEL_MAP_EN
    [9] = {
        .patch_pc = 0x00018c32, 
        .replace_function = llm_ch_map_update_patch,
    },
#else
    [9] = {
        .patch_pc = 0x00012f30, 
    },    
#endif
    [8] = {
        .patch_pc = 0x00012410, // replace em_ble_rxmaxbuf_set(cs_idx, LE_MAX_OCTETS) in lld_con_start
    },
    [7] = {
        .patch_pc = 0x0001e500, // 0x0001e500,
    },
    [6] = {
#if defined(CFG_HCI_CODE) || defined(CFG_FT_CODE)
        .patch_pc = 0x00000001,
#else
        .patch_pc = 0x0001e808,
#endif
    },

    // this position has been taken in private mesh application, search FPB_CompSet for details
    [5] = {
        .patch_pc = 0x00000001,
    },

    [4] = {
        .patch_pc = 0x00004aae,
        .replace_function = frspim_rd,
    },
    [3] = {
        .patch_pc = 0x00004b02,
        .replace_function = frspim_wr,
    },
    [2] = {
        .patch_pc = 0x000195d2,
        .replace_function = platform_reset_patch,
    },
    [1] = {
        .patch_pc = 0x00019760, // replace pmu_get_rc_clk implement
    },
    [0] = {
        //.patch_pc = 0x000119c8, // 3832: SUBS R0, #50 1fc0: SUBS R0, R0, #1
        .patch_pc = 0x0001199a,
        .replace_function = con_max_lat_calc_patch,
    },
};

__attribute__((aligned(64))) uint32_t patch_map[16] =
{
    0xBF00DF00,  //0
    0x2000B57C,
    0xBF00DF02,
    0xBF00DF03,
    0xBF00DF04,
    0x46080141,
#if defined(CFG_HCI_CODE) || defined(CFG_FT_CODE)
    0x00000000,
#else
    (uint32_t)llc_patch_1,
#endif
    0x0001F8FD,
    0xEB01201B, // 8
#if  UPDATE_CHANEL_MAP_EN
    0xBF00DF09,
#else
    0xBF00BF00,
#endif
    0xBF00DF0a, //10
    0xe002d87f,
    0xBF00DF0c, // 0xe002d87f, // 12
    0xBF00DF0D,//0xDF0CBF00,
    0xBF00DF0E,
    0xBF00DF0F,
};

static uint32_t lld_regs_buffer[6];

uint8_t buck_value_org, dldo_value_org;

__attribute__((section("ram_code"))) void patch_init(void)
{
    uint8_t patch_num = sizeof(patch_elements) / sizeof(patch_elements[0]);
    uint8_t i;

    FPB_SetRemap((uint32_t)&patch_map[0]);
    //patch_map[8] = 0xEB012000 | __jump_table.max_rx_buffer_size;

    for(i=patch_num; i!=0; )
    {
        i--;
        if(patch_elements[i].patch_pc)
        {
            //if(i < 5  )  //if you want to use jlink to do debug, not enable patch4
                //;//FPB_CompUnset(i);
            //else
                FPB_CompSet(patch_elements[i].patch_pc, 0x00, i);
        }
        else
        {
            break;
        }
    }

    FPB_Enable();
#ifdef FLASH_PROTECT
//    *(volatile uint32_t *)0x500B0000|= (1<<14);
#endif
}

__attribute__((section("ram_code"))) void patch_set_entry(void)
{
    patch_elements[9].patch_pc = 0x00002b70; 
    patch_elements[8].patch_pc = 0x00002be4;
    patch_map[9] = 0x4B2A0051;
    patch_map[8] = 0x22087933;    
}
__attribute__((section("ram_code"))) void patch_reset_entry(void)
{
    #if UPDATE_CHANEL_MAP_EN
    patch_elements[9].patch_pc = 0x00018c32; 
    patch_elements[9].replace_function = llm_ch_map_update_patch;
    patch_map[9] = 0xBF00DF09;
    #else
    patch_elements[9].patch_pc = 0x00012f30;
    patch_map[9] = 0xBF00BF00;
    #endif
    patch_elements[8].patch_pc = 0x00012410;
    patch_map[8] = 0xEB012000 | __jump_table.max_rx_buffer_size;
}

extern uint32_t record_lr;
__attribute__((section("ram_code"))) uint32_t svc_exception_handler_ram(uint32_t svc_pc,uint32_t lr)
{
    uint8_t patch_num = sizeof(patch_elements) / sizeof(patch_elements[0]);
    uint8_t i;
#ifdef USER_MEM_API_ENABLE
    record_lr = lr;
#endif
    for(i=0; i<patch_num; i++)
    {
        if(svc_pc == patch_elements[i].patch_pc)
        {
            break;
        }
    }

    if(i != patch_num)
    {
        return (uint32_t)(patch_elements[i].replace_function);
    }

    return 0;
}

__attribute__((section("ram_code"))) void low_power_save_entry_imp(uint8_t type)
{
    if(type == LOW_POWER_SAVE_ENTRY_BASEBAND)
    {
        lld_regs_buffer[0] = *(volatile uint32_t *)0x40000130;
        lld_regs_buffer[1] = *(volatile uint32_t *)0x40000134;
        lld_regs_buffer[2] = *(volatile uint32_t *)0x40000138;
        lld_regs_buffer[3] = *(volatile uint32_t *)0x4000013c;
        lld_regs_buffer[4] = *(volatile uint32_t *)0x40000170;
        lld_regs_buffer[5] = *(volatile uint32_t *)0x40000174;
        
        buck_value_org = ool_read(PMU_REG_BUCK_CTRL1);
        dldo_value_org = ool_read(PMU_REG_DLDO_CTRL);
        /* set BUCK voltage to higher level */
        ool_write(PMU_REG_BUCK_CTRL1, 0x45);
        /* set DLDO voltage to higher level */
        ool_write(PMU_REG_DLDO_CTRL, (0x72 & 0xf8) | (dldo_value_org & 0x07));

        /* set cs pin of internal flash in high level */
        //pmu_gpio_set_dir(GPIO_PORT_B, GPIO_BIT_0, GPIO_DIR_OUT);

        /* power off flash */
        ool_write(PMU_REG_FLASH_POR_CTRL, ool_read(PMU_REG_FLASH_POR_CTRL) & 0xfd);

        ool_write(PMU_REG_OSC_DRV_CTRL, 0x3f);

        pmu_calibration_stop();
    }
}


__attribute__((section("ram_code"))) void low_power_restore_entry_imp(uint8_t type)
{
    if(type == LOW_POWER_RESTORE_ENTRY_BEGINNING)
    {
        SCB->VTOR = 0x20000000; //set exception vector offset to RAM space

        /* restore BUCK voltage */
        ool_write(PMU_REG_BUCK_CTRL1, buck_value_org);
        /* restore DLDO voltage */
        ool_write(PMU_REG_DLDO_CTRL, dldo_value_org);

        /* power on flash */
        ool_write(PMU_REG_FLASH_POR_CTRL, ool_read(PMU_REG_FLASH_POR_CTRL) | 0x02);

        ool_write(PMU_REG_OSC_DRV_CTRL, 0x20);
    }
    else if(type == LOW_POWER_RESTORE_ENTRY_BASEBAND)
    {
        *(volatile uint32_t *)0x40000130 = lld_regs_buffer[0];
        *(volatile uint32_t *)0x40000134 = lld_regs_buffer[1];
        *(volatile uint32_t *)0x40000138 = lld_regs_buffer[2];
        *(volatile uint32_t *)0x4000013c = lld_regs_buffer[3];
        *(volatile uint32_t *)0x40000170 = lld_regs_buffer[4];
        *(volatile uint32_t *)0x40000174 = lld_regs_buffer[5];
        
#ifndef KEEP_CACHE_SRAM_RETENTION
        // manul enable the cache and invalidating the SRAM
        enable_cache(true);
#else
        // manul enable the cache without invalidating the SRAM
        enable_cache(false);
#endif
    }
    else if(type == LOW_POWER_RESTORE_ENTRY_DRIVER)
    {
        /* handle the cs control to QSPI controller */
        //pmu_gpio_set_dir(GPIO_PORT_B, GPIO_BIT_0, GPIO_DIR_IN);
#ifndef CFG_HCI_CODE
        extern uint8_t pmu_clk_src; // 0: internal RC, 1: external 32768
        if(pmu_clk_src == 0) {
            pmu_calibration_start(PMU_CALI_SRC_LP_RC, __jump_table.lp_clk_calib_cnt);
        }
#endif        

        if((ool_read(PMU_REG_KEYSCAN_CTRL) & PMU_KEYSCAN_EN)
           && ((pmu_get_isr_state() & PMU_ISR_KEYSCAN_STATE) == 0))
        {
            ool_write(PMU_REG_RST_CTRL, ool_read(PMU_REG_RST_CTRL) &(~PMU_RST_KEYSCAN));
            ool_write(PMU_REG_RST_CTRL, ool_read(PMU_REG_RST_CTRL) | PMU_RST_KEYSCAN);
        }
        patch_reset_entry();
        patch_init();
    }
}


__attribute__((section("ram_code"))) __attribute__((weak)) void user_entry_before_sleep_imp(void)
{
    /* turn off flash power */
    //ool_write(PMU_REG_FLASH_POR_CTRL, ool_read(PMU_REG_FLASH_POR_CTRL) & 0xfd);

    //uart_putc_noint_no_wait(UART0, 's');
}

__attribute__((section("ram_code"))) __attribute__((weak)) void user_entry_after_sleep_imp(void)
{
    /* turn on flash power */
    //ool_write(PMU_REG_FLASH_POR_CTRL, ool_read(PMU_REG_FLASH_POR_CTRL) | 0x02);

    //uart_putc_noint_no_wait(UART0, 'w');
    //pmu_calibration_start(PMU_CALI_SRC_LP_RC, __jump_table.lp_clk_calib_cnt);
}

__attribute__((weak)) __attribute__((section("ram_code"))) uint8_t app_get_ota_state(void)
{
    return 0;
}

__attribute__((section("ram_code"))) void flash_write(uint32_t offset, uint32_t length, uint8_t * buffer)
{
    uint32_t last_space;
    void (*flash_write_)(uint32_t offset, uint32_t length, uint8_t * buffer) = (void (*)(uint32_t, uint32_t, uint8_t *))0x00004899;

    GLOBAL_INT_DISABLE();
    disable_cache();
#ifdef FLASH_PROTECT
//    *(volatile uint32_t *)0x500B0000 &= (~(1<<14));
    flash_protect_disable(0);
#endif	
    if(((offset >= 2*__jump_table.image_size) 
            && (offset < FLASH_MAX_SIZE)) || (app_get_ota_state()))
    {
        while(length) {
            if(offset < 1*1024*1024) {
                last_space = 1*1024*1024-offset;
                if(last_space >= length) {
                    last_space = length;
                }
                
                flash_write_(offset, last_space, buffer);
                offset += last_space;
                buffer += last_space;
                length -= last_space;
            }
            else {
                uint32_t base_addr = offset & (~(1*1024*1024-1));
                uint32_t internal_offset = offset & (1*1024*1024-1);
                uint32_t current_remap_address, remap_size;
                current_remap_address = system_regs->remap_virtual_addr;
                remap_size = system_regs->remap_length;

                system_regs->remap_virtual_addr = 0;
                system_regs->remap_length = 0;
                *(volatile uint32_t *)0x500b0024 = 0x01000000+base_addr;
                last_space = 1*1024*1024-internal_offset;
                if(last_space >= length) {
                    last_space = length;
                }

                flash_write_(internal_offset, last_space, buffer);
                offset += last_space;
                buffer += last_space;
                length -= last_space;
                
                *(volatile uint32_t *)0x500b0024 = 0x01000000;
                system_regs->remap_virtual_addr = current_remap_address;
                system_regs->remap_length = remap_size;
            }
        }
    }
#ifdef FLASH_PROTECT
    flash_protect_enable(0);
//    *(volatile uint32_t *)0x500B0000 |= (1<<14);
#endif	
    enable_cache(true);
    GLOBAL_INT_RESTORE();
}

__attribute__((section("ram_code"))) void flash_erase(uint32_t offset, uint32_t length)
{
    void (*flash_erase_)(uint32_t offset, uint32_t length) = (void (*)(uint32_t, uint32_t))0x00004775;

    GLOBAL_INT_DISABLE();
    disable_cache();
#ifdef FLASH_PROTECT
//    *(volatile uint32_t *)0x500B0000 &= (~(1<<14));
    flash_protect_disable(0);
#endif	
    if(((offset >= 2*__jump_table.image_size) 
            && (offset < FLASH_MAX_SIZE)) || (app_get_ota_state()))
        flash_erase_(offset, length);
#ifdef FLASH_PROTECT
    flash_protect_enable(0);
//    *(volatile uint32_t *)0x500B0000 |= (1<<14);
#endif	
    enable_cache(true);
    GLOBAL_INT_RESTORE();
}

__attribute__((section("ram_code"))) void flash_read(uint32_t offset, uint32_t length, uint8_t *buffer)
{
    uint32_t last_space;
    void (*flash_read_)(uint32_t offset, uint32_t length, uint8_t *buffer) = (void (*)(uint32_t, uint32_t, uint8_t *))0x00004809;
    
    if(offset >= FLASH_MAX_SIZE)
        return;
    while(length) {
        if(offset < 1*1024*1024) {
            last_space = 1*1024*1024-offset;
            if(last_space >= length) {
                last_space = length;
            }
            
            flash_read_(offset, last_space, buffer);
            offset += last_space;
            buffer += last_space;
            length -= last_space;
        }
        else {
            uint32_t base_addr = offset & (~(1*1024*1024-1));
            uint32_t internal_offset = offset & (1*1024*1024-1);
            uint32_t current_remap_address, remap_size;
            current_remap_address = system_regs->remap_virtual_addr;
            remap_size = system_regs->remap_length;

            GLOBAL_INT_DISABLE();
            disable_cache();
            system_regs->remap_virtual_addr = 0;
            system_regs->remap_length = 0;
            *(volatile uint32_t *)0x500b0024 = 0x01000000+base_addr;
            last_space = 1*1024*1024-internal_offset;
            if(last_space >= length) {
                last_space = length;
            }

            flash_read_(internal_offset, last_space, buffer);
            offset += last_space;
            buffer += last_space;
            length -= last_space;
            
            *(volatile uint32_t *)0x500b0024 = 0x01000000;
            system_regs->remap_virtual_addr = current_remap_address;
            system_regs->remap_length = remap_size;
            enable_cache(true);
            GLOBAL_INT_RESTORE();
        }
    }
}

