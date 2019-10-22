#ifndef _CMD_COMMON_H
#define _CMD_COMMON_H

#ifdef __cplusplus
extern "C" {
#endif

#define BPS_LEN          50 /* 避免命令太长crash，先暂时设置短一些。 */

#define DISP_LEN         256
#define EBUF_LEN         256
#define HIST_NUM         10
#define TREE_DEPTH       20
#define PARA_NUM         20
#define UNAME_LEN        19
#define PWORD_LEN        19
#define PBUF_LEN         (1024 * 300)

#define CONF_FILE_SIZE   0x8000
#define MAX_FILE_CMD     0x1D00
#define MAX_IF_CMD       0x20

#define PAGE_SCROLL      0xAAAA
#define LINE_SCROLL      0xBBBB
#define NONE_SCROLL      0xCCCC

#define MAX_SAME_TOKEN   100
#define TAB_DISP_NUM     2
#define TAB_DISP_STR        ("%-40s")
#define DISP_STR            ("\r\n%-40s %s")
#define DISP_STR_END        ("%-40s %s\r\n")

#define SCR_ROWS         21
#define SCR_COLS         256

#define CMD_MIN_OUTLEN            (256)
#define CMD_SHELL_READ_BUF_LEN    (1024)

#define CMD_BUFFER_LEN            (255)
#define SHELL_EXIT_CMD            ("exit")
#define SHELL_KEY_LEN             (32)
#define SHELL_CMD_MAX_NUM         (26)

#define SHELL_CMD_TRY_TIMES       (5)
#define LOGIN_TRY_TIMES           (3)
#define ONE_HUNDRED_MILLSECOND    (100 * 1000)
#define CMD_LOG_BUF_LEN           (200)
#define MEM_INFO_MAX_LINES        (4)
#define STAT_INFO_MAX_LINES       (1)
#define AVE_STAT_INFO_MAX_LINES   (4)

#define CMD_EMU_MAX_BUF           (1024 * 15)
#define LOGOUT_NAME               "logout"

#define CMD_COMPLETED_RES         "\r\nCompleted!"
#define CMD_INPUT_ERROR_RES       "\r\nInput error! ErrCode = %d\r\n"
#define CMD_FAIL_RES              "\r\nCmd process failed! ErrCode = %d\r\n"



#define CMD_REBOOT_FAILURE           (10)
#define CMD_COMMAND_ERROR            (15)
#define CMD_PARAMETERS_INSUFFICIENCY (40)
#define CMD_PARAMETERS_VALUE_ERROR   (41)
#define CMD_AUTHORITY_NOT_ENOUGH     (43)
#define CMD_LOGIN_FAILED             (56)
#define CMD_NOT_LOGIN                (57)

#define CMD_USER_LOCKED              (55)
#define CMD_USER_EXIST_ERROR         (58)
#define CMD_USER_UNEXIST_ERROR       (59)
#define CMD_PROCESS_ERROR            (1)
#define CMD_MODULE_UNMOUNTED_ERROR   (2)
#define CMD_OVER_TIME                (3)
#define CMD_PATH_ERROR               (4)
#define CMD_USERNAME_CHECK_ERROR     (5)
#define CMD_PASSWORD_CHECK_ERROR     (6)
#define CMD_PASSWORD_RETRY_ERROR     (7)
#define CMD_CREATE_MSG_ERROR         (8)

#define PRES_ARG 3
#define SCEN_ARG 4
#define TREB_ARG 5
#define FOUR_ARG 6
#define FIVE_ARG 7
#define SIX_ARG  8
#define SEV_ARG  9


#define CHAR2NUM(c)  (c - 0x30)
#define IS_NUMBER(c) ((c >= '0' && c <= '9') ? 1 : 0)

#define CMD_STRCMP      CMD_StringCmp

#define FIRST_LOGIN (0)
#define NONE_FIRST_LOGIN (1)

typedef struct cmd_base_info
{
    ULONG Session;
    ULONG ulIFd;
    ULONG ulOFd;
    CHAR acRBuf[EBUF_LEN];
    ULONG ulRLen;
    CHAR acSBuf[EBUF_LEN];
    ULONG ulSLen;
    VOID (*cmd_send)(ULONG ulFd, CHAR *cBuf, ULONG ulLen);
    ULONG (*cmd_recv)(ULONG ulFd, CHAR *cBuf, ULONG ulLen);
} CMD_Base;

typedef struct mode_info
{
    UINT8 ucOne;
    UINT16 usTwo; /*unused */
    UINT8 ucThree; /*unused */
    ULONG ulPorts;
} CMD_Mode;

typedef struct cmd_ctrl_info
{
    CMD_Mode CurMode;
    ULONG UserLevel;
    CMD_Base BaseIo;
    CHAR History[HIST_NUM][DISP_LEN];
    ULONG ulHisNums;
    ULONG ulHisHead;
    ULONG ulHisTail;
    ULONG ulCurHisPos;
    CHAR acPBuf[PBUF_LEN];
    CHAR acEBuf[DISP_LEN];
    ULONG ulELen;
    ULONG ulESC;
    ULONG TreeDep;
    CHAR *CmdTree[TREE_DEPTH];

    ULONG ulParaBLen;
    CHAR ParaBuf[DISP_LEN];
    ULONG ParaNum;
    CHAR *CurPara[PARA_NUM];

    USHORT usPing;
    USHORT usString;
    ULONG ulLogin;
    ULONG ulTrig;
    ULONG ulPromLen;
    ULONG ulLines;
    ULONG ulScroll;
    ULONG ulQuit;
} CMD_Ctrl;

typedef struct _cmd_DefIfCon
{
    char *DefCmd[MAX_IF_CMD];
    ULONG CmdNum;
} CMD_IFProm;

typedef enum {
    EN_CLI_PORT_SERIAL = 0,
    EN_CLI_PORT_SSH,
} EN_CLI_PORT;

extern CMD_Ctrl *CmdCtrl;


extern UINT  CMD_StrToInt(CHAR *pcSourcr, ULONG ulLen, INT *piNum);
extern UINT  CMD_StrToUlong(CHAR *pcSourcr, ULONG ulLen, ULONG *pulNum);
extern UINT  CMD_StrToFloat(CHAR *pcSourcr, ULONG ulLen, double *pdNum);
extern UINT  CMD_MathLog2(ULONG x);
extern VOID  CMD_WelComeInfo(ULONG ulSession, UINT uiFirstLogin);
extern VOID  CMD_OutputChar(ULONG ulSession, CHAR cChar);
extern VOID  CMD_OutputStr(ULONG ulSession, CHAR *pcString);
extern VOID  CMD_OutputBuf(ULONG ulSession, CHAR *Buf);
extern UINT  CMD_GetCurrentColumn(ULONG ulSes);
extern VOID  CMD_ClearScreen(ULONG ulSession);
extern VOID  CMD_DeletePrevChar(ULONG ulSession);
extern VOID  CMD_ClearParameter(ULONG ulSession);
extern VOID  CMD_ClearCurPrompt(ULONG ulSession);
extern VOID  CMD_ClearTreeInfo(ULONG ulSession);
extern VOID  CMD_ClearRecvBuf(ULONG ulSession);
extern VOID  CMD_HisNewPrompt(ULONG ulSession);
extern VOID  CMD_PrevHisRecord(ULONG ulSession);
extern VOID  CMD_NextHisRecord(ULONG ulSession);
extern VOID  CMD_AddParameter(ULONG ulSession, CHAR *pcArg);
extern VOID  CMD_AddPrestorePara(ULONG ulSession);
extern VOID  CMD_RebuildEditBuf(ULONG ulSession);
extern ULONG CMD_StringCmp(CHAR *pSrc, CHAR *pDet);
extern VOID  CMD_GetScrollMode(ULONG ulSession);
extern VOID  CMD_StartPing(ULONG ulSession, ULONG ulTaskId);
extern VOID  CMD_StopPing(ULONG ulSession);
extern UINT  CMD_Sprintf(char **buf, char *fmt, ...);
extern VOID  CMD_PrintfStr(ULONG ulSession, CHAR *pcString, ...);
extern UINT  CMD_ConfirmQuery(ULONG ulSes, CHAR *pcQueryInfor);
extern UINT  CMD_GetVarLimit(CHAR *pcLimit, ULONG *pulDLimit, ULONG *pulSLimit);
extern UINT  CMD_GetVarIntLimit(CHAR *pcLimit, INT *piDLimit, INT *piSLimit);
extern UINT  CMD_GetVarFloatLimit(CHAR *pcLimit, double *pdDLimit, double *pdSLimit);
extern VOID  CMD_DispNextTips(ULONG ulSession);
extern INT   CMD_FloatToInt(CHAR *pcSourcr, int iMultiple, int *piNum);

extern INT  CMD_CheckDateValid(CHAR *pcDate);
extern INT  CMD_CheckTimeValid(CHAR *pcTime);
extern UINT CMD_GetTimeOut(void);

extern int CMD_GetEnableState(char *pcBuf, bool *pbFlag);

extern BOOL CMD_CheckPower(double power);
extern ULONG CMD_Return(int rc, char *pcCommand, ULONG ulSes);
extern VOID CMD_SprintfPower(CHAR **pcBuf, CHAR *format, double power);
extern VOID CMD_SprintfDB(CHAR **pcBuf, CHAR *format, double power);

extern VOID CMD_DisplayTime(ULONG ulSes);

extern EN_CLI_PORT  CMD_GetCliPort(void);
extern void CMD_SetCliPort(EN_CLI_PORT enPort);

#ifdef __cplusplus
}
#endif

#endif

