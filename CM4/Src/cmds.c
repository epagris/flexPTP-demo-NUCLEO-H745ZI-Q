#include "cmds.h"
#include "flexptp/task_ptp.h"

#include <stdlib.h>

#include <FreeRTOS.h>
#include <cmsis_os2.h>

#include <cliutils/cli.h>
#include <standard_output/standard_output.h>

#include <EthDrv/phy_drv/phy_common.h>

#include <etherlib/etherlib.h>

// ---------------------------------

CMD_FUNCTION(os_info) {
    osVersion_t kVersion;
    char kId[32];
    kId[sizeof(kId) - 1] = 0;
    osKernelGetInfo(&kVersion, kId, sizeof(kId) - 1);
    MSG("OS: %s\n Kernel version: %d\n API version: %d\n", kId, kVersion.kernel, kVersion.api);

    HeapStats_t stats;
    vPortGetHeapStats(&stats);
    MSG("Free OS memory: %u bytes\n", stats.xAvailableHeapSpaceInBytes);

    return 0;
}

CMD_FUNCTION(phy_info) {
    phy_print_full_name();
    return 0;
}

#ifdef ETH_ETHERLIB

CMD_FUNCTION(print_ip) {
    MSGraw("IP: " ANSI_COLOR_BYELLOW);
    PRINT_IPv4(E.ethIntf->ip);
    MSGraw(ANSI_COLOR_RESET "\r\n");
    return 0;
}

CMD_FUNCTION(eth_tmr) {
    timer_report(E.tmr);
    return 0;
}

CMD_FUNCTION(eth_conns) {
    packsieve_report_full(&E.ethIntf->sieve);
    return 0;
}

CMD_FUNCTION(eth_mem) {
    mp_report(E.mp);
    return 0;
}

CMD_FUNCTION(eth_arpc) {
    arpc_dump(E.ethIntf->arpc);
    return 0;
}

CMD_FUNCTION(eth_cbdt) {
    cbdt_report(E.cbdt);
    return 0;
}

#endif

CMD_FUNCTION(start_flexptp) {
    if (!task_ptp_is_operating()) {
        MSG("Starting flexPTP...\n\n");
        reg_task_ptp();
    } else {
        MSG("Nice try, but no. :)\n"
            "flexptp is already up and running!\n");
    }

    return 0;
}



// ---------------------------------

void cmd_init() {
    cli_register_command("osinfo \t\t\tPrint OS-related information", 1, 0, os_info);
    cli_register_command("phyinfo \t\t\tPrint Ethernet PHY information", 1, 0, phy_info);
    cli_register_command("flexptp \t\t\tStart flexPTP daemon", 1, 0, start_flexptp);

#ifdef ETH_ETHERLIB
    cli_register_command("ip \t\t\tPrint IP-address", 1, 0, print_ip);
    cli_register_command("eth tmr \t\t\tPrint EtherLib timer report", 2, 0, eth_tmr);
    cli_register_command("eth conns \t\t\tPrint active connections", 2, 0, eth_conns);
    cli_register_command("eth mem \t\t\tPrint EtherLib memory pool state", 2, 0, eth_mem);
    cli_register_command("eth arpc \t\t\tPrint EtherLib ARP cache", 2, 0, eth_arpc);
    cli_register_command("eth cbdt \t\t\tPrint EtherLib CBD table", 2, 0, eth_cbdt);
#endif
}