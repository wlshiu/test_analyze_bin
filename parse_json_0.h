#ifndef __parse_json_H_wmiuZleJ_ls7U_HnQG_sdOM_unRnBEFNAbz9__
#define __parse_json_H_wmiuZleJ_ls7U_HnQG_sdOM_unRnBEFNAbz9__

#ifdef __cplusplus
extern "C" {
#endif

#include "util.h"
//=============================================================================
//                Constant Definition
//=============================================================================
#define MAX_MEMBER_NAME_LENGTH          128

typedef enum MEMBER_TYPE_T
{
    MEMBER_TYPE_UNKNOW      = 0,
    MEMBER_TYPE_VOID,
    MEMBER_TYPE_UINT32,
    MEMBER_TYPE_UINT16,
    MEMBER_TYPE_UINT8,
    MEMBER_TYPE_FLOAT,
    MEMBER_TYPE_STRING,
    MEMBER_TYPE_BORDER_UINT8,
    MEMBER_TYPE_UINT32_ARRAY,
    MEMBER_TYPE_UINT16_ARRAY,
    MEMBER_TYPE_UINT8_ARRAY,
    MEMBER_TYPE_FLOAT_ARRAY,
    MEMBER_TYPE_STRING_LIST,
}MEMBER_TYPE;

typedef enum CMD_DIRECTION_MODE_T
{
    CMD_DIRECTION_MODE_OTHER        = 0,
    CMD_DIRECTION_MODE_INPUT,
    CMD_DIRECTION_MODE_OUTPUT,
}CMD_DIRECTION_MODE;
//=============================================================================
//                Macro Definition
//=============================================================================

//=============================================================================
//                Structure Definition
//=============================================================================
/**
 * Single member info
 */
typedef struct CMD_MEMBER_ITEM_T
{
    char            member_name[MAX_MEMBER_NAME_LENGTH];
    MEMBER_TYPE     member_type;
    uint32_t        byte_length;
    uint32_t        array_elm_cnt;
    uint32_t        order;

}CMD_MEMBER_ITEM;

typedef struct CMD_MEMBER_SUB_GROUP_T
{
    char                subgroup_name[MAX_MEMBER_NAME_LENGTH];
    CMD_MEMBER_ITEM     subgroup_cnt_member;

    uint32_t            total_subgroup_members;
    CMD_MEMBER_ITEM     subgroup_member[55];
    uint32_t            order;

}CMD_MEMBER_SUB_GROUP;

typedef struct CMD_MEMBER_GROUP_T
{
    char                group_name[MAX_MEMBER_NAME_LENGTH];
    CMD_MEMBER_ITEM     group_cnt_member;

    uint32_t            total_group_members;
    CMD_MEMBER_ITEM     group_member[55];
    uint32_t            order;

    uint32_t                total_sub_groups;
    CMD_MEMBER_SUB_GROUP    sub_groups[10];

}CMD_MEMBER_GROUP;

typedef struct CMD_SWITCH_GROUP_T
{
    char                switch_name[MAX_MEMBER_NAME_LENGTH];

    uint32_t            total_switch_members;
    CMD_MEMBER_ITEM     switch_members[50];

    uint32_t            total_switch_groups;
    CMD_MEMBER_GROUP    switch_groups[15];
    uint32_t            order;
    uint32_t            cumulative_struct_member_cnt;

}CMD_SWITCH_GROUP;

/**
 * A command attribute, ex. GetProfilesInput
 */
typedef struct CMD_ATTR_T
{
    uint32_t            pattern_type;
    CMD_DIRECTION_MODE  mode; // input or output

    char                cmd_name[MAX_MEMBER_NAME_LENGTH];
    uint32_t            total_members;
    CMD_MEMBER_ITEM     *pMembers;

    uint32_t            total_groups;
    CMD_MEMBER_GROUP    *pGroups;

    uint32_t            total_switchs;
    CMD_SWITCH_GROUP    *pSwitchs;

    uint32_t            cumulative_struct_member_cnt;

}CMD_ATTR;
//=============================================================================
//                Global Data Definition
//=============================================================================

//=============================================================================
//                Private Function Definition
//=============================================================================

//=============================================================================
//                Public Function Definition
//=============================================================================
uint32_t
Parse_Json(
    char        *pPath,
    CMD_ATTR    *pInput_cmd_attr,
    CMD_ATTR    *pOutput_cmd_attr,
    CMD_ATTR    *pOther_cmd_attr);


#ifdef __cplusplus
}
#endif

#endif
