#include "cmd_inc.h"
#include "DSC_inc.h"

#define DEF_CLI_TIME_OUT                    15 /* unit: minutes*/
UINT g_uiCol = 0;
UINT g_uiCliTimeOut = DEF_CLI_TIME_OUT * 60; /* unit: seconds*/

static EN_CLI_PORT m_enCLI_port = EN_CLI_PORT_SERIAL;

UINT
CMD_MathLog2(ULONG x)
{
    switch (x) {
    case 0x1:
        return 0;
    case 0x2:
        return 1;
    case 0x4:
        return 2;
    case 0x8:
        return 3;
    case 0x10:
        return 4;
    case 0x20:
        return 5;
    case 0x40:
        return 6;
    case 0x80:
        return 7;
    case 0x100:
        return 8;
    case 0x200:
        return 9;
    case 0x400:
        return 10;
    case 0x800:
        return 11;
    case 0x1000:
        return 12;
    case 0x2000:
        return 13;
    case 0x4000:
        return 14;
    case 0x8000:
        return 15;
    default:
        return 0;
    }
}

UINT
CMD_StrToInt(CHAR *pcSourcr, ULONG ulLen, INT *piNum)
{
    CHAR cChar;
    ULONG ulIndex = 0;

    if (pcSourcr == NULL || ulLen > strlen(pcSourcr)) {
        *piNum = NULL_INT;
        return CMD_ERROR;
    }

    ulIndex = 0;
    if ('-' == pcSourcr[ulIndex]) {                                         /* 负数                                                    */
        ulIndex++;
    }

    for (; ulIndex < ulLen; ulIndex++) {
        cChar = pcSourcr[ulIndex];

        if (!IS_NUMBER(cChar)) {
            *piNum = NULL_INT;
            return CMD_ERROR;
        }
    }

    *piNum = atoi(pcSourcr);

    return CMD_OK;
}


UINT
CMD_StrToUlong(CHAR *pcSourcr, ULONG ulLen, ULONG *pulNum)
{
    CHAR cChar;
    ULONG ulIndex;

    if (pcSourcr == NULL || ulLen > strlen(pcSourcr)) {
        *pulNum = 0xFFFFFFFF;
        return CMD_ERROR;
    }

    ulIndex = 0;
    if ('-' == pcSourcr[ulIndex]) {                                         /* 负数                                                    */
        ulIndex++;
    }

    for (; ulIndex < ulLen; ulIndex++) {
        cChar = pcSourcr[ulIndex];

        if (!IS_NUMBER(cChar)) {
            *pulNum = 0xFFFFFFFF;
            return CMD_ERROR;
        }
    }

    *pulNum = atoi(pcSourcr);

    return CMD_OK;
}



UINT
CMD_StrToFloat(CHAR *pcSourcr, ULONG ulLen, double *pdNum)
{
    int i = 0;
    int iFlag = 0;
    double dNum = NULL_FLOAT;

    if (pcSourcr == NULL || ulLen > strlen(pcSourcr)) {
        *pdNum = NULL_FLOAT;
        return CMD_ERROR;
    }

    iFlag = 0;
    for (i = 0; i < ulLen; i++) {                                           /* '.' <= 1 个                                             */
        if ('.' == pcSourcr[i]) {
            if (1 == iFlag) {
                *pdNum = NULL_FLOAT;
                return CMD_ERROR;
            }
            iFlag = 1;
        }
    }

    i = 0;
    if ('-' == pcSourcr[i]) {                                               /* 负数                                                    */
        i++;
    }
    if (!IS_NUMBER(pcSourcr[i])) {
        *pdNum = NULL_FLOAT;
        return CMD_ERROR;
    }
    i++;

    for (; i < ulLen; i++) {
        if ('.' == pcSourcr[i]) {
            continue;
        }

        if (!IS_NUMBER(pcSourcr[i])) {
            *pdNum = NULL_FLOAT;
            return CMD_ERROR;
        }

    }

    dNum = atof(pcSourcr);
    *pdNum = dNum;

    return CMD_OK;
}



INT
CMD_FloatToInt(CHAR *pcSourcr, int iMultiple, int *piNum)
{
    char temp;
    int i = 0, Len;
    int iDate, iOffset = 0, iFlag = 0;

    Len = strlen(pcSourcr);
    iFlag = 0;
    for (i = 0; i < Len; i++) {                                             /* '.' <= 1 个                                             */
        if ('.' == pcSourcr[i]) {
            if (1 == iFlag) {
                *piNum = 0xFFFF;
                return CMD_ERROR;
            }
            iFlag = 1;
        }
    }

    i = 0;
    if ('-' == pcSourcr[i]) {                                               /* 负数                                                    */
        i++;
    }
    if (!IS_NUMBER(pcSourcr[i])) {
        *piNum = 0xFFFF;
        return CMD_ERROR;
    }
    i++;


    for (; i < Len; i++) {
        if ('.' == pcSourcr[i]) {
            temp = pcSourcr[i];
            pcSourcr[i] = pcSourcr[i + 1];
            pcSourcr[i + 1] = temp;
            iOffset++;
        }

        if (0x00 == pcSourcr[i]) {
            if (0 != iOffset) {
                iOffset--;
            }
            break;
        } else if (!IS_NUMBER(pcSourcr[i])) {
            *piNum = 0xFFFF;
            return CMD_ERROR;
        }

    }
    if (iOffset > iMultiple) {
        *piNum = 0xFFFF;
        return CMD_ERROR;
    }

    iDate = atoi(pcSourcr);
    for (; iOffset < iMultiple; iOffset++) {
        iDate = iDate * 10;
    }

    *piNum = iDate;

    return CMD_OK;
}


UINT
CMD_Sprintf(char **buf, char *fmt, ...)
{
    ULONG ulCount;
    va_list vaList;

    va_start(vaList, fmt);
    ulCount = vsprintf(*buf, fmt, vaList);
    *buf = *buf + ulCount;
    va_end(vaList);

    return ulCount;
}

UINT
CMD_ConfirmQuery(ULONG ulSes, CHAR *pcQueryInfor)
{
    ULONG ulCount = 0;
    CHAR acChar[2];
    int iRet = CMD_ERROR;

    if (NULL == pcQueryInfor) {
        CMD_OutputStr(ulSes, "\r\nDo you want to continue (y/n) [n] :");
    } else {
        CMD_OutputStr(ulSes, pcQueryInfor);
    }

    for (;; ) {
        CmdCtrl[ulSes].BaseIo.cmd_recv(CmdCtrl[ulSes].BaseIo.ulIFd, &acChar[ulCount], 1);

        CmdCtrl[ulSes].ulTrig = tickGet();

        if (0 == ulCount) {
            if (acChar[0] == 'y' || acChar[0] == 'Y') {
                CMD_OutputChar(ulSes, acChar[0]);
                ulCount++;
                iRet = CMD_OK;
            } else if (acChar[0] == 'n' || acChar[0] == 'N') {
                CMD_OutputChar(ulSes, acChar[0]);
                ulCount++;
                iRet = CMD_ERROR;
            } else if (acChar[0] == '\r' || acChar[0] == '\n') {
                return CMD_ERROR;
            } else {
                continue;
            }
        } else {
            if (acChar[1] == '\n' || acChar[1] == '\r') {
                return iRet;
            } else if (acChar[1] == '\b') {
                CMD_DeletePrevChar(ulSes);
                ulCount = 0;
                continue;
            } else {
                continue;
            }
        }
    }
}

VOID
CMD_ClearRecvBuf(ULONG ulSes)
{
    CmdCtrl[ulSes].BaseIo.ulRLen = 0;
    CmdCtrl[ulSes].BaseIo.acRBuf[0] = 0;
}

VOID
CMD_ClearTreeInfo(ULONG ulSes)
{
    CmdCtrl[ulSes].TreeDep = 0;
    CmdCtrl[ulSes].CmdTree[0] = CmdCtrl[ulSes].acEBuf;
}

UINT
CMD_GetVarLimit(CHAR *pcLimit, ULONG *pulDLimit, ULONG *pulSLimit)
{
    ULONG ulTemp;
    CHAR *pcPos = NULL;
    ULONG ulLen = 0;

    if (pcLimit == NULL || pulDLimit == NULL || pulSLimit == NULL) {
        return CMD_ERROR;
    }

    ulLen = strlen(pcLimit);

    if (pcLimit[0] != '[' || pcLimit[ulLen - 1] != ']') {
        return CMD_ERROR;
    }
    if (ulLen == 3 && pcLimit[1] == '.') {
        *pulDLimit = *pulSLimit = NULL_LONG;
        return CMD_OK;
    }

    pcPos = strchr(pcLimit, ',');
    if (NULL == pcPos) {
        if (OK != CMD_StrToUlong(pcLimit + 1, ulLen - 2, &ulTemp)) {
            return CMD_ERROR;
        }
        *pulDLimit = ulTemp;
        *pulSLimit = ulTemp;
    } else {
        if ((pcPos - pcLimit - 1) == 1 && pcLimit[1] == '.') {
            ulTemp = NULL_LONG;
        } else {
            if (OK != CMD_StrToUlong(pcLimit + 1, pcPos - pcLimit - 1, &ulTemp)) {
                return CMD_ERROR;
            }
        }

        *pulDLimit = ulTemp;

        ulLen = strlen(pcPos + 1);
        if ((ulLen - 1) == 1 && pcPos[1] == '.') {
            ulTemp = NULL_LONG;
        } else {
            if (OK != CMD_StrToUlong(pcPos + 1, strlen(pcPos + 1) - 1, &ulTemp)) {
                return CMD_ERROR;
            }
        }

        *pulSLimit = ulTemp;
    }

    return CMD_OK;
}


UINT
CMD_GetVarIntLimit(CHAR *pcLimit, INT *piDLimit, INT *piSLimit)
{
    INT iTemp = 0;
    CHAR *pcPos = NULL;
    ULONG ulLen = 0;

    if (pcLimit == NULL || piDLimit == NULL || piSLimit == NULL) {
        return CMD_ERROR;
    }

    ulLen = strlen(pcLimit);

    if (pcLimit[0] != '[' || pcLimit[ulLen - 1] != ']') {
        return CMD_ERROR;
    }
    if (ulLen == 3 && pcLimit[1] == '.') {
        *piDLimit = *piSLimit = NULL_INT;
        return CMD_OK;
    }

    pcPos = strchr(pcLimit, ',');
    if (NULL == pcPos) {
        if (OK != CMD_StrToInt(pcLimit + 1, ulLen - 2, &iTemp)) {
            return CMD_ERROR;
        }
        *piDLimit = iTemp;
        *piSLimit = iTemp;
    } else {
        if ((pcPos - pcLimit - 1) == 1 && pcLimit[1] == '.') {
            iTemp = NULL_INT;
        } else {
            if (OK != CMD_StrToInt(pcLimit + 1, pcPos - pcLimit - 1, &iTemp)) {
                return CMD_ERROR;
            }
        }

        *piDLimit = iTemp;

        ulLen = strlen(pcPos + 1);
        if ((ulLen - 1) == 1 && pcPos[1] == '.') {
            iTemp = NULL_INT;
        } else {
            if (OK != CMD_StrToInt(pcPos + 1, strlen(pcPos + 1) - 1, &iTemp)) {
                return CMD_ERROR;
            }
        }

        *piSLimit = iTemp;
    }

    return CMD_OK;
}


UINT
CMD_GetVarFloatLimit(CHAR *pcLimit, double *pdDLimit, double *pdSLimit)
{
    double dTemp = NULL_FLOAT;
    CHAR *pcPos = NULL;
    ULONG ulLen = 0;

    if (pcLimit == NULL || pdDLimit == NULL || pdSLimit == NULL) {
        return CMD_ERROR;
    }

    ulLen = strlen(pcLimit);

    if (pcLimit[0] != '[' || pcLimit[ulLen - 1] != ']') {
        return CMD_ERROR;
    }
    if (ulLen == 3 && pcLimit[1] == '.') {
        *pdDLimit = *pdSLimit = NULL_FLOAT;
        return CMD_OK;
    }

    pcPos = strchr(pcLimit, ',');
    if (NULL == pcPos) {
        if (OK != CMD_StrToFloat(pcLimit + 1, ulLen - 2, &dTemp)) {
            return CMD_ERROR;
        }
        *pdDLimit = dTemp;
        *pdSLimit = dTemp;
    } else {
        if ((pcPos - pcLimit - 1) == 1 && pcLimit[1] == '.') {
            dTemp = NULL_FLOAT;
        } else {
            if (OK != CMD_StrToFloat(pcLimit + 1, pcPos - pcLimit - 1, &dTemp)) {
                return CMD_ERROR;
            }
        }

        *pdDLimit = dTemp;

        ulLen = strlen(pcPos + 1);
        if ((ulLen - 1) == 1 && pcPos[1] == '.') {
            dTemp = NULL_FLOAT;
        } else {
            if (OK != CMD_StrToFloat(pcPos + 1, strlen(pcPos + 1) - 1, &dTemp)) {
                return CMD_ERROR;
            }
        }

        *pdSLimit = dTemp;
    }

    return CMD_OK;
}


VOID
CMD_AddParameter(ULONG ulSes, CHAR *pcPara)
{
    ULONG ulIndex = 0;
    ULONG ulQuot = FALSE;
    ULONG ulLen;

    if (NULL == pcPara)
        ulLen = 0;
    else
        ulLen = strlen(pcPara);

    if (CmdCtrl[ulSes].ParaNum >= PARA_NUM) {
        return;
    }
    if (NULL == pcPara) {
        CmdCtrl[ulSes].CurPara[CmdCtrl[ulSes].ParaNum] = NULL;
    } else {
        CmdCtrl[ulSes].CurPara[CmdCtrl[ulSes].ParaNum] = CmdCtrl[ulSes].ParaBuf + CmdCtrl[ulSes].ulParaBLen;

        if (*pcPara == '"') {
            ulQuot = TRUE;
            ulIndex++;
            ulLen--;
        }

        if (ulQuot) {
            for (; ulIndex < ulLen; ulIndex++) {
                CmdCtrl[ulSes].ParaBuf[CmdCtrl[ulSes].ulParaBLen++] = cmd_tolower(pcPara[ulIndex]);
            }
        } else {
            for (; ulIndex < ulLen; ulIndex++) {
                CmdCtrl[ulSes].ParaBuf[CmdCtrl[ulSes].ulParaBLen++] = pcPara[ulIndex];
            }
        }

        CmdCtrl[ulSes].ParaBuf[CmdCtrl[ulSes].ulParaBLen++] = 0;
    }

    CmdCtrl[ulSes].ParaNum++;
}

VOID
CMD_AddPrestorePara(ULONG ulSes)
{
    CHAR acTempBuf[16];
    ULONG ulPort = CmdCtrl[ulSes].CurMode.ulPorts;

    sprintf(acTempBuf, "%ld", ulSes);
    CMD_AddParameter(ulSes, acTempBuf);

    if (CmdCtrl[ulSes].CurMode.ucOne == CONF_MODE1 && CmdCtrl[ulSes].CurMode.usTwo == IF_MODE2) {
        if (CmdCtrl[ulSes].CurMode.ucThree == ETHE_MODE3) {
            CMD_AddParameter(ulSes, "ethernet");
        } else if (CmdCtrl[ulSes].CurMode.ucThree == GIGA_MODE3) {
            CMD_AddParameter(ulSes, "slot");
            ulPort = ulPort + 24;
        } else {
            CMD_AddParameter(ulSes, "vlan");
        }
        sprintf(acTempBuf, "%ld", ulPort);
        CMD_AddParameter(ulSes, acTempBuf);
    } else {
        CMD_AddParameter(ulSes, NULL);
        CMD_AddParameter(ulSes, NULL);
    }
}

VOID
CMD_ClearParameter(ULONG ulSes)
{
    ULONG ulTemp;

    CmdCtrl[ulSes].ParaNum = 0;
    CmdCtrl[ulSes].ulParaBLen = 0;

    for (ulTemp = 0; ulTemp < PARA_NUM; ulTemp++) {
        CmdCtrl[ulSes].CurPara[ulTemp] = NULL;
    }
}

VOID
CMD_HisNewPrompt(ULONG ulSes)
{
    char acCmp[2];

    acCmp[0] = cmd_tolower(CmdCtrl[ulSes].acEBuf[0]);
    acCmp[1] = cmd_tolower(CmdCtrl[ulSes].acEBuf[1]);

    if (acCmp[0] == 'h' && acCmp[1] == 'i') {
        return;
    }

    if (CmdCtrl[ulSes].ulHisNums < HIST_NUM) {
        CmdCtrl[ulSes].ulHisNums++;
    }

    strncpy(CmdCtrl[ulSes].History[CmdCtrl[ulSes].ulHisTail]
            , CmdCtrl[ulSes].acEBuf
            , CmdCtrl[ulSes].ulELen > DISP_LEN - BPS_LEN ? DISP_LEN - BPS_LEN : CmdCtrl[ulSes].ulELen);

    if (CmdCtrl[ulSes].acEBuf[CmdCtrl[ulSes].ulELen - 1] == '\n' ||
        CmdCtrl[ulSes].acEBuf[CmdCtrl[ulSes].ulELen - 1] == '\r') {
        CmdCtrl[ulSes].History[CmdCtrl[ulSes].ulHisTail][CmdCtrl[ulSes].ulELen - 1] = 0;
    } else {
        CmdCtrl[ulSes].History[CmdCtrl[ulSes].ulHisTail][CmdCtrl[ulSes].ulELen] = 0;
    }
    CmdCtrl[ulSes].ulHisTail++;

    if (CmdCtrl[ulSes].ulHisTail == HIST_NUM) {
        CmdCtrl[ulSes].ulHisTail = 0;
    }

    if (CmdCtrl[ulSes].ulHisNums == HIST_NUM) {
        CmdCtrl[ulSes].ulHisHead = CmdCtrl[ulSes].ulHisTail;
    }

    CmdCtrl[ulSes].ulCurHisPos = CmdCtrl[ulSes].ulHisTail;
}

VOID
CMD_PrevHisRecord(ULONG ulSes)
{
    ULONG ulHistory = CmdCtrl[ulSes].ulCurHisPos;

    if (CmdCtrl[ulSes].ulHisNums == 0) {
        return;
    }

    if (CmdCtrl[ulSes].ulCurHisPos == 0) {
        CmdCtrl[ulSes].ulCurHisPos = HIST_NUM - 1;
    } else {
        CmdCtrl[ulSes].ulCurHisPos--;
    }

    if (CmdCtrl[ulSes].History[CmdCtrl[ulSes].ulCurHisPos][0] != 0) {
        CMD_ClearCurPrompt(ulSes);

        strcpy(CmdCtrl[ulSes].acEBuf, CmdCtrl[ulSes].History[CmdCtrl[ulSes].ulCurHisPos]);
        CmdCtrl[ulSes].ulELen = strlen(CmdCtrl[ulSes].acEBuf);

        CMD_OutputStr(ulSes, CmdCtrl[ulSes].acEBuf);
    } else {
        CmdCtrl[ulSes].ulCurHisPos = ulHistory;
    }
}

VOID
CMD_NextHisRecord(ULONG ulSes)
{
    ULONG ulHistory = CmdCtrl[ulSes].ulCurHisPos;

    if (CmdCtrl[ulSes].ulHisNums == 0) {
        return;
    }

    CmdCtrl[ulSes].ulCurHisPos = (CmdCtrl[ulSes].ulCurHisPos + 1) % HIST_NUM;

    if (CmdCtrl[ulSes].History[CmdCtrl[ulSes].ulCurHisPos][0] != 0) {
        CMD_ClearCurPrompt(ulSes);

        strcpy(CmdCtrl[ulSes].acEBuf, CmdCtrl[ulSes].History[CmdCtrl[ulSes].ulCurHisPos]);
        CmdCtrl[ulSes].ulELen = strlen(CmdCtrl[ulSes].acEBuf);

        CMD_OutputStr(ulSes, CmdCtrl[ulSes].acEBuf);
    } else {
        CmdCtrl[ulSes].ulCurHisPos = ulHistory;
    }
}

VOID
CMD_StopPing(ULONG ulSes)
{
    if (ulSes > CLI_MAX_TELNET) {
        return;
    }

    CmdCtrl[ulSes].usPing = FALSE;
}

VOID
CMD_RebuildEditBuf(ULONG ulSes)
{
    ULONG ulIndex;

    CmdCtrl[ulSes].ulELen = 0;

    if (CmdCtrl[ulSes].TreeDep > 0) {
        for (ulIndex = 0; ulIndex < CmdCtrl[ulSes].TreeDep - 1; ulIndex++) {
            strcpy(CmdCtrl[ulSes].acEBuf + CmdCtrl[ulSes].ulELen
                   , CmdCtrl[ulSes].CmdTree[ulIndex]);
            CmdCtrl[ulSes].ulELen += strlen(CmdCtrl[ulSes].CmdTree[ulIndex]);
            CmdCtrl[ulSes].acEBuf[CmdCtrl[ulSes].ulELen++] = ' ';
        }

        if (0 != CMD_STRCMP("?", CmdCtrl[ulSes].CmdTree[ulIndex])) {
            strcpy(CmdCtrl[ulSes].acEBuf + CmdCtrl[ulSes].ulELen
                   , CmdCtrl[ulSes].CmdTree[ulIndex]);
            CmdCtrl[ulSes].ulELen += strlen(CmdCtrl[ulSes].CmdTree[ulIndex]);
            CmdCtrl[ulSes].acEBuf[CmdCtrl[ulSes].ulELen++] = ' ';
        }

        CmdCtrl[ulSes].acEBuf[CmdCtrl[ulSes].ulELen] = 0;
    }
}

VOID
CMD_OutputStr(ULONG ulSes, CHAR *pcStr)
{
    ULONG ulLen = 0;

    if (NULL == pcStr) {
        return;
    }

    ulLen = strlen(pcStr);
    if (0 != ulLen) {
        CmdCtrl[ulSes].BaseIo.cmd_send(CmdCtrl[ulSes].BaseIo.ulOFd, pcStr, ulLen);
    }

    return;
}

VOID
CMD_PrintfStr(ULONG ulSes, CHAR *pcStr, ...)
{
    va_list vaList;
    CHAR acBuf[1536];                                                       /* 1024+512,1024 show mfg not enough                       */
    ULONG ulLen = 0;

    if (pcStr == NULL) {
        return;
    }

    va_start(vaList, pcStr);
    vsprintf(acBuf, pcStr, vaList);
    va_end(vaList);

    ulLen = strlen(acBuf);

    if (0 != ulLen) {
        CmdCtrl[ulSes].BaseIo.cmd_send(CmdCtrl[ulSes].BaseIo.ulOFd, acBuf, ulLen);
    }

    return;
}

VOID
CMD_OutputBuf(ULONG ulSes, CHAR *Buf)
{
    ULONG ulLen = 0;
    CHAR *pcBuf = Buf;
    CHAR *pcPosition = strchr(pcBuf, '\n');

    if (NULL_LONG == CmdCtrl[ulSes].BaseIo.ulOFd) {
        return;
    }

    CmdCtrl[ulSes].ulLines = 0;
    CmdCtrl[ulSes].ulScroll = NONE_SCROLL;

    while (pcPosition != NULL) {
        ulLen = pcPosition - pcBuf;
        ulLen++;

        CmdCtrl[ulSes].BaseIo.cmd_send(CmdCtrl[ulSes].BaseIo.ulOFd, pcBuf, ulLen);
        CmdCtrl[ulSes].ulLines++;

        if (*(pcPosition + 1) == 0) {
            return;
        }
        if ((PAGE_SCROLL == CmdCtrl[ulSes].ulScroll && SCR_ROWS == CmdCtrl[ulSes].ulLines) ||
            LINE_SCROLL == CmdCtrl[ulSes].ulScroll) {
            if (PAGE_SCROLL == CmdCtrl[ulSes].ulScroll || LINE_SCROLL == CmdCtrl[ulSes].ulScroll) {
                CMD_DispNextTips(ulSes);
            }

            CMD_GetScrollMode(ulSes);

            if (NONE_SCROLL == CmdCtrl[ulSes].ulScroll) {
                break;
            } else {
                CMD_OutputStr(ulSes, "\x1b[2K");
                CMD_OutputStr(ulSes, "\r");
            }
        }

        pcBuf = pcPosition + 1;
        pcPosition = strchr(pcBuf, '\n');
    }

    if (NONE_SCROLL != CmdCtrl[ulSes].ulScroll && *pcBuf != 0) {
        CmdCtrl[ulSes].BaseIo.cmd_send(CmdCtrl[ulSes].BaseIo.ulOFd, pcBuf, strlen(pcBuf));
    }
}

VOID
CMD_OutputChar(ULONG ulSes, CHAR cChar)
{
    CmdCtrl[ulSes].BaseIo.acSBuf[0] = cChar;
    CmdCtrl[ulSes].BaseIo.acSBuf[1] = 0;
    CmdCtrl[ulSes].BaseIo.cmd_send(CmdCtrl[ulSes].BaseIo.ulOFd,
                                   CmdCtrl[ulSes].BaseIo.acSBuf, 1);
}

VOID
CMD_ClearScreen(ULONG ulSes)
{
    CMD_OutputStr(ulSes, "\x1b[37;40m");
    CMD_OutputStr(ulSes, "\x1b[2J");
    CMD_OutputStr(ulSes, "\x1b[0m");
    CMD_OutputStr(ulSes, "\x1b[2J");
}

UINT
CMD_GetCurrentColumn(ULONG ulSes)
{
    int temp;
    ULONG ulIndex;
    /* ULONG ulRow   = 0;                                                       */
    ULONG ulCol = 0;
    ULONG ulCount = 0;
    CHAR acBuffer[256];

    CMD_OutputStr(ulSes, "\x1b[250C");
    CMD_OutputStr(ulSes, "\x1b[6n");
    usleep(300000);

    ioctl(CmdCtrl[ulSes].BaseIo.ulIFd, FIONREAD, (int *)&temp);

    for (ulCount = 0; ulCount < temp; ulCount++) {
        while (1 != CmdCtrl[ulSes].BaseIo.cmd_recv(CmdCtrl[ulSes].BaseIo.ulIFd,
                                                   &acBuffer[ulCount], 1)) ;
    }
    for (ulIndex = 0; ulIndex < ulCount; ulIndex++) {
        if (acBuffer[ulIndex] == '[') {
            /* ulRow = atoi(acBuffer + ulIndex + 1);                            */
        } else if ((acBuffer[ulIndex] == ';')) {
            ulCol = atoi(acBuffer + ulIndex + 1);
            break;
        }
    }

    if (1 >= ulCol) {
        ulCol = 80;
    }
    return ulCol;
}

UINT
CMD_PreviousLine(ULONG ulSes)
{
    /* ULONG ulIndex;                                                           */
    /* ULONG ulRow   = 0;                                                       */
    /* ULONG ulCol   = 0;                                                       */
    /* ULONG ulCount = 0;                                                       */
    CHAR acBuffer[256];
    /* int   temp;                                                              */

#if (1)
    /* 光标上移一行 \x1b[1A                                                     */
    /* \x1b[1H 光标到第一行，\x1b[14C 光标到第14列                              */

    sprintf(acBuffer, "\x1b[%dC", g_uiCol);                                 /* 超级终端 一行80个字符，所以这里是79。                   */
    CMD_OutputStr(ulSes, acBuffer);

    sprintf(acBuffer, "\x1b[1A");
    CMD_OutputStr(ulSes, acBuffer);

    /* \x1b[K 清除从光标到行尾的内容                                            */
    sprintf(acBuffer, "\x1b[K");
    CMD_OutputStr(ulSes, acBuffer);
#else
    CMD_OutputStr(ulSes, "\x1b[6n");

    ioctl(CmdCtrl[ulSes].BaseIo.ulIFd, FIONREAD, (int)&temp);

    for (ulCount = 0; ulCount < temp; ulCount++) {
        while (1 != CmdCtrl[ulSes].BaseIo.cmd_recv(CmdCtrl[ulSes].BaseIo.ulIFd,
                                                   &acBuffer[ulCount], 1)) ;
    }
    for (ulIndex = 0; ulIndex < ulCount; ulIndex++) {
        if (acBuffer[ulIndex] == '[') {
            ulRow = atoi(acBuffer + ulIndex + 1);
        } else if ((acBuffer[ulIndex] == ';')) {
            ulCol = atoi(acBuffer + ulIndex + 1);
            break;
        }
    }

    if (1 != ulCol) {
        return FALSE;
    }

    sprintf(acBuffer, "\x1b[%ld;80H", ulRow - 1);

    CMD_OutputStr(ulSes, acBuffer);

    CMD_OutputChar(ulSes, ' ');

    CMD_OutputStr(ulSes, acBuffer);
#endif
    return TRUE;
}

VOID
CMD_DeletePrevChar(ULONG ulSes)
{
    if ((CmdCtrl[ulSes].ulPromLen + CmdCtrl[ulSes].ulELen + 1) % g_uiCol == 0) {
        if (CMD_PreviousLine(ulSes)) {
            return;
        }
    }

    CMD_OutputChar(ulSes, '\b');
    CMD_OutputChar(ulSes, ' ');
    CMD_OutputChar(ulSes, '\b');
}

VOID
CMD_ClearCurPrompt(ULONG ulSes)
{
    ULONG ulIndex;

    for (ulIndex = CmdCtrl[ulSes].ulELen; ulIndex > 0; ulIndex--) {
        CmdCtrl[ulSes].ulELen--;
        CMD_DeletePrevChar(ulSes);
    }

    CmdCtrl[ulSes].ulELen = 0;
}

VOID
CMD_WelComeInfo(ULONG ulSes, UINT uiFirstLogin)
{
    g_uiCol = CMD_GetCurrentColumn(ulSes);

    g_uiCliTimeOut = CMD_GetTimeOut();

    LOG_WRITE(EN_LOG_NOTICE, "CLi-timeout: %u", g_uiCliTimeOut);

    if (FIRST_LOGIN == uiFirstLogin) {
        CMD_OutputStr(ulSes, _CLI_WELCOME_INFO_);
    } else {
        CMD_OutputStr(ulSes, "\r\n");
    }
}

VOID
CMD_DispNextTips(ULONG ulSes)
{
    CMD_OutputStr(ulSes, "\x1b[2K");
    CMD_OutputStr(ulSes, "\r");

    CMD_OutputStr(ulSes, "Next page: ");
    CMD_OutputStr(ulSes, "<SPACE>");

    CMD_OutputStr(ulSes, " Next line: ");
    CMD_OutputStr(ulSes, "<ENTER>");

    CMD_OutputStr(ulSes, " Quit: ");
    CMD_OutputStr(ulSes, "<Ctrl+D>");
}

INT
CMD_CheckDateValid(CHAR *pcDate)
{
    USHORT usYear;
    UCHAR ucMon, ucDay;
    CHAR chData[8] = {0};
    INT i = 0, ret = 0;

    ret = sscanf(pcDate, "%c%c%c%c-%c%c-%c%c", &(chData[0]), &(chData[1]), &(chData[2]), &(chData[3]), &(chData[4]), &(chData[5]), &(chData[6]), &(chData[7]));
    if (ret < 0) {
        goto ERR_LABEL;
    }
    for (i = 0; i < 8; i++) {
        if ( chData[i] < '0' || chData[i] > '9' ) {
            goto ERR_LABEL;
        }
    }


    /* format YYYY-MM-DD                                                        */
    sscanf(pcDate, "%4hd-%2hhu-%2hhu", &usYear, &ucMon, &ucDay);

    if (usYear > 2050 || usYear < 2000) {
        goto ERR_LABEL;
    }

    if (ucMon > 12 || ucMon < 1) {
        goto ERR_LABEL;
    }

    if (ucDay > 31 || ucDay < 1) {
        goto ERR_LABEL;
    }

    /**the format is defined by yang*********************************************/
    sprintf(pcDate, "%04hd-%02hhu-%02hhu", usYear, ucMon, ucDay);

    return OK;
ERR_LABEL:
    return ERROR;
}

INT
CMD_CheckTimeValid(CHAR *pcTime)
{
    UCHAR ucHour, ucMinute, ucSec;
    CHAR chData[6] = {0};
    INT i, ret = 0;

    ret = sscanf(pcTime, "%c%c:%c%c:%c%c", &(chData[0]), &(chData[1]), &(chData[2]), &(chData[3]), &(chData[4]), &(chData[5]));
    if (ret < 0) {
        goto ERR_LABEL;
    }
    for (i = 0; i < 6; i++) {
        if ( chData[i] < '0' || chData[i] > '9' ) {
            goto ERR_LABEL;
        }
    }

    sscanf(pcTime, "%2hhu:%2hhu:%2hhu", &ucHour, &ucMinute, &ucSec);

    /* format hh:mm:ss                                                          */
    if (ucHour > 24 || ucHour < 0) {
        goto ERR_LABEL;
    }

    if (ucMinute > 60 || ucMinute < 0) {
        goto ERR_LABEL;
    }

    if (ucSec > 60 || ucSec < 0) {
        goto ERR_LABEL;
    }

    /**the format is defined by yang*********************************************/
    sprintf(pcTime, "%02hhu:%02hhu:%02hhu", ucHour, ucMinute, ucSec);

    return OK;
ERR_LABEL:
    return ERROR;
}

UINT
CMD_GetTimeOut(void)
{
    INT rc = SR_ERR_OK;
    USHORT usTimeout = 0;

    /* unit is minute, need change to seconds           */
    rc = DSC_GetCliTimeout(&usTimeout);
    if (rc != SR_ERR_OK) {
        LOG_WRITE(EN_LOG_ERROR, "get timeout failed, use default");
        return DEF_CLI_TIME_OUT * 60;
    }

    return usTimeout * 60;
}

int
CMD_GetEnableState(char *pcBuf, bool *pbFlag)
{

    if (0 == strcasecmp(pcBuf, "enable") ||
        0 == strcasecmp(pcBuf, "on")) {

        *pbFlag = true;
        return OK;

    } else if (0 == strcasecmp(pcBuf, "disable") ||
               0 == strcasecmp(pcBuf, "off")) {

        *pbFlag = false;
        return OK;
    }

    return ERROR;
}

BOOL
CMD_CheckPower(double power)
{
    if (power <= -99.00) {
        return TRUE;
    } else {
        return FALSE;
    }

    return FALSE;
}

ULONG
CMD_Return(int rc, char *pcCommand, ULONG ulSes)
{
    if (SR_ERR_OK != rc) {
        CMD_PrintfStr(ulSes, "\r\nError Response:\r\n");
    } else {
        return CMD_OK;
    }

    switch (rc) {
    case SR_ERR_OK:
        return CMD_OK;
        break;
    case SR_ERR_UNSUPPORTED:
        CMD_PrintfStr(ulSes, "\r\n    [%d] Unsupported Command.\r\n", CMD_Unsupport);
        return CMD_Unsupport;
        break;
    case SR_ERR_INVAL_ARG:
        CMD_PrintfStr(ulSes, "\r\n    [%d] Command Argument Error.\r\n", CMD_ArgError);
        return CMD_ArgError;
        break;
    case SR_ERR_INTERNAL:
    case SR_ERR_NOT_FOUND:
    case SR_ERR_OPERATION_FAILED:
        CMD_PrintfStr(ulSes, "\r\n    [%d] Unable to execute.\r\n", CMD_UnableToExecute);
        return CMD_UnableToExecute;
        break;
    case SR_ERR_UNAUTHORIZED:
        CMD_PrintfStr(ulSes, "\r\n       [%d] Operation not authorized.\r\n", CMD_OperationNotAuthorized);
        return CMD_OperationNotAuthorized;
        break;
    case SR_ERR_INVAL_USER:
        CMD_PrintfStr(ulSes, "\r\n    [%d] The user name or password is not recognized.\r\n", CMD_UnknowUserName);
        return CMD_UnknowUserName;
        break;
    case SR_ERR_DATA_EXISTS:
        CMD_PrintfStr(ulSes, "\r\n    [%d] The same data exists. Or the same instructions in progress!\r\n", CMD_SameDataExists);
        return CMD_SameDataExists;
        break;
    case SR_ERR_VALIDATION_FAILED:
        CMD_PrintfStr(ulSes, "\r\n       [%d] The data validation failed.\r\n", CMD_DataValidationFaild);
        return CMD_DataValidationFaild;
        break;
    /*case SR_ERR_INVAL_ARG:
        CMD_PrintfStr(ulSes, "\r\n    [%d] Argument's value out of range.\r\n", CMD_ArgOutOfRange);
        return CMD_ArgOutOfRange;
        break;*/
    /*case SR_ERR_INTERNAL:
        CMD_PrintfStr(ulSes, "\r\n    [%d] Command accepted, but completion is delayed.\r\n", CMD_Delayed);
        return CMD_Delayed;
        break;*/
    /*case SR_ERR_UNSUPPORTED:
        CMD_PrintfStr(ulSes, "\r\n    [%d] Upgrade still running in background.\r\n", CMD_Upgrading);
        return CMD_Upgrading;
        break;*/
    /*case SR_ERR_UNSUPPORTED:
        CMD_PrintfStr(ulSes, "\r\n    [%d] The password is not recognized.\r\n", CMD_UnknowPassword);
        return CMD_UnknowPassword;
        break;*/
    default:
        CMD_PrintfStr(ulSes, "\r\n    [%d] Unable to execute.\r\n", CMD_UnableToExecute);
        return CMD_UnableToExecute;
        break;
    }

    return CMD_OK;
}


VOID
CMD_SprintfPower(CHAR **pcBuf, CHAR *format, double power)
{
    if (CMD_CheckPower(power)) {
        CMD_Sprintf(pcBuf, "%sN/A\r\n", format);
    } else {
        CMD_Sprintf(pcBuf, "%s%.02fdBm\r\n", format, power);
    }
}

VOID
CMD_SprintfDB(CHAR **pcBuf, CHAR *format, double power)
{
    if (CMD_CheckPower(power)) {
        CMD_Sprintf(pcBuf, "%sN/A\r\n", format);
    } else {
        CMD_Sprintf(pcBuf, "%s%.02fdB\r\n", format, power);
    }
}

INT
CMN_GetSystemTime(struct tm *pstSysTime)
{
    time_t iCurTime;
    struct tm *pstTmp = NULL;
    if (NULL == pstSysTime) {
        return CMD_ERROR;
    }
    time(&iCurTime);
    pstTmp = localtime(&iCurTime);
    pstSysTime->tm_year = pstTmp->tm_year + 1900;
    pstSysTime->tm_mon = pstTmp->tm_mon + 1;
    pstSysTime->tm_mday = pstTmp->tm_mday;
    pstSysTime->tm_hour = pstTmp->tm_hour;
    pstSysTime->tm_min = pstTmp->tm_min;
    pstSysTime->tm_sec = pstTmp->tm_sec;
    return CMD_OK;
}

VOID
CMD_DisplayTime(ULONG ulSes)
{
    CHAR acFmt[128] = {0};
    struct tm stLocalTime;
    memset(&stLocalTime, 0x00, sizeof(stLocalTime));
    CMN_GetSystemTime(&stLocalTime);
    sprintf(acFmt, "\r\n\r\n%4d-%02d-%02d %02d:%02d:%02d", stLocalTime.tm_year,
            stLocalTime.tm_mon, stLocalTime.tm_mday, stLocalTime.tm_hour,
            stLocalTime.tm_min, stLocalTime.tm_sec);
    CMD_OutputStr(ulSes, acFmt);
}

EN_CLI_PORT
CMD_GetCliPort(void)
{
    return m_enCLI_port;
}

void
CMD_SetCliPort(EN_CLI_PORT enPort)
{
    m_enCLI_port = enPort;
}

