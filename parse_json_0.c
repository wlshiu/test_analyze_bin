
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "json.h"
#include "parse_json.h"

//=============================================================================
//				  Constant Definition
//=============================================================================
#define SHOW_PARSING_MSG        0

typedef enum STATE_TYPE_T
{
    STATE_TYPE_UNKOWN       = 0,
    STATE_TYPE_GROUP,
    STATE_TYPE_SWITCH,
    STATE_TYPE_SWITCH_GROUP,
    STATE_TYPE_SUB_GROUP,
}STATE_TYPE;
//=============================================================================
//				  Macro Definition
//=============================================================================

//=============================================================================
//				  Structure Definition
//=============================================================================
typedef struct INFO_CTRL_T
{
    char                cmd_name[MAX_MEMBER_NAME_LENGTH];
    CMD_ATTR            *pInput_cmd_attr;
    CMD_ATTR            *pOutput_cmd_attr;
    CMD_ATTR            *pOther_cmd_attr;
    CMD_ATTR            *pAct_cmd_attr;
    STATE_TYPE          state_type;
    //uint32_t            isGroup;
    //uint32_t            isSwitch;

}INFO_CTRL;
//=============================================================================
//				  Global Data Definition
//=============================================================================
static uint32_t     g_order_cnt = 0;
static uint32_t     g_switch_order_cnt = 0;
//=============================================================================
//				  Private Function Definition
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
    else if( !strncmp(pVal, "stringList", strlen("stringList")) )
    {
        pCur_Member->byte_length = 4;
        pCur_Member->member_type = MEMBER_TYPE_STRING;
    }
    else if( !strncmp(pVal, "border_uint8_t", strlen("border_uint8_t")) )
    {
        pCur_Member->byte_length = 1;
        pCur_Member->member_type = MEMBER_TYPE_BORDER_UINT8;
    }
    else
    {
        int     ret = 0;
        int     array_size = 0;
        char    type_str[MAX_MEMBER_NAME_LENGTH] = {0};

        if( (ret = sscanf(pVal, "%u %s", &array_size, type_str)) == 2 )
        {
            if( !strncmp(type_str, _toStr(uint32_t), strlen(_toStr(uint32_t))) )
            {
                pCur_Member->byte_length   = 4 * array_size;
                pCur_Member->array_elm_cnt = array_size;
                pCur_Member->member_type = MEMBER_TYPE_UINT32_ARRAY;
            }
            else if( !strncmp(type_str, _toStr(uint16_t), strlen(_toStr(uint16_t))) )
            {
                pCur_Member->byte_length   = 2 * array_size;
                pCur_Member->array_elm_cnt = array_size;
                pCur_Member->member_type = MEMBER_TYPE_UINT16_ARRAY;
            }
            else if( !strncmp(type_str, _toStr(uint8_t), strlen(_toStr(uint8_t))) )
            {
                pCur_Member->byte_length   = 1 * array_size;
                pCur_Member->array_elm_cnt = array_size;
                pCur_Member->member_type = MEMBER_TYPE_UINT8_ARRAY;
            }
            else if( !strncmp(type_str, _toStr(float), strlen(_toStr(float))) )
            {
                pCur_Member->byte_length   = 4 * array_size;
                pCur_Member->array_elm_cnt = array_size;
                pCur_Member->member_type = MEMBER_TYPE_FLOAT_ARRAY;
            }
        }
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
            if( !strncmp(pKey, "cmd", strlen("cmd")) )
            {
                snprintf(pInfo_ctrl->cmd_name, MAX_MEMBER_NAME_LENGTH, "%s", json_object_get_string(pObj));
                _refind_member_name(pInfo_ctrl->cmd_name, strlen(pInfo_ctrl->cmd_name));
            }

            if( pInfo_ctrl->pAct_cmd_attr )
            {
                CMD_MEMBER_ITEM     *pCur_Member = 0;

                switch( pInfo_ctrl->state_type )
                {
                    case STATE_TYPE_GROUP:
                        {
                            CMD_MEMBER_GROUP    *pCur_group = &pInfo_ctrl->pAct_cmd_attr->pGroups[pInfo_ctrl->pAct_cmd_attr->total_groups];

                            if( !strlen(pCur_group->group_name) )
                            {
                                pCur_Member = &pCur_group->group_cnt_member;
                                pCur_Member->order = g_order_cnt++;
                            }
                            else
                            {
                                pCur_Member = &pCur_group->group_member[pCur_group->total_group_members];
                                pCur_Member->order = pCur_group->total_group_members;
                                pCur_group->total_group_members++;
                            }
                        }
                        break;
                    case STATE_TYPE_SUB_GROUP:
                        {
                            CMD_MEMBER_GROUP        *pCur_group = &pInfo_ctrl->pAct_cmd_attr->pGroups[pInfo_ctrl->pAct_cmd_attr->total_groups];
                            CMD_MEMBER_SUB_GROUP    *pCur_subgroup = 0;

                            pCur_subgroup = &pCur_group->sub_groups[pCur_group->total_sub_groups];
                            if( !strlen(pCur_subgroup->subgroup_name) )
                            {
                                pCur_Member = &pCur_subgroup->subgroup_cnt_member;
                                pCur_Member->order = pCur_group->total_group_members++; // ????
                            }
                            else
                            {
                                pCur_Member = &pCur_subgroup->subgroup_member[pCur_subgroup->total_subgroup_members];
                                pCur_Member->order = pCur_subgroup->total_subgroup_members;
                                pCur_subgroup->total_subgroup_members++;
                            }
                        }
                        break;
                    case STATE_TYPE_SWITCH_GROUP:
                        {
                            CMD_SWITCH_GROUP    *pCur_switch = &pInfo_ctrl->pAct_cmd_attr->pSwitchs[pInfo_ctrl->pAct_cmd_attr->total_switchs];
                            CMD_MEMBER_GROUP    *pCur_group = &pCur_switch->switch_groups[pCur_switch->total_switch_groups];
                            if( !strlen(pCur_group->group_name) )
                            {
                                pCur_Member = &pCur_group->group_cnt_member;
                                pCur_Member->order = g_switch_order_cnt++;
                            }
                            else
                            {
                                pCur_Member = &pCur_group->group_member[pCur_group->total_group_members];
                                pCur_Member->order = pCur_group->total_group_members;
                                pCur_group->total_group_members++;
                            }
                        }
                        break;
                    case STATE_TYPE_SWITCH:
                        {
                            CMD_SWITCH_GROUP    *pCur_switch = &pInfo_ctrl->pAct_cmd_attr->pSwitchs[pInfo_ctrl->pAct_cmd_attr->total_switchs];
                            pCur_Member = &pCur_switch->switch_members[pCur_switch->total_switch_members];
                            pCur_Member->order = g_switch_order_cnt++;
                            pCur_switch->total_switch_members++;
                        }
                        break;
                    default:
                        {
                            pCur_Member = &pInfo_ctrl->pAct_cmd_attr->pMembers[pInfo_ctrl->pAct_cmd_attr->total_members];
                            pInfo_ctrl->pAct_cmd_attr->total_members++;
                            pCur_Member->order = g_order_cnt++;
                        }
                        break;
                }

                snprintf(pCur_Member->member_name, MAX_MEMBER_NAME_LENGTH, "%s", pKey);
                _refind_member_name(pCur_Member->member_name, strlen(pCur_Member->member_name));
                _check_member_type(pCur_Member, json_object_get_string(pObj));
            }
            _msg(SHOW_PARSING_MSG, "vlaue = %s\n", json_object_get_string(pObj));
            break;
        case json_type_int:
            if( !strncmp(pKey, "pattern_type", strlen("pattern_type")) )
            {
                pInfo_ctrl->pAct_cmd_attr->pattern_type = json_object_get_int(pObj);
            }
            _msg(SHOW_PARSING_MSG, "vlaue = %d\n", json_object_get_int(pObj));
            break;
        case json_type_object:
            _msg(SHOW_PARSING_MSG, "\n");
            if( !strncmp(pKey, "Input", strlen("Input")) )
            {
                snprintf(pInfo_ctrl->pInput_cmd_attr->cmd_name, MAX_MEMBER_NAME_LENGTH,
                        "%s%s", pInfo_ctrl->cmd_name, pKey);
                _refind_member_name(pInfo_ctrl->pInput_cmd_attr->cmd_name, strlen(pInfo_ctrl->pInput_cmd_attr->cmd_name));
                pInfo_ctrl->pAct_cmd_attr = pInfo_ctrl->pInput_cmd_attr;
                pInfo_ctrl->pAct_cmd_attr->mode = CMD_DIRECTION_MODE_INPUT;
                g_order_cnt = 0;
                g_switch_order_cnt = 0;
            }
            else if( !strncmp(pKey, "Output", strlen("Output")) )
            {
                snprintf(pInfo_ctrl->pOutput_cmd_attr->cmd_name, MAX_MEMBER_NAME_LENGTH,
                    "%s%s", pInfo_ctrl->cmd_name, pKey);
                _refind_member_name(pInfo_ctrl->pOutput_cmd_attr->cmd_name, strlen(pInfo_ctrl->pOutput_cmd_attr->cmd_name));
                pInfo_ctrl->pAct_cmd_attr = pInfo_ctrl->pOutput_cmd_attr;
                pInfo_ctrl->pAct_cmd_attr->mode = CMD_DIRECTION_MODE_OUTPUT;
                g_order_cnt = 0;
                g_switch_order_cnt = 0;
            }
            else if( !strncmp(pKey, "Other", strlen("Other")) )
            {
                snprintf(pInfo_ctrl->pOther_cmd_attr->cmd_name, MAX_MEMBER_NAME_LENGTH,
                    "%s", pInfo_ctrl->cmd_name);
                _refind_member_name(pInfo_ctrl->pOther_cmd_attr->cmd_name, strlen(pInfo_ctrl->pOther_cmd_attr->cmd_name));
                pInfo_ctrl->pAct_cmd_attr = pInfo_ctrl->pOther_cmd_attr;
                pInfo_ctrl->pAct_cmd_attr->mode = CMD_DIRECTION_MODE_OTHER;
                g_order_cnt = 0;
                g_switch_order_cnt = 0;
            }
            else
            {
                switch( pInfo_ctrl->state_type )
                {
                    case STATE_TYPE_GROUP:
                        {
                            CMD_MEMBER_GROUP    *pCur_group = &pInfo_ctrl->pAct_cmd_attr->pGroups[pInfo_ctrl->pAct_cmd_attr->total_groups];
                            snprintf(pCur_group->group_name, MAX_MEMBER_NAME_LENGTH, "%s", pKey);
                            _refind_member_name(pCur_group->group_name, strlen(pCur_group->group_name));
                        }
                        break;
                    case STATE_TYPE_SUB_GROUP:
                        {
                            CMD_MEMBER_GROUP        *pCur_group = &pInfo_ctrl->pAct_cmd_attr->pGroups[pInfo_ctrl->pAct_cmd_attr->total_groups];
                            CMD_MEMBER_SUB_GROUP    *pCur_subgroup = 0;

                            pCur_subgroup = &pCur_group->sub_groups[pCur_group->total_sub_groups];

                            snprintf(pCur_subgroup->subgroup_name, MAX_MEMBER_NAME_LENGTH, "%s", pKey);
                            _refind_member_name(pCur_subgroup->subgroup_name, strlen(pCur_subgroup->subgroup_name));
                        }
                        break;
                    case STATE_TYPE_SWITCH:
                        {
                            CMD_SWITCH_GROUP    *pCur_switch = &pInfo_ctrl->pAct_cmd_attr->pSwitchs[pInfo_ctrl->pAct_cmd_attr->total_switchs];
                            snprintf(pCur_switch->switch_name, MAX_MEMBER_NAME_LENGTH, "%s", pKey);
                            _refind_member_name(pCur_switch->switch_name, strlen(pCur_switch->switch_name));
                        }
                        break;
                    case STATE_TYPE_SWITCH_GROUP:
                        {
                            CMD_SWITCH_GROUP    *pCur_switch = &pInfo_ctrl->pAct_cmd_attr->pSwitchs[pInfo_ctrl->pAct_cmd_attr->total_switchs];
                            CMD_MEMBER_GROUP    *pCur_group = &pCur_switch->switch_groups[pCur_switch->total_switch_groups];
                            snprintf(pCur_group->group_name, MAX_MEMBER_NAME_LENGTH, "%s", pKey);
                            _refind_member_name(pCur_group->group_name, strlen(pCur_group->group_name));
                        }
                        break;
                    default:
                        _msg(SHOW_PARSING_MSG, "get obj ++++\n");
                        break;
                }
            }

            _get_info_from_json(pObj, pInfo_ctrl, layer+1);
            if( pInfo_ctrl->state_type == STATE_TYPE_UNKOWN )
            {
                pInfo_ctrl->pAct_cmd_attr->cumulative_struct_member_cnt = g_order_cnt;
                pInfo_ctrl->pAct_cmd_attr = 0;
            }
            break;
        case json_type_array:
            {
                int             i = 0;
                uint32_t        array_size = json_object_array_length(pObj);
                if( !strncmp(pKey, "Group", strlen("Group")+1) ||
                    !strncmp(pKey, "Group 1", strlen("Group 1")+1) ||
                    !strncmp(pKey, "Group 2", strlen("Group 2")+1) ||
                    !strncmp(pKey, "Group 3", strlen("Group 3")+1) ||
                    !strncmp(pKey, "Group 4", strlen("Group 4")+1) )
                {
                    for(i = 0; i < array_size; i++)
                    {
                        CMD_MEMBER_GROUP    *pCur_Groups = 0;
                        json_object         *pArray_obj = json_object_array_get_idx(pObj, i);

                        pCur_Groups = &pInfo_ctrl->pAct_cmd_attr->pGroups[pInfo_ctrl->pAct_cmd_attr->total_groups];

                        pInfo_ctrl->state_type = STATE_TYPE_GROUP;
                        pCur_Groups->order = g_order_cnt++;
                        _get_info_from_json(pArray_obj, pInfo_ctrl, layer+1);
                        pInfo_ctrl->pAct_cmd_attr->total_groups++;
                    }
                    pInfo_ctrl->state_type = STATE_TYPE_UNKOWN;
                }
                else if( !strncmp(pKey, "Sub_Group", strlen("Sub_Group")+1) )
                {
                    STATE_TYPE          pre_state_type = pInfo_ctrl->state_type;
                    for(i = 0; i < array_size; i++)
                    {
                        CMD_MEMBER_GROUP        *pCur_Groups = &pInfo_ctrl->pAct_cmd_attr->pGroups[pInfo_ctrl->pAct_cmd_attr->total_groups];
                        CMD_MEMBER_SUB_GROUP    *pCur_subgroup = 0;
                        json_object             *pArray_obj = json_object_array_get_idx(pObj, i);

                        pCur_subgroup = &pCur_Groups->sub_groups[pCur_Groups->total_sub_groups];
                        pInfo_ctrl->state_type = STATE_TYPE_SUB_GROUP;
                        pCur_subgroup->order = pCur_Groups->total_group_members++;

                        _get_info_from_json(pArray_obj, pInfo_ctrl, layer+1);
                        pCur_Groups->total_sub_groups++;
                    }
                    pInfo_ctrl->state_type = pre_state_type;
                }
                else if( !strncmp(pKey, "Switch", strlen("Switch")+1) )
                {
                    for(i = 0; i < array_size; i++)
                    {
                        json_object     *pArray_obj = json_object_array_get_idx(pObj, i);

                        pInfo_ctrl->state_type = STATE_TYPE_SWITCH;
                        _get_info_from_json(pArray_obj, pInfo_ctrl, layer+1);
                        pInfo_ctrl->pAct_cmd_attr->pSwitchs[pInfo_ctrl->pAct_cmd_attr->total_switchs].cumulative_struct_member_cnt = g_switch_order_cnt;
                        pInfo_ctrl->pAct_cmd_attr->total_switchs++;
                    }
                    pInfo_ctrl->state_type = STATE_TYPE_UNKOWN;
                }
                else if( !strncmp(pKey, "Switch_Group", strlen("Switch_Group")+1) ||
                         !strncmp(pKey, "Switch_Group 1", strlen("Switch_Group 1")+1) ||
                         !strncmp(pKey, "Switch_Group 2", strlen("Switch_Group 2")+1) )
                {
                    STATE_TYPE          pre_state_type = pInfo_ctrl->state_type;
                    for(i = 0; i < array_size; i++)
                    {
                        CMD_SWITCH_GROUP    *pCur_switch = &pInfo_ctrl->pAct_cmd_attr->pSwitchs[pInfo_ctrl->pAct_cmd_attr->total_switchs];
                        CMD_MEMBER_GROUP    *pCur_group = &pCur_switch->switch_groups[pCur_switch->total_switch_groups];

                        json_object         *pArray_obj = json_object_array_get_idx(pObj, i);

                        pInfo_ctrl->state_type = STATE_TYPE_SWITCH_GROUP;
                        pCur_group->order = g_switch_order_cnt++;
                        _get_info_from_json(pArray_obj, pInfo_ctrl, layer+1);
                        pCur_switch->total_switch_groups++;
                    }
                    pInfo_ctrl->state_type = pre_state_type;
                }

                _msg(SHOW_PARSING_MSG, "vlaue = array(%d)\n", array_size);
            }
            break;
        default:
            printf("*");
            break;
        }
    }
    return 0;
}

//=============================================================================
//				  Public Function Definition
//=============================================================================
uint32_t
Parse_Json(
    char        *pPath,
    CMD_ATTR    *pInput_cmd_attr,
    CMD_ATTR    *pOutput_cmd_attr,
    CMD_ATTR    *pOther_cmd_attr)
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
            info_ctrl.pOutput_cmd_attr = pOutput_cmd_attr;
            info_ctrl.pOther_cmd_attr  = pOther_cmd_attr;
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


