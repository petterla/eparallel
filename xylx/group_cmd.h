#pragma once

#include "grouptype.h"

#pragma pack(push,1)

struct t_group_cmd_header
{
    uint32 magic; //0x20120604  host order	
    uint32 len;    
    uint16 cmd;
    uint64 seq;

};

// client->server
const uint16 k_gcmd_client_keepalive = 10000;
struct t_gcmd_client_keepalive
{
	t_group_cmd_header header;
};

// server->client
const uint16 k_gcmd_server_keepalive = 10001;
struct t_gcmd_server_keepalive
{
	t_group_cmd_header header;
};

// client->server
const uint16 k_gcmd_auth_request = 1001;
struct t_gcmd_auth_request
{
	t_group_cmd_header header;
	uint16 version;		//新增，客户端协议版本	
	t_group_token enc_token;//16字节，由xmpp返回
	uint16 reserved;
	t_uin client_uin;
};

// server->client
const uint16 k_gcmd_auth_challenge = 1002;
struct t_gcmd_auth_challenge
{
	t_group_cmd_header header;
	uint32 result; //新增，不支持客户端协议版本时，服务器端返回错误标志，否则返回成功标志
	uint64 challenge;
};

// client->server
const uint16 k_gcmd_challenge_response = 1003;
struct t_gcmd_challenge_response
{
	t_group_cmd_header header;
	uint8 hmac[20]; // hmac_sha(key=challenge,buffer = token);
	//uint16 version; //废弃，提前到t_gcmd_auth_request 中
	t_user_cookie cookie;
};

rc4_key = hmac_sha(key=token,buffer = challenge);

// server->client
const uint16 k_gcmd_auth_response = 1004;
struct t_gcmd_auth_response
{
	t_group_cmd_header header;
	uint32 result;
	vector<uint64> static_group_ids;	// 当前该用户所在的静态群
	vector<uint64> dynamic_group_ids;	// 当前用户所在的动态群
};

// client->server 开始接收消息
const uint16 k_gcmd_connect_request = 2000;
struct t_gcmd_connect_request
{
    t_group_cmd_header header;
    uint64 group_id;
};

// server->client
const uint16 k_gcmd_connect_response = 2001;
struct t_gcmd_connect_response
{
    t_group_cmd_header header;
    uint32 result;
};

// client->server
const uint16 k_gcmd_get_ginfo_request = 2002;
struct t_gcmd_get_ginfo_request
{
    t_group_cmd_header header;
    uint64 group_id;
};

// server->client
const uint16 k_gcmd_get_ginfo_response = 2003;
struct t_gcmd_get_ginfo_response
{
	t_group_cmd_header header;
	t_ginfo group_info;
};

// client->server
const uint16 k_gcmd_get_lastmsg_request = 2004;
struct t_gcmd_get_lastmsg_request
{
	t_group_cmd_header header;
	uint64 group_id;
};

// server->client
const uint16 k_gcmd_get_lastmsg_response = 2005;
struct t_gcmd_get_lastmsg_response
{
	t_group_cmd_header header;
	uint64 group_id;
	vector<t_gmessage> messages;
};

// client->server
const uint16 k_gcmd_get_lastcmd_request = 2006;
struct t_gcmd_get_lastcmd_request
{
	t_group_cmd_header header;
	uint64 group_id;
};

// server->client (这个包后面就是连续的notify)
const uint16 k_gcmd_get_lastcmd_response = 2007;
struct t_gcmd_get_lastcmd_response
{
	t_group_cmd_header header;
	uint64 group_id;
	uint32 cmd_count;
};

// client->server
const uint16 k_gcmd_create_request = 2008;
struct t_gcmd_create_request
{
    t_group_cmd_header header;
    uint8 group_type;
    uint8 group_verify;
    uint64 group_flag;
    uint64 group_attr;//新加
    wstring owner_name;//群主在此群的昵称
    wstring group_name; //群名称
    wstring group_intro;
};

// server->client
const uint16 k_gcmd_create_response = 2009;
struct t_gcmd_create_response
{
	t_group_cmd_header header;
	uint32 result;
	t_ginfo group_info;
};

// admin->server
const uint16 k_gcmd_dismiss_request = 2010;
struct t_gcmd_dismiss_request
{
	t_group_cmd_header header;
	uint64 group_id;
};

// server->admin
const uint16 k_gcmd_dismiss_response = 2011;
struct t_gcmd_dismiss_response
{
	t_group_cmd_header header;
	uint64 group_id;
	uint32 result;
};

// server->client
const uint16 k_gcmd_dismiss_notify = 3000;
struct t_gcmd_dismiss_notify
{
	t_group_cmd_header header;
	uint64 group_id;
};

// client->server
const uint16 k_gcmd_dismiss_confirm = 4000;
struct t_gcmd_dismiss_confirm
{
	t_group_cmd_header header;
};

// client->server
const uint16 k_gcmd_chat_request = 2012;
struct t_gcmd_chat_request
{
    t_group_cmd_header header;
    uint64 group_id;
    uint8 msg_type;
    uint8 msg_flag;
    uint32 msg_seq;
    wstring message;
};

// server->client
const uint16 k_gcmd_chat_response = 2013;
struct t_gcmd_chat_ack
{
    t_group_cmd_header header;
    uint64 group_id;
    uint32 msg_seq;
};

// server->client
const uint16 k_gcmd_chat_notify = 3001;
struct t_gcmd_chat_notify
{
	t_group_cmd_header header;
	uint64 group_id;
	t_gmessage message;
};

// client->server
const uint16 k_gcmd_chgstatus_request = 2014;
struct t_gcmd_chgstatus_request
{
	t_group_cmd_header header;
	uint8 status;
	vector<uint64> group_ids;
};

// server->client
const uint16 k_gcmd_chgstatus_response = 2015;
struct t_gcmd_chgstatus_response
{
	t_group_cmd_header header;
	uint32 result;
};

// server->client
const uint16 k_gcmd_chgstatus_notify = 3002;
struct t_gcmd_chgstatus_notify
{
	t_group_cmd_header header;
	uint8 status;
	t_user_cookie cookie;
	t_uin uin;
};

// client->server 1.name,2sex,3phone,4email,5comment,6msgset
const uint16 k_gcmd_chguinfo_request = 2016;
struct t_gcmd_chguinfo_request
{
	t_group_cmd_header header;
	uint64 group_id;
	vector<pair<uint8,wstring>> infos;
};

// server->client
const uint16 k_gcmd_chguinfo_response = 2017;
struct t_gcmd_chguinfo_response
{
	t_group_cmd_header header;
	uint64 group_id;
	uint32 result;
};

// server->client
const uint16 k_gcmd_chguinfo_notify = 3003;
struct t_gcmd_chguinfo_notify
{
	t_group_cmd_header header;
	uint64 group_id;
	t_uin uin;
	wstring name;
};

// client->server
const uint16 k_gcmd_getuinfo_request = 2018;
struct t_gcmd_getuinfo_request
{
	t_group_cmd_header header;
	uint64 group_id;
	t_uin uin;
};

// server->client
const uint16 k_gcmd_getuinfo_response = 2019;
struct t_gcmd_getuinfo_response
{
	t_group_cmd_header header;
	uint64 group_id;
	t_guser_info2 info;
};

// client->server
const uint16 k_gcmd_getuinfos_request = 2020;
struct t_gcmd_getuinfos_request
{
	t_group_cmd_header header;
	uint64 group_id;
};

// server->client
const uint16 k_gcmd_getuinfos_response = 2021;
struct t_gcmd_getuinfos_response
{
	t_group_cmd_header header;
	uint64 group_id;
	vector<t_guser_info> infos;
};

// client->server
const uint16 k_gcmd_quit_request = 2022;
struct t_gcmd_quit_request
{
	t_group_cmd_header header;
	uint64 group_id;
};

// server->client
const uint16 k_gcmd_quit_response = 2023;
struct t_gcmd_quit_response
{
	t_group_cmd_header header;
	uint64 group_id;	
	uint32 result;
};

// server->client
const uint16 k_gcmd_quit_notify = 3004;
struct t_gcmd_quit_notify
{
	t_group_cmd_header header;
	uint64 group_id;	
	t_uin quit_uin;
	wstring quit_name;
};

// client->server
const uint16 k_gcmd_setrole_request = 2024;
struct t_gcmd_setrole_request
{
	t_group_cmd_header header;
	uint64 group_id;
	t_uin target_uin;
	uint8 target_role;
};

// server->client
const uint16 k_gcmd_setrole_response = 2025;
struct t_gcmd_setrole_response
{
	t_group_cmd_header header;
	uint32 result;
};



//below start

// server->client
const uint16 k_gcmd_setrole_notify = 3005;
struct t_gcmd_setrole_notify
{
	t_group_cmd_header header;// 如果client是user，那么seq为0，否则seq不为0
	uint64 group_id;
	t_uin admin_uin;
	t_uin target_uin;
	uint8 target_role;
};

// admin->server
const uint16 k_gcmd_setrole_confirm = 4001;
struct t_gcmd_setrole_confirm
{
	t_group_cmd_header header;
};

// client->server 1group_flag,2group_name,3group_bulletin,4group_intro,5group_verify
const uint16 k_gcmd_chgginfo_request = 2026;
struct t_gcmd_chgginfo_request
{
	t_group_cmd_header header;
	uint64 group_id;
	vector<pair<uint8,wstring>> info;
};

// server->client
const uint16 k_gcmd_chgginfo_response = 2027;
struct t_gcmd_chgginfo_response
{
	t_group_cmd_header header;
	uint64 group_id;
	uint32 result;
};

// server->client
const uint16 k_gcmd_chgginfo_notify = 3006;
struct t_gcmd_chgginfo_notify
{
	t_group_cmd_header header;
	uint64 group_id;
	vector<pair<uint8,wstring>> info;
};

// client->server
const uint16 k_gcmd_kick_request = 2028;
struct t_gcmd_kick_request
{
	t_group_cmd_header header;
	uint64 group_id;
	t_uin taget_uin;
};

// server->client
const uint16 k_gcmd_kick_response = 2029;
struct t_gcmd_kick_response
{
	t_group_cmd_header header;
	uint64 group_id;	
	uint32 result;
};

// server->client
const uint16 k_gcmd_kick_notify = 3007;
struct t_gcmd_kick_notify
{
	t_group_cmd_header header;
	uint64 group_id;
	t_uin admin_uin;
	t_uin taget_uin;
};

// client->server // 静态群的被踢者回应此消息
const uint16 k_gcmd_kick_confirm = 4002;
struct t_gcmd_kick_confirm
{
	t_group_cmd_header header;
};

// client->server
const uint16 k_gcmd_join_request = 2030;
struct t_gcmd_join_request
{
	t_group_cmd_header header;
	uint64 group_id;
	wstring name;
	wstring reason;
};

// server->client
const uint16 k_gcmd_join_response = 2031;
struct t_gcmd_join_response
{
	t_group_cmd_header header;
	uint64 group_id;
	uint32 result;
};

// server->client join user 
const uint16 k_gcmd_join_notify = 3008;
struct t_gcmd_join_notify
{
	t_group_cmd_header header;
	uint64 group_id;
	bool allow;
};

const uint16 k_gcmd_join_confirm = 4003;
struct t_gcmd_join_confirm
{
	t_group_cmd_header header;
};

// server->admin
const uint16 k_gcmd_join_notify2 = 3009;
struct t_gcmd_join_notify2
{
	t_group_cmd_header header;
	uint64 group_id;
	t_uin uin;
	wstring name;
	wstring reason;
};

// server->client (发给其他用户)
const uint16 k_gcmd_newuser_notify = 3010;
struct t_gcmd_newuser_notify
{
	t_group_cmd_header header;
	uint64 group_id;
	t_uin uin;
	wstring name;
};

// admin->server
const uint16 k_gcmd_join_confirm2 = 4004;
struct t_gcmd_join_confirm2
{
	t_group_cmd_header header;
	bool allow;
};

// admin->server
const uint16 k_gcmd_invite_request = 2032;
struct t_gcmd_invite_request
{
	t_group_cmd_header header;
	uint64 group_id;
	t_uin invite_uin;
	wstring invite_reason;
};

// server->admin
const uint16 k_gcmd_invite_response = 2033;
struct t_gcmd_invite_response
{
	t_group_cmd_header header;
	uint32 result;
};

// server->invitee
const uint16 k_gcmd_invite_notify = 3011;
struct t_gcmd_invite_notify
{
	t_group_cmd_header header;
	uint64 group_id;
	wstring group_name;
	wstring invite_reason;
};

// invitee->server
const uint16 k_gcmd_invite_confirm = 4005;
struct t_gcmd_invite_comfirm
{
	t_group_cmd_header header;
	bool allow;
};

// server->admin
const uint16 k_gcmd_invite_notify2 = 3012;
struct t_gcmd_invite_notify2
{
	t_group_cmd_header header;
	uint64 group_id;
	wstring group_name;
	bool allow;
};

// client->server
const uint16 k_gcmd_file_share_request = 2034;
struct t_gcmd_file_share_request
{
	t_group_cmd_header header;
	uint64 group_id;
	t_offline_info offline_info;
	t_task_id task_id;
	uint64 total_size;
	uint32 file_count;
	uint32 path_count;
	bool folder;
	wstring task_name;
	wstring task_attr;
	wstring task_desc;
	string task_seed;	// t_u2u_recv_task_info
};

// server->client
const uint16 k_gcmd_file_share_response = 2035;
struct t_gcmd_file_share_response
{
	t_group_cmd_header header;
	uint32 result;
};

// server->client
const uint16 k_gcmd_file_share_notify = 3013;
struct t_gcmd_file_share_notify
{
	t_group_cmd_header header;
	uint64 group_id;
	t_gfile_info file_info;
};
//
// client->server
const uint16 k_gcmd_file_finish_request = 2036;
struct t_gcmd_file_finish_request
{
	t_group_cmd_header header;
	uint64 group_id;
	t_task_id task_id;
};

// server->client
const uint16 k_gcmd_file_finish_response = 2037;
struct t_gcmd_file_finish_response
{
	t_group_cmd_header header;
	uint32 result;
};

// client->server
const uint16 k_gcmd_get_files_request = 2038;
struct t_gcmd_get_files_request
{
	t_group_cmd_header header;
	uint64 group_id;
	wstring filter;	// 一个sql语句，按时间过滤，发布者过滤，文件名过滤，描述过滤
};

// server->client
const uint16 k_gcmd_get_files_response = 2039;
struct t_gcmd_get_files_response
{
	t_group_cmd_header header;
	uint64 group_id;
	vector<t_gfile_info> files;
};

// client->server (只有owner或者admin可以修改共享文件的描述)
// 1: task_name; 2: task_desc
const uint16 k_gcmd_chg_file_request = 2040;
struct t_gcmd_chg_file_request
{
	t_group_cmd_header header;
	uint64 group_id;
	t_task_id task_id;
	vector<pair<uint8,wstring> > gfile_infos;
};

// server->client
const uint16 k_gcmd_chg_file_response = 2041;
struct t_gcmd_chg_file_response
{
	t_group_cmd_header header;
	uint32 result;
};

// client->server
const uint16 k_gcmd_get_file_request = 2042;
struct t_gcmd_get_file_request
{
	t_group_cmd_header header;
	uint64 group_id;
	t_task_id task_id;
};

// server->client
const uint16 k_gcmd_get_file_response = 2043;
struct t_gcmd_get_file_response
{
	t_group_cmd_header header;
	uint64 group_id;
	t_task_id task_id;
	string task_seed;
};

// client->server
const uint16 k_gcmd_get_history_request = 2044;
struct t_gcmd_get_history_request
{
	t_group_cmd_header header;
	uint64 group_id;
	wstring filter;
};

// server->client
const uint16 k_gcmd_get_history_response = 2045;
struct t_gcmd_get_history_response
{
	t_group_cmd_header header;
	uint64 group_id;
	vector<t_gmessage> messages;
};

// server->client
const uint16 k_gcmd_kickoff_notify = 3014;
struct t_gcmd_kickoff_notify
{
	t_group_cmd_header header;
	uint32 ip;//被哪个IP踢掉
};

// client->server
const uint16 k_gcmd_get_usergroups_request = 2046
struct t_gcmd_get_usergroups_request
{
	t_group_cmd_header header;
	t_uin uin;	
}

// server->client
const k_gcmd_get_usergroups_response = 2047
struct t_gcmd_get_usergroups_response
{
	t_group_cmd_header header;
	t_uin uin;	
	vector<uint64> static_group_ids;	// 当前该用户所在的静态群
}


#pragma pack(pop)

