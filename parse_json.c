
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "json.h"
#include "parse_json.h"

//=============================================================================
//                Constant Definition
//=============================================================================
#define SHOW_PARSING_MSG        0

typedef enum STATE_TYPE_T
{
    STATE_TYPE_UNKOWN       = 0,
    STATE_TYPE_GROUP,
    STATE_TYPE_OPTION,
    STATE_TYPE_OPTION_GROUP,

}STATE_TYPE;
//=============================================================================
//                Macro Definition
//=============================================================================

//=============================================================================
//                Structure Definition
//=============================================================================
typedef struct INFO_CTRL_T
{
    char                item_name[MAX_MEMBER_NAME_LENGTH];
    CMD_ATTR            *pInput_item_attr;
    CMD_ATTR            *pAct_item_attr;
    STATE_TYPE          state_type;
    //uint32_t            isGroup;
    //uint32_t            isSwitch;

}INFO_CTRL;
//=============================================================================
//                Global Data Definition
//=============================================================================
static uint32_t     g_order_cnt = 0;
static uint32_t     g_option_order_cnt = 0;
//=============================================================================
//                Private Function Definition
//=============================================================================
static uint32_t
_refind_member_name(char *pStr, uint32_t length)
{
    do{
        int     i = 0;
        bool    bLast_char = false;
        if( !pStr || !length )     break;

        for(i = 0; i < length; i++)
        {
            if( pStr[length - 1 - i] == ' ' ||
                pStr[length - 1 - i] == '-' )
            {
                if( bLast_char == true )
                    pStr[length - 1 - i] = '_';
                else
                    pStr[length - 1 - i] = '\0';
            }
            else  bLast_char = true;
        }
    }while(0);
    return 0;
}

static uint32_t
_check_member_type(
    CMD_MEMBER_ITEM     *pCur_Member,
    char                *pVal)
{
    if( !strncmp(pVal, _toStr(void), strlen(_toStr(void))) )
    {
        pCur_Member->byte_length = 4;
        pCur_Member->member_type = MEMBER_TYPE_VOID;
    }
    else if( !strncmp(pVal, _toStr(uint32_t), strlen(_toStr(uint32_t))) )
    {
        pCur_Member->byte_length = 4;
        pCur_Member->member_type = MEMBER_TYPE_UINT32;
    }
    else if( !strncmp(pVal, _toStr(uint16_t), strlen(_toStr(uint16_t))) )
    {
        pCur_Member->byte_length = 2;
        pCur_Member->member_type = MEMBER_TYPE_UINT16;
    }
    else if( !strncmp(pVal, _toStr(uint8_t), strlen(_toStr(uint8_t))) )
    {
        pCur_Member->byte_length = 1;
        pCur_Member->member_type = MEMBER_TYPE_UINT8;
    }
    else if( !strncmp(pVal, _toStr(char), strlen(_toStr(char))) )
    {
        pCur_Member->byte_length = 1;
        pCur_Member->member_type = MEMBER_TYPE_CHAR;
    }
    else if( !strncmp(pVal, _toStr(float), strlen(_toStr(float))) )
    {
        pCur_Member->byte_length = 4;
        pCur_Member->member_type = MEMBER_TYPE_FLOAT;
    }
    else if( !strncmp(pVal, "string", strlen("string")) )
    {
        pCur_Member->byte_length = 4;
        pCur_Member->member_type = MEMBER_TYPE_STRING;
    }
    else
    {

    }

    return 0;
}

static uint32_t
_get_info_from_json(
    struct json_object  *jParent,
    INFO_CTRL           *pInfo_ctrl,
    uint32_t            layer)
{
    json_object_object_foreach(jParent, pKey, pObj)
    {
        do{int i; for(i=0;i<layer;i++)_msg(SHOW_PARSING_MSG, "  ");}while(0);

        _msg(SHOW_PARSING_MSG, "get key: %s,", pKey);
        switch( json_object_get_type(pObj) )
        {
        case json_type_string:
            break;
        case json_type_int:
            break;
        case json_type_object:
            break;
        case json_type_array:
            break;
        default:
            printf("*");
            break;
        }
    }
    return 0;
}
//=============================================================================
//                Public Function Definition
//=============================================================================
uint32_t
Parse_Json(
    char        *pPath,
    CMD_ATTR    *pInput_cmd_attr)
{
    uint32_t        result = 0;
    FILE            *fp = 0;
    uint8_t         *pBin_buf = 0;
    do{
        uint32_t    fileSize = 0;

        if( !pPath )    break;

        if( !(fp = fopen(pPath, "r")) )
        {
            _err("open %s fail !!\n", pPath);
            result = -1;
            while(1);
            break;
        }

        fseek(fp, 0, SEEK_END);
        fileSize = ftell(fp);
        fseek(fp, 0, SEEK_SET);
        if( !(pBin_buf = malloc(fileSize + 1)) )
        {
            result = -1;
            break;
        }

        memset(pBin_buf, 0x0, fileSize + 1);

        fread(pBin_buf, 1, fileSize, fp);
        fclose(fp);     fp = 0;

        {
            struct json_object      *jRoot = json_tokener_parse((char*)pBin_buf);
            INFO_CTRL               info_ctrl = {0};

            info_ctrl.pInput_cmd_attr  = pInput_cmd_attr;
            g_order_cnt = 0;
            _get_info_from_json(jRoot, &info_ctrl, 0);
        }

    }while(0);
    if( result )
    {
        if( fp )            fclose(fp);
        if( pBin_buf )      free(pBin_buf);
    }
    return result;
}


