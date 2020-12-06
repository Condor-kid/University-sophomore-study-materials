
#include <stdint.h>
#include "stringio.h"
#define BUFLEN 16

extern void clearScreen();
extern void powerOff();
extern void reBoot();
extern uint8_t getUserProgNum();
extern char* getUserProgName(uint16_t progid);
extern uint16_t getUserProgSize(uint16_t progid);
extern uint8_t getUserProgCylinder(uint16_t progid);
extern uint8_t getUserProgHead(uint16_t progid);
extern uint8_t getUserProgSector(uint16_t progid);
extern uint16_t getUserProgAddrSeg(uint16_t progid);
extern uint16_t getUserProgAddrOff(uint16_t progid);
extern void loadAndRun(uint8_t cylinder, uint8_t head, uint8_t sector, uint16_t len, uint16_t seg, uint16_t offset);
extern uint8_t getDateYear();
extern uint8_t getDateMonth();
extern uint8_t getDateDay();
extern uint8_t getDateHour();
extern uint8_t getDateMinute();
extern uint8_t getDateSecond();
extern uint8_t bcd2decimal(uint8_t bcd);
extern void loadProcessMem(uint8_t cylinder, uint8_t head, uint8_t sector, uint16_t len, uint16_t seg, uint16_t offset, int progid_to_run);

extern uint16_t timer_flag;

void Delay()
{
	int i = 0;
	int j = 0;
	for( i=0;i<10000;i++ )
		for( j=0;j<10000;j++ )
		{
			j++;
			j--;
		}
}

/* 系统启动界面 */
void startUp() {
    clearScreen();
    const char* title = "Condor_OS";
    const char* subtitle = "18340066_hwz";
    const char* hint = "OS has been loaded successfully. Press ENTER to start shell.";
    printInPos(title, strlen(title), 5, 35);
    printInPos(subtitle, strlen(subtitle), 6, 33);
    printInPos(hint, strlen(hint), 15, 8);
}

/* 打印系统提示符 */
void promptString() {
    const char* prompt_string = "Condor_OS#";
    print_c(prompt_string, 0x0A);  // 绿色
}

/* 显示帮助信息 */
void showHelp() {
    const char *help_msg = 
    "Shell for Condor_OS, - on x86 PC\r\n"
    "This is a shell which is used for Condor_OS. These shell commands are defined internally. Use `help` to see the list.\r\n"
    "\r\n"
    "    help - show information about builtin commands\r\n"
    "    clear - clear the terminal screen\r\n"
    "    list - show a list of user programmes and their ProgIDs\r\n"
    "    bat <ProgIDs> - run user programmes in batch method, e.g. `bat 2 1 3`\r\n"
    "    run <ProgIDs> - create processes and run programmes simultaneously\r\n"
    "    poweroff - force power-off the machine\r\n"
    "    reboot - reboot the machine\r\n"
    "    date - display the current date and time\r\n"
    ;
    print(help_msg);
}

/* 显示用户程序信息 */
void listUserProg() {
    const char* hint = "You can use `run <ProgID>` to run a user programme.\r\n";
    const char* list_head =
        "ProgID  - Program Name  -  Size  -  Addr - Cylinder - Head - Sector\r\n";
    const char* separator = "  -  ";
    print(hint);
    print(list_head);
    uint16_t prog_num = getUserProgNum();  // 获取用户程序数量
    for(int i = 1; i <= prog_num; i++) {
        print(itoa(i, 10)); print(separator);  // 打印ProgID
        print(getUserProgName(i));
        for(int j = 0, len = 16-strlen(getUserProgName(i)); j < len; j++) {
            putchar(' ');
        }
        print(separator);  // 打印用户程序名
        print(itoa(getUserProgSize(i), 10)); print(separator);  // 打印用户程序大小
        putchar('0'); putchar('x');  // 在十六进制数前显示0x
        print(itoa(getUserProgAddrSeg(i), 16)); print(separator);  // 打印用户程序内存地址
        print(itoa(getUserProgCylinder(i), 10)); print(separator);  // 打印用户程序存放的柱面号
        print(itoa(getUserProgHead(i), 10)); print(separator);  // 打印用户程序存放的磁头号
        print(itoa(getUserProgSector(i), 10));  // 打印用户程序存放的起始扇区
        NEWLINE;
    }
}

/* 显示日期时间 */
void showDateTime() {
    putchar('2'); putchar('0');
    print(itoa(bcd2decimal(getDateYear()), 10)); putchar('-');
    print(itoa(bcd2decimal(getDateMonth()), 10)); putchar('-');
    print(itoa(bcd2decimal(getDateDay()), 10)); putchar(' ');
    print(itoa(bcd2decimal(getDateHour()), 10)); putchar(':');
    print(itoa(bcd2decimal(getDateMinute()), 10)); putchar(':');
    print(itoa(bcd2decimal(getDateSecond()), 10));
    NEWLINE;
}

/* 批处理执行程序 */
void batch(char* cmdstr) {
    char progids[BUFLEN+1];
    getAfterFirstWord(cmdstr, progids);  // 获取run后的参数列表
    uint8_t isvalid = 1;  // 参数有效标志位
    for(int i = 0; progids[i]; i++) {  // 判断参数是有效的
        if(!isnum(progids[i]) && progids[i]!=' ') {  // 既不是数字又不是空格，无效参数
            isvalid = 0;
            break;
        }
        if(isnum(progids[i]) && progids[i]-'0'>getUserProgNum()) {
            isvalid = 0;
            break;
        }
    }
    if(isvalid) {  // 参数有效，则按顺序执行指定的用户程序
        int i = 0;
        for(int i = 0; progids[i] != '\0'; i++) {
            if(isnum(progids[i])) {  // 是数字（不是空格）
                int progid_to_run = progids[i] - '0';  // 要运行的用户程序ProgID
                loadAndRun(getUserProgCylinder(progid_to_run), getUserProgHead(progid_to_run), getUserProgSector(progid_to_run), getUserProgSize(progid_to_run)/512, getUserProgAddrSeg(progid_to_run), getUserProgAddrOff(progid_to_run));
                clearScreen();
            }
        }
        const char* hint = "All programmes have been executed successfully as you wish.\r\n";
        print(hint);
    }
    else {  // 参数无效，报错，不执行任何用户程序
        const char* error_msg = "Invalid arguments. ProgIDs must be numbers and less than or equal to ";
        print(error_msg);
        print(itoa(getUserProgNum(), 10));
        putchar('.');
        NEWLINE;
    }
}

void multiProcessing(char* cmdstr) {
    char progids[BUFLEN+1];
    getAfterFirstWord(cmdstr, progids);  // 获取run后的参数列表
    uint8_t isvalid = 1;  // 参数有效标志位
    if(progids[0] == '\0') { isvalid = 0; }
    for(int i = 0; progids[i]; i++) {  // 判断参数是有效的
        if(!isnum(progids[i]) && progids[i]!=' ') {  // 既不是数字又不是空格，无效参数
            isvalid = 0;
            break;
        }
        if(isnum(progids[i]) && progids[i]-'0'>4) {  // 只能运行前4个用户程序
            isvalid = 0;
            break;
        }
    }
    if(isvalid) {  // 参数有效，则按顺序执行指定的用户程序
        int i = 0;
        for(int i = 0; progids[i] != '\0'; i++) {
            if(isnum(progids[i])) {  // 是数字（不是空格）
                int progid_to_run = progids[i] - '0';  // 要运行的用户程序ProgID
                loadProcessMem(getUserProgCylinder(progid_to_run), getUserProgHead(progid_to_run), getUserProgSector(progid_to_run), getUserProgSize(progid_to_run)/512, getUserProgAddrSeg(progid_to_run), getUserProgAddrOff(progid_to_run), progid_to_run);
            }
        }
        timer_flag = 1;  // 允许时钟中断处理多进程
        Delay();
        timer_flag = 0;  // 禁止时钟中断处理多进程
        clearScreen();
        const char* hint = "All processes have been killed.\r\n";
        print(hint);
    }
    else {  // 参数无效，报错，不执行任何用户程序
        const char* error_msg = "Invalid arguments. ProgIDs must be numbers and less than or equal to 4.";
        print(error_msg);
        NEWLINE;
    }
}

/* 操作系统shell */
void shell() {
    clearScreen();
    showHelp();
    char cmdstr[BUFLEN+1] = {0};  // 用于存放用户输入的命令和参数
    char cmd_firstword[BUFLEN+1] = {0};  // 用于存放第一个空格之前的子串
    enum command             { help,   clear,   list,   bat,   run,   poweroff,    reboot,   date};
    const char* commands[] = {"help", "clear", "list", "bat", "run", "poweroff",  "reboot", "date"};

    while(1) {
        promptString();
        readToBuf(cmdstr, BUFLEN);
        getFirstWord(cmdstr, cmd_firstword);

        if(strcmp(cmd_firstword, commands[help]) == 0) {
            showHelp();
        }
        else if(strcmp(cmd_firstword, commands[clear]) == 0) {
            clearScreen();
        }
        else if(strcmp(cmd_firstword, commands[list]) == 0) {
            listUserProg();
        }
        else if(strcmp(cmd_firstword, commands[bat]) == 0) {  // bat：批处理
            batch(cmdstr);
        }
        else if(strcmp(cmd_firstword, commands[run]) == 0) {  // bat：批处理
            multiProcessing(cmdstr);
        }
        else if(strcmp(cmd_firstword, commands[poweroff]) == 0) {
            powerOff();
        }
        else if(strcmp(cmd_firstword, commands[reboot]) == 0) {
            reBoot();
        }
        else if(strcmp(cmd_firstword, commands[date]) == 0) {
            showDateTime();
        }
        else {
            if(cmd_firstword[0] != '\0') {
                const char* error_msg = ": command not found\r\n";
                print(cmd_firstword);
                print(error_msg);
            }
        }
    }
}
